#include "tools/pdf_atoms.hpp"
#include "tools.hpp"

namespace pdf {

    using tools::slice;
    using tools::atom_type;

    /*
        The pdf_table contains pre-defined PDF symbols.
    */

    const std::unordered_map<slice, atom_type, tools::atom_table::hash> tools::atom_table::pdf_table {

        // keywords
        { "f", keywords::f },
        { "false", keywords::_false },
        { "n", keywords::n },
        { "null", keywords::null },
        { "R", keywords::R },
        { "trailer", keywords::trailer },
        { "true", keywords::_true },
        { "startxref", keywords::startxref },
        { "xref", keywords::xref },

        // names
        { "/ID", names::ID },
        { "/Info", names::Info },
        { "/Prev", names::Prev },
        { "/Root", names::Root },
        { "/Size", names::Size },

    };

}
