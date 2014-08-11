#include "parser.hpp"
#include "pdfp.hpp"
#include "tools.hpp"

#include <memory>

namespace pdf {

    using tools::atom_table;
    using tools::slice;

    class PdfParser : public IPdfParser {
    public:
        PdfParser(slice pdf) : pdf(pdf) { init(); }

        ~PdfParser() {}

    private:
        slice pdf;
        atom_table atoms;

        void init() {
            if (!pdf.starts_with("%PDF-1."))
                throw pdf_error("no pdf header");
            slice trailer = pdf.find_last("trailer");
            if (trailer.empty())
                throw pdf_error("no pdf trailer");
            process_trailer(trailer);
        }

        void process_trailer(slice trailer) {
            parser p(trailer, atoms);
            p.expect_keyword(keywords::trailer);
            auto d = p.next_object();
            if (!d.is_dict())
                throw pdf_error("no pdf dictionary");
            p.expect_keyword(keywords::startxref);
            auto v = p.next_object();
            if (!v.is_integer())
                throw pdf_error("startxref not an integer");
            int startxref = v.get_integer();
        }
    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<IPdfParser> {
        return std::make_unique<PdfParser>(slice(begin, end));
    }

}