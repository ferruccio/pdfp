#ifndef TOOLS_SLICE_HPP
#define TOOLS_SLICE_HPP

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <functional>

namespace pdf { namespace tools {

    /*
        A slice is a non-owning immutable reference to a sequence of chars.
    */
    class slice {
    public:
        using cptr = const char*;

        slice(cptr str) {
            assert(str != nullptr);
            _begin = _end = str;
            while (*_end)
                ++_end;
        }

        slice(cptr begin, cptr end) {
            // clang 3.5: This assertion fails on Linux when running unit tests, but works on OS X.
            //assert(begin != nullptr);
            assert(end != nullptr);
            assert(begin <= end);
            _begin = begin;
            _end = end;
        }

        slice(cptr begin, unsigned int length) {
            assert(begin != nullptr);
            _begin = begin;
            _end = _begin + length;
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
            if (empty()) throw std::runtime_error("slice::first: empty slice");
            return *_begin;
        }

        auto rest() const noexcept -> slice {
            return empty() ? *this : slice(_begin + 1, _end);
        }

        auto operator[](int i) const noexcept -> char {
            return _begin[i];
        }

        auto starts_with(slice s) const noexcept -> bool {
            return left(s.length()) == s;
        }

        auto skip(unsigned int n) const noexcept -> slice {
            return slice(begin() + std::min(n, length()), end());
        }

        auto skip(slice s) const noexcept -> slice {
            return s.begin() >= this->begin() && s.end() <= this->end()
                ? slice(s.end(), this->end())
                : *this;
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

}}

#endif
