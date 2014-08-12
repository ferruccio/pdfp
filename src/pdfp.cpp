#include <memory>

#include "pdfp.hpp"

#include "parser.hpp"
#include "pdf_dictionaries.hpp"
#include "tools.hpp"
#include "xref_table.hpp"

namespace pdf {

    using std::make_unique;
    using std::unique_ptr;
    using tools::atom_table;
    using tools::slice;

    class pdf_parser : public PdfParser {
    public:
        pdf_parser(slice pdf) : pdf(pdf) { init(); }
        ~pdf_parser() {}

    private:
        slice pdf;
        atom_table atoms;
        unique_ptr<xref_table> xref;

        void init() {
            if (!pdf.starts_with("%PDF-1."))
                throw pdf_error("no pdf header");
            slice trailer = pdf.find_last("trailer");
            if (trailer.empty())
                throw pdf_error("no pdf trailer");
            process_trailer(trailer);
        }

        void process_trailer(slice input) {
            parser p(input, atoms);

            p.expect_keyword(keywords::trailer);
            auto d = p.next_object();
            if (!d.is_dict())
                throw format_error("pdf_parser::process_trailer: no pdf dictionary");
            trailer_dict trailer(d);

            xref = make_unique<xref_table>(pdf, trailer.Size());

            p.expect_keyword(keywords::startxref);
            xref->get_from(p.expect_integer());
        }
    };

    auto make_pdf_parser(const char* begin, const char* end) -> unique_ptr<PdfParser> {
        return make_unique<pdf_parser>(slice(begin, end));
    }

}