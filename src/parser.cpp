#include "parser.hpp"
#include "pdf_atoms.hpp"

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

    auto skipws(slice input) noexcept -> slice {
        while (!input.empty() && iswhitespace(*input))
            input = input.rest();
        return input;
    }

    auto name(slice input) noexcept -> token {
        bool first = true;
        return token(token_type::name, input.take_until([&](char ch)->bool {
            return first ? first = false : isbreak(ch);
        }));
    }

    auto number(slice input) noexcept -> token {
        return token(token_type::number, input.take_while(isnumeric));
    }

    auto string(slice input) noexcept -> token {
        unsigned int nesting = 0;
        bool quote = false;
        bool done = false;
        auto tok = input.take_until([&](char ch)->bool {
            if (done)
                return true;
            if (quote)
                return quote = false; // assignment is intentional
            switch (ch) {
                case '(': ++nesting; // fall through
                default: return false;
                case ')': done = --nesting == 0; return false;
                case '\\': return !(quote = true); // assignment is intentional
            }
        });
        return token(token_type::string, tok);
    }

    auto lbrack(slice input) noexcept -> token {
        if (input.length() < 2)
            return token(token_type::bad_token, input);
        if (input[1] == '<')
            return token(token_type::dict_begin, input.left(2));
        bool done = false;
        auto tok = input.take_until([&](char ch)->bool {
            if (done)
                return true;
            done = ch == '>';
            return false;
        });
        return token(token_type::hexstring, tok);
    }

    auto rbrack(slice input) noexcept -> token {
        return input.length() < 2 || input[1] != '>'
            ? token(token_type::bad_token, input)
            : token(token_type::dict_end, input.left(2));
    }

    auto keyword(slice input) noexcept -> token {
        auto tok = input.take_until(isbreak);
        return tok.length() == 0
            ? token(token_type::bad_token, input)
            : token(token_type::keyword, tok);
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
        Returns the first token in input.
    */
    auto peek_token(slice input) noexcept -> token {
        // skip over any whitespace and comments
        for (;;) {
            if ((input = skipws(input)).empty())
                return token(token_type::nothing, input);
            if (*input != '%')
                break;
            input = input.skip_until(iseol);
        }

        switch (*input) {
            case '/': return name(input);
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '+': case '-': case '.': return number(input);
            case '(': return string(input);
            case '<': return lbrack(input);
            case '>': return rbrack(input);
            case '[': return token(token_type::array_begin, input.left(1));
            case ']': return token(token_type::array_end, input.left(1));
            default: return keyword(input);
        }
    }

    /*
        Returns a tuple consisting of the first token in the slice and
        another slice which consists of the original slice minus the initial token.
        By repeatedly calling next_token() and feeding the remainder slice back
        into it, we can extract all tokens in a slice.
    */
    auto next_token(slice input) noexcept -> tuple<token, slice> {
        auto tok = peek_token(input);
        return make_tuple(tok, input.skip(tok.value()));
    }

}

namespace {

    using pdf::variant;

    /*
        Parser support functions.
    */
    auto parse_number(slice input) -> variant {
        long isign = 1;
        double rsign = 1.0;
        switch (input.first()) {
            case '-': isign = -1; rsign = -1.0; // fall through
            case '+': input = input.rest(); break;
        }
        bool decimal = false;
        double divisor = 1.0;
        long value = 0;
        for (char c : input)
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

    /*
        Replace the id and gen objects at the end of objects vector with a reference object.
    */
    void generate_reference(std::vector<variant>& objects) {
        if (objects.size() < 2)
            throw std::runtime_error("generate_reference: not enough objects");
        auto gen = objects.back();
        if (!gen.is_integer())
            throw std::runtime_error("generate_reference: gen is not an integer");
        objects.pop_back();
        auto id = objects.back();
        if (!id.is_integer())
            throw std::runtime_error("generate_reference: id is not an integer");
        objects.pop_back();
        objects.push_back(variant::make_ref(id.get_integer(), gen.get_integer()));
    }

}

namespace pdf {

    auto parser::next_object() -> variant {
        token tok;
        tie(tok, this->input) = next_token(this->input);
        switch (tok.type()) {
            case token_type::nothing: return variant();
            case token_type::bad_token: throw std::runtime_error("next_object: invalid token");
            case token_type::keyword:
                switch (atoms[tok.value()]) {
                    case keywords::null: return variant::make_null();
                    case keywords::_true: return variant::make_boolean(true);
                    case keywords::_false: return variant::make_boolean(false);
                    default: return variant::make_keyword(atoms[tok.value()]);
                }
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

    void parser::expect_keyword(atom_type keyword) {
        variant kw = next_object();
        if (!kw.is_keyword())
            throw std::runtime_error("expect_keyword: not a keyword");
        if (kw.get_keyword() != keyword)
            throw std::runtime_error("expect_keyword: unexpected keyword");
    }

    void parser::parse_until(token_type type, std::vector<variant>& result) {
        for (;;) {
            auto tok = peek_token(this->input);
            if (tok.type() == type) {
                skip_token(tok);
                return;
            }
            if (tok.type() == token_type::nothing)
                throw std::runtime_error("parse_until: unexpected end");
            if (tok.type() == token_type::keyword && atoms[tok.value()] == keywords::R) {
                generate_reference(result);
                skip_token(tok);
            } else {
                result.push_back(next_object());
            }
        }
    }

    auto parser::parse_array() -> variant {
        auto array = variant::make_array();
        parse_until(token_type::array_end, array.get_array());
        return array;
    }

    auto parser::parse_dict() -> variant {
        std::vector<variant> source;
        parse_until(token_type::dict_end, source);
        auto dict = variant::make_dict();
        auto& d = dict.get_dict();
        const variant* name = nullptr;
        for (const auto& v : source)
            if (name == nullptr) {
                name = &v;
                if (!name->is_name())
                    throw std::runtime_error("parse_dict: not a name");
            } else {
                d[name->get_name()] = v;
                name = nullptr;
            }
        return dict;
    }

}
