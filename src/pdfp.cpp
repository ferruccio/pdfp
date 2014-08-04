#include "pdfp.hpp"
#include "tools.hpp"

#include <memory>

namespace pdf {

    using namespace tools;

    class PdfParser : public IPdfParser {
    public:
        PdfParser(slice pdf) : pdf(pdf) { init(); }

        ~PdfParser() {}

    private:
        slice pdf;

        void init() {
            if (!pdf.starts_with("%PDF-1."))
                throw pdf_error("no pdf header");
            slice trailer = pdf.find_last("trailer");
            if (trailer.empty())
                throw pdf_error("no pdf trailer");
        }
    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<IPdfParser> {
        return std::make_unique<PdfParser>(slice(begin, end));
    }

}