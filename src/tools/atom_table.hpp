#ifndef TOOLS_ATOM_TABLE_HPP
#define TOOLS_ATOM_TABLE_HPP

#include <unordered_map>

#include "../pdfp.hpp"

namespace pdf { namespace tools {

    /*
        An atom table provides a unique mapping from slices to simple tokens (atom_type).
        It also provides support for slices with predefined values for tokens.
        Assumption: all slices stored in a atom table have longer lifetimes than
        the atom table itself, otherwise bad things will happen...
    */
    using atom_type = unsigned int;

    class atom_table {
    public:
        atom_table() {}

        const atom_type nothing = 0; // assume no atom == 0

        auto add(slice key) noexcept -> atom_type {
            auto value = pdf_table.find(key);
            if (value != pdf_table.end())
                return value->second;
            value = table.find(key);
            return value != table.end() ? value->second : table[key] = next++;
        }

        void add(slice key, atom_type value) {
            if (haskey(key)) throw format_error("atom_table::add: duplicate key");
            table[key] = value;
        }

        auto operator[](slice key) noexcept -> atom_type {
            return add(key);
        }

        auto find(slice key) const noexcept -> atom_type {
            auto value = pdf_table.find(key);
            if (value != pdf_table.end())
                return value->second;
            value = table.find(key);
            return value != table.end() ? value->second : nothing;
        }

        // brute force reverse lookup: for debugging purposes only
        auto lookup(atom_type value) const noexcept -> slice {
            for (const auto& kv : pdf_table)
                if (kv.second == value)
                    return kv.first;
            for (const auto& kv : table)
                if (kv.second == value)
                    return kv.first;
            return "???";
        }

    private:
        struct hash {
            auto operator()(slice s) const noexcept -> std::size_t {
                std::size_t hash = 0;
                for (char c : s)
                    hash = hash * 101 + c;
                return hash;
            }
        };

        // PDF symbols
        static const std::unordered_map<slice, atom_type, hash> pdf_table;

        // other symbols
        std::unordered_map<slice, atom_type, hash> table;
        atom_type next = 0x10000;

        auto haskey(slice key) const noexcept -> bool {
            return pdf_table.find(key) != pdf_table.end() || table.find(key) != table.end();
        }
    };

}}

#endif
