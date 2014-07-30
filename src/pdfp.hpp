#ifndef PDFP_HPP
#define PDFP_HPP

#include <memory>
#include <stdexcept>

namespace pdf {

    class pdf_error : public std::runtime_error {
    public:
        pdf_error(const char* what) : runtime_error(what) {}
    };

    struct IPdfParser {
        virtual ~IPdfParser() {}

    };

    auto make_pdf_parser(const char* begin, const char* end) -> std::unique_ptr<IPdfParser>;

}

#endif