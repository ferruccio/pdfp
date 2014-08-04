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

    inline auto operator<<(std::ostream& os, slice s) -> std::ostream& {
        for (char c : s)
            os << c;
        return os;
    }

    /*
        An atom table provides a unique mapping from slices to simple tokens (atom_type).
        It also provides support for slices with predefined values for tokens.
        Assumption: all slices stored in a atom table have longer lifetimes than
        the atom table itself, otherwise bad things will happen...
    */
    using atom_type = unsigned int;

    class atom_table {
    public:
        atom_table(atom_type first = 0) : next(first) {}

        atom_table(std::initializer_list<std::pair<slice, atom_type>> init) {
            for (auto kv : init)
                add(kv.first, kv.second);
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
        atom_type next;

        auto haskey(slice key) const noexcept -> bool {
            return table.find(key) != table.end();
        }
    };

    enum class variant_type {
        null, keyword, boolean, integer, real, name, string, hexstring, array, dict
    };

    //
    //  variants are used to hold any PDF item
    //
    class variant {
    public:
        variant() : _type(variant_type::null) {}
        ~variant() { nullify(); }

        variant(const variant& rhs) { *this = rhs; }

        variant(variant&& rhs) {
            nullify();
            switch (rhs.type()) {
                case variant_type::null: break;
                case variant_type::keyword: // fall through
                case variant_type::name: _var.atom = rhs._var.atom; break;
                case variant_type::boolean: _var.bool_val = rhs._var.bool_val; break;
                case variant_type::integer: _var.int_val = rhs._var.int_val; break;
                case variant_type::real: _var.real_val = rhs._var.real_val; break;
                case variant_type::string: // fall through
                case variant_type::hexstring: _var.ref = rhs._var.ref; break;
                // move
                case variant_type::array:
                    _var.array = rhs._var.array;
                    rhs._var.array = nullptr;
                    rhs._type = variant_type::null;
                    break;
                case variant_type::dict:
                    _var.dict = rhs._var.dict;
                    rhs._var.dict = nullptr;
                    rhs._type = variant_type::null;
                    break;
            }
        }

        auto operator=(const variant& rhs) noexcept -> variant& {
            nullify();
            switch (rhs.type()) {
                case variant_type::null: break;
                case variant_type::keyword: // fall through
                case variant_type::name: _var.atom = rhs._var.atom; break;
                case variant_type::boolean: _var.bool_val = rhs._var.bool_val; break;
                case variant_type::integer: _var.int_val = rhs._var.int_val; break;
                case variant_type::real: _var.real_val = rhs._var.real_val; break;
                case variant_type::string: // fall through
                case variant_type::hexstring: _var.ref = rhs._var.ref; break;
                // copy
                case variant_type::array:
                    _var.array = new std::vector<variant>(rhs._var.array->size());
                    _type = variant_type::array;
                    std::copy(rhs._var.array->begin(), rhs._var.array->end(), _var.array->begin());
                    break;
                case variant_type::dict:
                    _var.dict = new std::map<atom_type, variant>();
                    _type = variant_type::dict;
                    _var.dict->insert(rhs._var.dict->begin(), rhs._var.dict->end());
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

        static auto make_array() noexcept -> variant {
            variant v(variant_type::array);
            v._var.array = new std::vector<variant>();
            return v;
        }

        static auto make_dict() noexcept -> variant {
            variant v(variant_type::dict);
            v._var.dict = new std::map<atom_type, variant>();
            return v;
        }

        auto type() const noexcept -> variant_type { return _type; }

    private:
        variant(variant_type type) : _type(type) {}
        variant(atom_type value, variant_type type) : _type(type) { _var.atom = value; }
        variant(bool value) : _type(variant_type::boolean) { _var.bool_val = value; }
        variant(int value) : _type(variant_type::integer) { _var.int_val = value; }
        variant(double value) : _type(variant_type::real) { _var.real_val = value; }
        variant(slice value, variant_type type) : _type(type) { _var.ref = value; }

    private:
        union var {
            var() {}
            ~var() {}

            slice ref;
            atom_type atom;
            bool bool_val;
            int int_val;
            double real_val;
            std::vector<variant>* array;
            std::map<atom_type, variant>* dict;
        };

        void nullify() {
            switch (type()) {
                case variant_type::array: delete _var.array; break;
                case variant_type::dict: delete _var.dict; break;
                default: break;
            }
            _type = variant_type::null;
        }

        variant_type _type;
        var _var;
    };

}}

#endif
