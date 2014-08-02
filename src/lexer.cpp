#include "lexer.hpp"

//
//  Lexer support functions
//
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

    auto isnumberchar(char ch) noexcept -> bool {
        switch (ch) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '+': case '-': case '.': return true;
            default: return false;
        }
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

    auto number(slice src) noexcept -> std::tuple<token, slice> {
        auto tok = src.take_while(isnumberchar);
        return make_tuple(token(token_type::name, tok), src.skip(tok.length()));
    }

    auto string(slice src) noexcept -> std::tuple<token, slice> {
        unsigned int nesting = 0;
        auto tok = src.take_while([&nesting](char ch)->bool {
            switch (ch) {
                case '(': ++nesting; // fall through
                default: return false;
                case ')': return --nesting == 0;
            }
        });
        return make_tuple(token(token_type::name, tok), src.skip(tok.length()));
    }

}

//
//  next_token() takes a slice and returns the first token in the slice
//  as well as the slice remaining after the token.
//
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
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '+': case '-': case '.': return number(src);
            case '(': return string(src);
        }
        return make_tuple(token(token_type::string, src), src);
    }

}
