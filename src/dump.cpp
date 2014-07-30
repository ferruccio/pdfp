#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "pdfp.hpp"

auto read_file(const char* filename) -> std::vector<char>
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("failed to open file");
    return std::vector<char>((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
    using namespace std;

    if (argc != 2) {
        cout << "Usage: dump <filename>\n";
        return 0;
    }

    auto pdf = read_file(argv[1]);
    cout << argv[1] << ": " << pdf.size() << " bytes read" << endl;

    auto pp = pdf::make_pdf_parser(&*begin(pdf), &*end(pdf));

    return 0;
}