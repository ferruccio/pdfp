#ifndef PDF_ATOMS_HPP
#define PDF_ATOMS_HPP

#include "../tools.hpp"

namespace pdf {

    using tools::atom_type;

    /*
        NB: No atom value should ever be zero.
    */

    enum keywords : atom_type {
        _start_keywords_ = 1000, // not used
        f, _false, n, null, R, trailer, _true, startxref, xref
    };

    enum names : atom_type {
        _start_names_ = 2000, // not used
        ID, Info, Prev, Root, Size
    };

}

#endif
