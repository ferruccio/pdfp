#include "tools/pdf_atoms.hpp"
#include "tools.hpp"

namespace pdf {

    using tools::slice;
    using tools::atom_type;

    /*
        The pdf_atoms table is an immmutable atom_table used to initialize the
        contents of a parser's atom_table with pre-defined PDF symbols.
    */

    const std::unordered_map<slice, atom_type, tools::atom_table::hash> tools::atom_table::pdf_table {

        // keywords
        { "false", keywords::_false },
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
