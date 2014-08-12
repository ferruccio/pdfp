#ifndef PDFP_HPP
#define PDFP_HPP

#include <memory>
#include <stdexcept>

namespace pdf {

    class pdf_error : public std::runtime_error {
    public:
        pdf_error(const char* what) : std::runtime_error(what) {}
    };

    class format_error : public std::runtime_error {
    public:
        format_error(const char* what) : std::runtime_error(what) {}
    };

    struct PdfParser {
        virtual ~PdfParser() {}
    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<PdfParser>;

}

#endif