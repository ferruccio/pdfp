#ifndef PARSER_HPP
#define PARSER_HPP

#include <experimental/optional>
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
        bad_token,
        keyword, name, string, hexstring, number,
        array_begin, array_end, dict_begin, dict_end
    };

    auto operator<<(std::ostream& os, token_type tt) noexcept -> std::ostream&;

    class token {
    public:
        token(token_type type, slice value) : _type(type), _value(value) {}

        auto type() const noexcept -> token_type { return _type; }
        auto value() const noexcept -> slice { return _value; }

    private:
        token_type _type;
        slice _value;
    };

    using opt_token = std::experimental::optional<token>;

    auto peek_token(slice input) noexcept -> opt_token;
    auto next_token(slice input) noexcept -> std::tuple<opt_token, slice>;

    using opt_variant = std::experimental::optional<variant>;

    class parser {
    public:
        parser(slice input, const atom_table& atoms) : input(input), atoms(atoms) {}

        auto next_object() -> opt_variant;
        void expect_keyword(atom_type keyword);
        auto expect_integer() -> int;
        auto expect_dict() -> variant;
        auto remainder() const noexcept -> slice { return input; }

    private:
        slice input;
        atom_table atoms; // should be a const& but clang++ crashes

        void skip_token(token tok) noexcept {
            this->input = this->input.skip(tok.value());
        }

        void parse_until(token_type type, std::vector<variant>& result);
        auto parse_array() -> variant;
        auto parse_dict() -> variant;
    };

}

#endif
