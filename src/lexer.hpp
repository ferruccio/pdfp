#ifndef LEXER_HPP
#define LEXER_HPP

#include <tuple>

#include "tools.hpp"

namespace pdf {

    using tools::slice;

    enum class token_type {
        nothing, keyword, name, string, hexstring, integer, real,
        array_begin, array_end, dict_begin, dict_end
    };

    struct token {
        token(token_type type, slice value) : type(type), value(value) {}

        const token_type type;
        const slice value;
    };

    // Return the first token in a slice and the remainder of the slice.
    auto next_token(slice src) noexcept -> std::tuple<token, slice>;

}

#endif
