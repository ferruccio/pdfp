#include "lexer.hpp"
namespace {

    using pdf::token;
    using pdf::token_type;
    using pdf::tools::slice;
    using std::make_tuple;

    auto iswhitespace(char ch) noexcept -> bool {
        switch (ch) {
            case 0x00: case 0x09: case 0x0a:
            case 0x0c: case 0x0d: case 0x20: return true;
            default: return false;
        }
    }

    auto isdelimiter(char ch) noexcept -> bool {
        switch (ch) {
            case '(': case ')': case '<': case '>':
            case '[': case ']': case '{': case '}':
            case '/': case '%': return true;
            default: return false;
        }
    }

    auto iseol(char ch) noexcept -> bool {
        return ch == 0x0a || ch == 0x0d;
    }

    auto isnamechar(char ch) noexcept -> bool {
        return !iswhitespace(ch) && !isdelimiter(ch);
    }

    auto skipws(pdf::slice src) noexcept -> pdf::slice {
        while (!src.empty() && iswhitespace(*src))
            src = src.rest();
        return src;
    }

    auto name(slice src) noexcept -> std::tuple<token, slice> {
        auto tok = src.take_while(isnamechar);
        return make_tuple(token(token_type::name, tok), src.skip(tok.length()));
    }

}

namespace pdf {

    using std::make_tuple;

    auto next_token(slice src) noexcept -> std::tuple<token, slice> {
        for (;;) {
            if ((src = skipws(src)).empty())
                return make_tuple(token(token_type::nothing, src), src);
            if (*src != '%')
                break;
            src = src.skip_until(iseol);
        }

        switch (*src) {
            case '/': return name(src.rest());
        }
        return make_tuple(token(token_type::string, src), src);
    }

}
