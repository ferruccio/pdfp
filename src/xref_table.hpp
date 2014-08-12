#ifndef XREF_TABLE_HPP
#define XREF_TABLE_HPP

#include <vector>

#include "tools.hpp"

namespace pdf {

    using std::vector;
    using tools::slice;
    using tools::variant;

    class xref_table {
    public:
        xref_table(slice source, int size) : source(source) {
            if (size <= 0)
                throw pdf_error("xref_table: invalid table size");
            objects.resize(size + 1);
        }

        void get_from(int offset) {

        }

    private:
        const slice source; // entire pdf file
        vector<variant> objects;
    };

}

#endif
