#ifndef PARSER_HPP
#define PARSER_HPP

#include <ostream>
#include <tuple>

#include "tools.hpp"

namespace pdf {

    using tools::slice;
    using tools::variant;
    using tools::atom_table;
    using tools::atom_type;

    auto get_pdf_atoms() noexcept -> const atom_table&;

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

    // Returns the first token in a slice and the remainder of the slice.
    auto next_token(slice src) noexcept -> std::tuple<token, slice>;

    class parser {
    public:
        parser(slice src, atom_table& atoms) : src(src), atoms(atoms) {}

        auto next_object() -> variant;
        void expect_keyword(atom_type keyword);

    private:
        slice src;
        atom_table& atoms;
    };

}

#endif
