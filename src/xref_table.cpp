#include <tuple>

#include "xref_table.hpp"
#include "parser.hpp"

namespace pdf {

    void xref_table::get_previous(int offset) {

    }

    void xref_table::get_from(int offset) {
        opt_xref_header header;
        slice input("");
        std::tie(header, input) = get_header(this->input.skip(offset));
    }

    auto xref_table::get_header(slice input) const -> tuple<opt_xref_header, slice> {
        using std::experimental::make_optional;
        using std::make_tuple;

        tools::atom_table tab;
        parser p(input, tab);
        try {
            unsigned first = static_cast<unsigned>(p.expect_integer());
            unsigned count = static_cast<unsigned>(p.expect_integer());
            return make_tuple(make_optional(xref_header(first, count)), p.remainder());
        } catch (format_error&) {
            return make_tuple(opt_xref_header(), p.remainder());
        }
    }

}
