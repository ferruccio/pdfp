#include "tools.hpp"

namespace pdf { namespace tools {

    auto operator<<(std::ostream& os, slice s) -> std::ostream& {
        for (char c : s)
            os << c;
        return os;
    }

    auto operator<<(std::ostream& os, variant v) -> std::ostream& {
        switch (v.type()) {
            case variant_type::nothing: os << "*nothing*"; break;
            case variant_type::null: os << "null"; break;
            case variant_type::boolean: os << (v.get_boolean() ? "true" : "false"); break;
            case variant_type::integer: os << v.get_integer(); break;
            case variant_type::real: os << v.get_real(); break;
            case variant_type::string: os << '(' << v.get_string() << ')'; break;
            case variant_type::hexstring: os << '<' << v.get_hexstring() << '>'; break;
            case variant_type::keyword: os << '@' << v.get_keyword(); break;
            case variant_type::name: os << '/' << v.get_name(); break;
            case variant_type::ref: os << v.get_ref().id << ' ' << v.get_ref().gen << " R"; break;
            case variant_type::array: {
                bool space = false;
                os << '[';
                for (auto& a : v.get_array()) {
                    if (space)
                        os << ' ';
                    else
                        space = true;
                    os << a;
                }
                os << ']';
                break;
            }
            case variant_type::dict: {
                bool space = false;
                os << "<<";
                for (auto& kv : v.get_dict()) {
                    if (space)
                        os << ' ';
                    else
                        space = true;
                    os << '/' << kv.first << ' ' << kv.second;
                }
                os << ">>";
                break;
            }
        }
        return os;
    }

    using namespace std;

    auto operator<<(std::ostream& os, variant_proxy vp) -> std::ostream& {
        auto& v = vp.var;
        switch (v.type()) {
            case variant_type::nothing: os << "*nothing*"; break;
            case variant_type::null: os << "null"; break;
            case variant_type::boolean: os << (v.get_boolean() ? "true" : "false"); break;
            case variant_type::integer: os << v.get_integer(); break;
            case variant_type::real: os << v.get_real(); break;
            case variant_type::string: os << '(' << v.get_string() << ')'; break;
            case variant_type::hexstring: os << '<' << v.get_hexstring() << '>'; break;
            case variant_type::keyword: os << vp.atoms.lookup(v.get_keyword()); break;
            case variant_type::name: os << '/' << vp.atoms.lookup(v.get_name()); break;
            case variant_type::ref: os << v.get_ref().id << ' ' << v.get_ref().gen << " R"; break;
            case variant_type::array: {
                bool space = false;
                os << '[';
                for (auto& a : v.get_array()) {
                    if (space)
                        os << ' ';
                    else
                        space = true;
                    os << a(vp.atoms);
                }
                os << ']';
                break;
            }
            case variant_type::dict: {
                bool space = false;
                os << "<<";
                for (auto& kv : v.get_dict()) {
                    if (space)
                        os << ' ';
                    else
                        space = true;
                    os << '/' << vp.atoms.lookup(kv.first) << ' ' << kv.second(vp.atoms);
                }
                os << ">>";
                break;
            }
        }
        return os;
    }
}}