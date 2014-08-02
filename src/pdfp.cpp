#include "pdfp.hpp"
#include "tools.hpp"

#include <memory>

namespace pdf {

    using namespace tools;

    class PdfParser : public IPdfParser {
    public:
        PdfParser(slice pdf) : pdf(pdf) {}
        ~PdfParser() {}

    private:
        slice pdf;
    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<IPdfParser> {
        return std::make_unique<PdfParser>(slice(begin, end));
    }

}