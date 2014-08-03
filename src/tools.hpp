#ifndef PDF_TOOLS_HPP
#define PDF_TOOLS_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <ostream>
#include <unordered_map>
#include <stdexcept>
#include <string>

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

    private:
        cptr _begin;
        cptr _end;
    };

    auto operator<<(std::ostream& os, slice s) -> std::ostream& {
        for (char c : s)
            os << c;
        return os;
    }

    /*
        An atom table provides a unique mapping from strings to simple tokens (atom_type).
        It also provides support for strings with predefined values for tokens.
    */
    using atom_type = unsigned int;

    class atom_table {
    public:
        atom_table(atom_type first = 0) : next(first) {}

        auto add(const std::string& key) noexcept -> atom_type {
            auto value = table.find(key);
            return value != table.end() ? value->second : table[key] = next++;
        }

        void add(const std::string& key, atom_type value) {
            if (haskey(key)) throw std::runtime_error("pdf::tools::atom_table::add() - duplicate key");
            table[key] = value;
        }

        auto operator[](const std::string& key) noexcept -> atom_type {
            return add(key);
        }

    private:
        std::unordered_map<std::string, atom_type> table;
        atom_type next;

        auto haskey(const std::string& key) const noexcept -> bool {
            return table.find(key) != table.end();
        }
    };

}}

#endif
