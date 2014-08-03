#ifndef LEXER_HPP
#define LEXER_HPP

#include <ostream>
#include <tuple>

#include "tools.hpp"

namespace pdf {

    using tools::slice;

    enum class token_type {
        nothing, bad_token,
        keyword, name, string, hexstring, number,
        array_begin, array_end, dict_begin, dict_end
    };

    auto operator<<(std::ostream& os, token_type tt) noexcept -> std::ostream&;

    class token {
    public:
        token() : _type(token_type::nothing), _value(slice("")) {}
        token(token_type type, slice value) : _type(type), _value(value) {}

        auto type() const noexcept -> token_type { return _type; }
        auto value() const noexcept -> slice { return _value; }

    private:
        token_type _type;
        slice _value;
    };

    // Return the first token in a slice and the remainder of the slice.
    auto next_token(slice src) noexcept -> std::tuple<token, slice>;

}

#endif
