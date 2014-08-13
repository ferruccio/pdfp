#ifndef PDF_DICTIONARIES_HPP
#define PDF_DICTIONARIES_HPP

#include "tools.hpp"

namespace pdf {

    using tools::variant;
    using tools::atom_type;

    class pdf_dict {
    public:
        pdf_dict(const variant& v) : dict(v.get_dict()) {}

        auto get_integer(atom_type name, int value = 0) const -> int {
            auto val = dict.find(name);
            return val == dict.end() ? value : val->second.get_integer();
        }

    private:
        const variant::dict_type& dict;
    };

    class trailer_dict : public pdf_dict {
    public:
        trailer_dict(const variant& v) : pdf_dict(v) {}

        auto Size() const -> int { return get_integer(names::Size); }
        auto Prev() const -> int { return get_integer(names::Prev); }
    };
}

#endif
