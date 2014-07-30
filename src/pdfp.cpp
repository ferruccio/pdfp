#include "pdfp.hpp"
#include "tools.hpp"

#include <memory>

namespace pdf {

    using namespace tools;

    class PdfParser : public IPdfParser {
    public:
        PdfParser(slice<char> pdf) : pdf(pdf) {}
        ~PdfParser() {}

    private:
        slice<char> pdf;
    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<IPdfParser> {
        return std::make_unique<PdfParser>(slice<char>(begin, end));
    }

}