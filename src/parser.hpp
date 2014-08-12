#ifndef PARSER_HPP
#define PARSER_HPP

#include <ostream>
#include <tuple>
#include <vector>

#include "tools.hpp"

namespace pdf {

    using tools::slice;
    using tools::variant;
    using tools::atom_table;
    using tools::atom_type;

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

    auto peek_token(slice input) noexcept -> token;
    auto next_token(slice input) noexcept -> std::tuple<token, slice>;

    class parser {
    public:
        parser(slice input, const atom_table& atoms) : input(input), atoms(atoms) {}

        auto next_object() -> variant;
        void expect_keyword(atom_type keyword);
        auto expect_integer() -> int;

    private:
        slice input;
        atom_table atoms;

        void skip_token(token tok) noexcept {
            this->input = this->input.skip(tok.value());
        }

        void parse_until(token_type type, std::vector<variant>& result);
        auto parse_array() -> variant;
        auto parse_dict() -> variant;
    };

}

#endif
