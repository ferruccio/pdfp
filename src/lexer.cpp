#include "lexer.hpp"

//
//  Lexer support functions
//
namespace {

    using pdf::token;
    using pdf::token_type;
    using pdf::tools::slice;
    using std::make_tuple;
    using std::tuple;

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

    auto isbreak(char ch) noexcept -> bool {
        return iswhitespace(ch) || isdelimiter(ch);
    }

    auto iseol(char ch) noexcept -> bool {
        return ch == 0x0a || ch == 0x0d;
    }

    auto isnumeric(char ch) noexcept -> bool {
        switch (ch) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '+': case '-': case '.': return true;
            default: return false;
        }
    }

    auto skipws(slice src) noexcept -> slice {
        while (!src.empty() && iswhitespace(*src))
            src = src.rest();
        return src;
    }

    auto name(slice src) noexcept -> tuple<token, slice> {
        auto tok = src.take_until(isbreak);
        return make_tuple(token(token_type::name, tok), src.skip(tok.length()));
    }

    auto number(slice src) noexcept -> tuple<token, slice> {
        auto tok = src.take_while(isnumeric);
        return make_tuple(token(token_type::number, tok), src.skip(tok.length()));
    }

    auto string(slice src) noexcept -> tuple<token, slice> {
        unsigned int nesting = 0;
        bool quote = false;
        auto tok = src.take_until([&](char ch)->bool {
            if (quote) {
                quote = false;
                return false;
            }
            switch (ch) {
                case '(': ++nesting; // fall through
                default: return false;
                case ')': return --nesting == 0;
                case '\\': quote = true; return false;
            }
        });
        return make_tuple(token(token_type::string, tok.rest()), src.skip(tok.length() + 1));
    }

    auto lbrack(slice src) noexcept -> tuple<token, slice> {
        auto src0 = src.rest();
        if (src0.empty())
            return make_tuple(token(token_type::bad_token, src), src);
        if (*src0 == '<')
            return make_tuple(token(token_type::dict_begin, src.left(2)), src0.rest());
        auto tok = src.take_until([](char ch)->bool { return ch == '>'; });
        return make_tuple(token(token_type::hexstring, tok.rest()), src.skip(tok.length() + 1));
    }

    auto rbrack(slice src) noexcept -> tuple<token, slice> {
        auto src0 = src.rest();
        return src0.empty() || *src0 != '>'
            ? make_tuple(token(token_type::bad_token, src), src)
            : make_tuple(token(token_type::dict_end, src.left(2)), src0.rest());
    }

    auto single_char(slice src, token_type type) noexcept -> tuple<token, slice> {
        return make_tuple(token(type, src.left(1)), src.rest());
    }

    auto keyword(slice src) noexcept -> tuple<token, slice> {
        auto tok = src.take_until(isbreak);
        return tok.length() == 0
            ? make_tuple(token(token_type::bad_token, src), src)
            : make_tuple(token(token_type::keyword, tok), src.skip(tok.length()));
    }

}

//
//  next_token() takes a slice and returns the first token in the slice
//  as well as the slice remaining after the token.
//
namespace pdf {

    using std::make_tuple;
    using std::tuple;

    auto operator<<(std::ostream& os, token_type tt) noexcept -> std::ostream& {
        switch (tt) {
            case token_type::nothing: os << "nothing"; break;
            case token_type::bad_token: os << "bad_token"; break;
            case token_type::keyword: os << "keyword"; break;
            case token_type::name: os << "name"; break;
            case token_type::string: os << "string"; break;
            case token_type::hexstring: os << "hexstring"; break;
            case token_type::number: os << "number"; break;
            case token_type::array_begin: os << "array_begin"; break;
            case token_type::array_end: os << "array_end"; break;
            case token_type::dict_begin: os << "dict_begin"; break;
            case token_type::dict_end: os << "dict_end"; break;
            default: os << "???"; break;
        }
        return os;
    }

    auto next_token(slice src) noexcept -> tuple<token, slice> {
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
            case '<': return lbrack(src);
            case '>': return rbrack(src);
            case '[': return single_char(src, token_type::array_begin);
            case ']': return single_char(src, token_type::array_end);
            default: return keyword(src);
        }
    }

}
