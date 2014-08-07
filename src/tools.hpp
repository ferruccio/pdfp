#ifndef PDF_TOOLS_HPP
#define PDF_TOOLS_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <map>
#include <ostream>
#include <unordered_map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace pdf { namespace tools {

    /*
        A slice is a non-owning immutable reference to a sequence of chars.
    */
    class slice {
    public:
        using cptr = const char*;

        slice(const char* str) : _begin(str), _end(str + std::strlen(str)) {}

        slice(cptr begin, cptr end) : _begin(begin), _end(end) {
            assert(begin != nullptr);
            assert(end != nullptr);
        }

        slice(cptr begin, unsigned int length) : _begin(begin), _end(begin + length) {
            assert(begin != nullptr);
        }

        slice(const slice& src) : _begin(src.begin()), _end(src.end()) {}

        auto begin() const noexcept -> cptr { return _begin; }
        auto end() const noexcept -> cptr { return _end; }
        auto length() const noexcept -> unsigned int { return _end - _begin; }
        auto empty() const noexcept -> bool { return _begin == _end; }

        auto left(unsigned int length) const noexcept -> slice {
            return slice(_begin, _begin + std::min(this->length(), length));
        }

        auto remove_left(unsigned int length) const noexcept -> slice {
            return slice(_begin + std::min(this->length(), length), _end);
        }

        auto operator==(const slice& rhs) const noexcept -> bool {
            if (length() != rhs.length()) return false;
            if (begin() == rhs.begin()) return true;
            return std::mismatch(_begin, _end, rhs.begin()).first == _end;
        }

        auto operator!=(const slice& rhs) const noexcept -> bool {
            return !(*this == rhs);
        }

        auto operator*() const noexcept -> char { return *_begin; }

        auto first() const -> char {
            if (empty()) throw std::runtime_error("pdf::tools::slice::first() - empty slice");
            return *_begin;
        }

        auto rest() const noexcept -> slice {
            return empty() ? *this : slice(_begin + 1, _end);
        }

        auto starts_with(slice s) const noexcept -> bool {
            return left(s.length()) == s;
        }

        auto skip(unsigned int n) const noexcept -> slice {
            return slice(begin() + std::min(n, length()), end());
        }

        auto skip_until(std::function<auto (char)->bool> pred) const noexcept -> slice {
            auto p = begin();
            while (p != end() && !pred(*p))
                ++p;
            return slice(p, end());
        }

        auto skip_while(std::function<auto (char)->bool> pred) const noexcept -> slice {
            auto p = begin();
            while (p != end() && pred(*p))
                ++p;
            return slice(p, end());
        }

        auto take_until(std::function<auto (char)->bool> pred) const noexcept -> slice {
            auto p = begin();
            while (p != end() && !pred(*p))
                ++p;
            return slice(begin(), p);
        }

        auto take_while(std::function<auto (char)->bool> pred) const noexcept -> slice {
            auto p = begin();
            while (p != end() && pred(*p))
                ++p;
            return slice(begin(), p);
        }

        auto find_last(slice what) const noexcept -> slice {
            auto p = std::find_end(begin(), end(), what.begin(), what.end());
            return p == end()
                ? slice("")
                : slice(p, end());
        }

    private:
        cptr _begin;
        cptr _end;
    };

    auto operator<<(std::ostream& os, slice s) -> std::ostream&;

    /*
        An atom table provides a unique mapping from slices to simple tokens (atom_type).
        It also provides support for slices with predefined values for tokens.
        Assumption: all slices stored in a atom table have longer lifetimes than
        the atom table itself, otherwise bad things will happen...
    */
    using atom_type = unsigned int;

    class atom_table {
    public:
        atom_table() {}

        atom_table(std::initializer_list<std::pair<slice, atom_type>> init) {
            for (auto kv : init)
                add(kv.first, kv.second);
        }

        atom_table(const atom_table& at) {
            table.insert(at.table.begin(), at.table.end());
        }

        auto add(slice key) noexcept -> atom_type {
            auto value = table.find(key);
            return value != table.end() ? value->second : table[key] = next++;
        }

        void add(slice key, atom_type value) {
            if (haskey(key)) throw std::runtime_error("pdf::tools::atom_table::add() - duplicate key");
            table[key] = value;
        }

        auto operator[](slice key) noexcept -> atom_type {
            return add(key);
        }

        // brute force reverse lookup: for debugging purposes only
        auto lookup(atom_type value) const noexcept -> slice {
            for (const auto& kv : table)
                if (kv.second == value)
                    return kv.first;
            return "???";
        }

    private:
        struct hash {
            auto operator()(slice s) const noexcept -> std::size_t {
                std::size_t hash = 0;
                for (char c : s)
                    hash = hash * 101 + c;
                return hash;
            }
        };

        std::unordered_map<slice, atom_type, hash> table;
        atom_type next = 0x10000;

        auto haskey(slice key) const noexcept -> bool {
            return table.find(key) != table.end();
        }
    };

    enum class variant_type {
        nothing, // used to indicate "no object" vs. null which is a PDF null object
        null, keyword, boolean, integer, real, name, string, hexstring, array, dict, ref
    };

    struct objref {
        objref(int id = 0, int gen = 0) : id(id), gen(gen) {}
        int id, gen;
    };

    /*
        variants are used to hold any PDF item
    */
    class variant;

    /*
        A variant_proxy is a temporary object used to associate a variant with an
        atom_table so that operator<< can print textual names and keywords.
        It's meant to be used for unit testing & debugging.
    */
    struct variant_proxy {
        variant_proxy(const variant& var, const atom_table& atoms) : var(var), atoms(atoms) {}

        const variant& var;
        const atom_table& atoms;
    };

    class variant {
    public:
        using array_type = std::vector<variant>;
        using dict_type = std::map<atom_type, variant>;

        variant() {}
        ~variant() { destroy(); }

        variant(const variant& rhs) { *this = rhs; }

        variant(variant&& rhs) {
            destroy();
            switch (rhs.type()) {
                case variant_type::array:
                    _var.array = rhs._var.array;
                    _type = rhs._type;
                    rhs._var.array = nullptr;
                    rhs._type = variant_type::null;
                    break;
                case variant_type::dict:
                    _var.dict = rhs._var.dict;
                    _type = rhs._type;
                    rhs._var.dict = nullptr;
                    rhs._type = variant_type::null;
                    break;
                default:
                    assign(rhs);
                    break;
            }
        }

        // create a variant proxy bound to this variant and an atom_table
        auto operator()(const atom_table& atoms) const noexcept -> variant_proxy {
            return variant_proxy(*this, atoms);
        }

        auto operator=(const variant& rhs) noexcept -> variant& {
            destroy();
            switch (rhs.type()) {
                case variant_type::array:
                    _var.array = new array_type(rhs._var.array->size());
                    _type = variant_type::array;
                    std::copy(rhs._var.array->begin(), rhs._var.array->end(), _var.array->begin());
                    break;
                case variant_type::dict:
                    _var.dict = new dict_type();
                    _type = variant_type::dict;
                    _var.dict->insert(rhs._var.dict->begin(), rhs._var.dict->end());
                    break;
                default:
                    assign(rhs);
                    break;
            }
            return *this;
        }

        static auto make_null() noexcept -> variant {
            return variant(variant_type::null);
        }

        static auto make_keyword(atom_type keyword) noexcept -> variant {
            return variant(keyword, variant_type::keyword);
        }

        static auto make_name(atom_type name) noexcept -> variant {
            return variant(name, variant_type::name);
        }

        static auto make_boolean(bool value) noexcept -> variant {
            return variant(value);
        }

        static auto make_integer(int value) noexcept -> variant {
            return variant(value);
        }

        static auto make_real(double value) noexcept -> variant {
            return variant(value);
        }

        static auto make_string(slice s) noexcept -> variant {
            return variant(s, variant_type::string);
        }

        static auto make_hexstring(slice s) noexcept -> variant {
            return variant(s, variant_type::hexstring);
        }

        static auto make_ref(int id, int gen) noexcept -> variant {
            return variant(objref(id, gen));
        }

        static auto make_array() -> variant {
            variant v(variant_type::array);
            v._var.array = new array_type();
            return v;
        }

        static auto make_dict() -> variant {
            variant v(variant_type::dict);
            v._var.dict = new dict_type();
            return v;
        }

        auto is_nothing() const noexcept -> bool { return _type == variant_type::nothing; }
        auto is_null() const noexcept -> bool { return _type == variant_type::null; }
        auto is_keyword() const noexcept -> bool { return _type == variant_type::keyword; }
        auto is_name() const noexcept -> bool { return _type == variant_type::name; }
        auto is_boolean() const noexcept -> bool { return _type == variant_type::boolean; }
        auto is_integer() const noexcept -> bool { return _type == variant_type::integer; }
        auto is_real() const noexcept -> bool { return _type == variant_type::real; }
        auto is_string() const noexcept -> bool { return _type == variant_type::string; }
        auto is_hexstring() const noexcept -> bool { return _type == variant_type::hexstring; }
        auto is_array() const noexcept -> bool { return _type == variant_type::array; }
        auto is_dict() const noexcept -> bool { return _type == variant_type::dict; }
        auto is_ref() const noexcept -> bool { return _type == variant_type::ref; }
        auto is_numeric() const noexcept -> bool { return is_integer() || is_real(); }
        auto is_text() const noexcept -> bool { return is_string() || is_hexstring(); }

        auto get_keyword() const -> atom_type {
            if (!is_keyword()) throw std::runtime_error("pdf::tools::variant: not a keyword");
            return _var.atom;
        }

        auto get_name() const -> atom_type {
            if (!is_name()) throw std::runtime_error("pdf::tools::variant: not a name");
            return _var.atom;
        }

        auto get_boolean() const -> bool {
            if (!is_boolean()) throw std::runtime_error("pdf::tools::variant: not a boolean");
            return _var.bool_val;
        }

        auto get_integer() const -> int {
            if (!is_integer()) throw std::runtime_error("pdf::tools::variant: not an integer");
            return _var.int_val;
        }

        auto get_real() const -> double {
            if (!is_real()) throw std::runtime_error("pdf::tools::variant: not a real");
            return _var.real_val;
        }

        auto get_string() const -> slice {
            if (!is_string()) throw std::runtime_error("pdf::tools::variant: not a string");
            return _var.str;
        }

        auto get_hexstring() const -> slice {
            if (!is_hexstring()) throw std::runtime_error("pdf::tools::variant: not a hexstring");
            return _var.str;
        }

        auto get_ref() const -> objref {
            if (!is_ref()) throw std::runtime_error("pdf::tools::variant: not a ref");
            return _var.ref;
        }

        auto get_array() const -> array_type& {
            if (!is_array()) throw std::runtime_error("pdf::tools::variant: not an array");
            return *_var.array;
        }

        auto get_dict() const -> dict_type& {
            if (!is_dict()) throw std::runtime_error("pdf::tools::variant: not a dict");
            return *_var.dict;
        }

        auto type() const noexcept -> variant_type { return _type; }

        auto size() const noexcept -> std::size_t {
            switch (type()) {
                case variant_type::array: return _var.array->size();
                case variant_type::dict: return _var.dict->size();
                default: return 0;
            }
        }

        auto haskey(atom_type key) const -> bool {
            if (!is_dict()) throw std::runtime_error("pdf::tools::variant: not a dict");
            return (*_var.dict).find(key) != (*_var.dict).end();
        }

        auto operator[](int index) const -> variant {
            auto array = get_array();
            if (index < 0 || index > array.size())
                throw std::runtime_error("pdf::tools::variant: bad array index");
            return array[index];
        }

        auto operator[](atom_type key) const -> variant {
            return get_dict()[key];
        }

    private:
        variant(variant_type type) : _type(type) {}
        variant(atom_type value, variant_type type) : _type(type) { _var.atom = value; }
        variant(bool value) : _type(variant_type::boolean) { _var.bool_val = value; }
        variant(int value) : _type(variant_type::integer) { _var.int_val = value; }
        variant(double value) : _type(variant_type::real) { _var.real_val = value; }
        variant(slice value, variant_type type) : _type(type) { _var.str = value; }
        variant(objref value) : _type(variant_type::ref) { _var.ref = value; }

    private:
        union var {
            var() {}
            ~var() {}

            slice str;
            atom_type atom;
            bool bool_val;
            int int_val;
            double real_val;
            array_type* array;
            dict_type* dict;
            objref ref;
        };

        void destroy() {
            switch (type()) {
                case variant_type::array: delete _var.array; break;
                case variant_type::dict: delete _var.dict; break;
                default: break;
            }
            _type = variant_type::null;
        }

        void assign(const variant& rhs) {
            switch (rhs.type()) {
                case variant_type::nothing: // fall through
                case variant_type::null: break;
                case variant_type::keyword: // fall through
                case variant_type::name: _var.atom = rhs._var.atom; break;
                case variant_type::boolean: _var.bool_val = rhs._var.bool_val; break;
                case variant_type::integer: _var.int_val = rhs._var.int_val; break;
                case variant_type::real: _var.real_val = rhs._var.real_val; break;
                case variant_type::string: // fall through
                case variant_type::hexstring: _var.str = rhs._var.str; break;
                case variant_type::ref: _var.ref = rhs._var.ref; break;
                // does not handle complex types (array, dict)
                default: return;
            }
            _type = rhs._type;
        }

        variant_type _type = variant_type::nothing;
        var _var;
    };

    auto operator<<(std::ostream& os, variant v) -> std::ostream&;
    auto operator<<(std::ostream& os, variant_proxy vp) -> std::ostream&;

}}

#endif
