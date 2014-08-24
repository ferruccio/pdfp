#ifndef XREF_TABLE_HPP
#define XREF_TABLE_HPP

#include <experimental/optional>
#include <tuple>
#include <vector>

#include "tools.hpp"

namespace pdf {

    using std::tuple;
    using std::vector;
    using tools::slice;
    using tools::variant;

    struct xref_header {
        xref_header(unsigned first, unsigned count) : first(first), count(count) {}
        unsigned first;
        unsigned count;
    };

    using opt_xref_header = std::experimental::optional<xref_header>;

    class xref_entry {
    public:
        xref_entry(unsigned offset = 0, unsigned short gen = 0xffff) : _offset(offset), _gen(gen) {}

        auto offset() const noexcept -> unsigned { return _offset; }
        auto gen() const noexcept -> unsigned short { return _gen; }

    private:
        unsigned _offset;
        unsigned short _gen;
    };

    class xref_table {
    public:
        xref_table(slice input, int size) : input(input) {
            if (size <= 0)
                throw pdf_error("xref_table: invalid table size");
            objects.resize(size + 1);
        }

        void get_previous(int offset);
        void get_from(int offset);

    private:
        const slice input; // entire pdf file
        vector<xref_entry> objects;

        auto get_header(slice input) const -> tuple<opt_xref_header, slice>;
    };

}

#endif
