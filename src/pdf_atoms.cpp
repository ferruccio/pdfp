#include "pdf_atoms.hpp"
#include "tools.hpp"

namespace pdf {

    using tools::atom_table;

    /*
        The pdf_atoms table is an immmutable atom_table used to initialize the
        contents of a parser's atom_table with pre-defined PDF keywords and names.
    */

    const atom_table pdf_atoms {

        // keywords
        { "R", keywords::R },
        { "trailer", keywords::trailer },
        { "startxref", keywords::startxref },
        { "xref", keywords::xref },

        // names
        { "ID", names::ID },
        { "Info", names::Info },
        { "Root", names::Root },
        { "Size", names::Size },

    };

}
