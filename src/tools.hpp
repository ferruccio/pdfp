#ifndef PDF_TOOLS_HPP
#define PDF_TOOLS_HPP

#include <algorithm>
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <stdexcept>
#include <string>

namespace pdf { namespace tools {

    using length_type = unsigned int;

    /*
        A slice is a non-owning reference to a block of contiguous objects.
    */
    template <typename T>
    class slice {
    public:
        using cptr = T const * const;

        slice<T>(cptr data, length_type length) : _data(data), _length(length) {
            assert(data != nullptr);
        }

        slice<T>(cptr begin, cptr end) : _data(begin), _length(end - begin) {
            assert(begin != nullptr);
            assert(end != nullptr);
            assert(begin <= end);
        }

        slice<T>(const slice<T>& src) : _data(src.data()), _length(src.length()) {}

        auto data() const noexcept -> cptr { return _data; }
        auto length() const noexcept -> length_type { return _length; }
        auto empty() const noexcept -> bool { return length() == 0; }

        auto left(length_type length) const noexcept -> slice<T> {
            return slice<T>(_data, std::min(_length, length));
        }

        auto remove_left(length_type length) const noexcept -> slice<T> {
            length = std::min(_length, length);
            return slice<T>(_data + length, _length - length);
        }

        auto operator==(const slice<T>& rhs) const noexcept -> bool {
            if (length() != rhs.length()) return false;
            if (data() == rhs.data()) return true;
            const auto end = data() + length();
            return std::mismatch(data(), end, rhs.data()).first == end;
        }

        auto operator!=(const slice<T>& rhs) const noexcept -> bool {
            return !(*this == rhs);
        }

        auto first() const -> T {
            if (empty()) throw std::runtime_error("pdf::tools::slice::first() - empty slice");
            return data()[0];
        }

        auto rest() const noexcept -> slice<T> {
            return empty() ? *this : slice<T>(data() + 1, length() - 1);
        }

        auto starts_with(slice<T> s) const noexcept -> bool {
            return left(s.length()) == s;
        }

    private:
        cptr _data;
        const length_type _length;
    };

    class char_slice : public slice<char> {
    public:
        char_slice(const char* str) : slice<char>(str, std::strlen(str)) {}
    };

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
