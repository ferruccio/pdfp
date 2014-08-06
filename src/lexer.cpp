#include "lexer.hpp"

namespace {

    using pdf::token;
    using pdf::token_type;
    using pdf::tools::slice;
    using std::make_tuple;
    using std::tuple;
    using std::tie;

    /*
        Lexer support functions
    */
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
                case '\\': return !(quote = true);
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

namespace pdf {

    using std::make_tuple;
    using std::tuple;

    /*
        Write a token's type as a string to an ostream.
        For unit testing and debugging purposes.
    */
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

    /*
        Returns a tuple consisting of the first token in the slice and
        another slice which consists of the original slice minus the initial token.
        By repeatedly calling next_token() and feeding the remainder slice back
        into it, we can extract all tokens in a slice.
    */
    auto next_token(slice src) noexcept -> tuple<token, slice> {
        // skip over any whitespace and comments
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

namespace {

    using pdf::variant;

    /*
        Parser support functions.
    */
    auto parse_number(slice src) -> variant {
        long isign = 1;
        double rsign = 1.0;
        switch (src.first()) {
            case '-': isign = -1; rsign = -1.0; // fall through
            case '+': src = src.rest(); break;
        }
        bool decimal = false;
        double divisor = 1.0;
        long value = 0;
        for (char c : src)
            switch (c) {
                case '.':
                    if (!decimal) {
                        decimal = true;
                        break;
                    } // fall through
                case '+':
                case '-': throw std::runtime_error("parse_number: invalid number");
                default:
                    value = value * 10 + (c - '0');
                    if (decimal) divisor *= 10.0;
            }
        if (decimal)
            return variant::make_real(rsign * static_cast<double>(value) / divisor);
        else
            return variant::make_integer(isign * value);

    }

    auto parse_array() -> variant {
        return variant();
    }

    auto parse_dict() -> variant {
        return variant();
    }
}

namespace pdf {

    auto parser::next_object() -> variant {
        token tok;
        tie(tok, this->src) = next_token(this->src);
        switch (tok.type()) {
            case token_type::nothing: return variant();
            case token_type::bad_token: throw std::runtime_error("next_object: invalid token");
            case token_type::keyword: return variant::make_keyword(atoms[tok.value()]);
            case token_type::name: return variant::make_name(atoms[tok.value()]);
            case token_type::string: return variant::make_string(tok.value());
            case token_type::hexstring: return variant::make_hexstring(tok.value());
            case token_type::number: return parse_number(tok.value());
            case token_type::array_begin: return parse_array();
            case token_type::array_end: throw std::runtime_error("next_object: unexpected array end");
            case token_type::dict_begin: return parse_dict();
            case token_type::dict_end: throw std::runtime_error("next_object: unexpected dict end");
            default: throw std::runtime_error("next_object: unexpected error");
        }
    }

}
