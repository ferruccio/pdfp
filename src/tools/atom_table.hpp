#ifndef TOOLS_ATOM_TABLE_HPP
#define TOOLS_ATOM_TABLE_HPP

#include <unordered_map>

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

        atom_table(std::initializer_list<std::pair<slice, atom_type>> init) {
            for (auto kv : init)
                add(kv.first, kv.second);
        }

        atom_table(const atom_table& at) {
            table.insert(at.table.begin(), at.table.end());
        }

        auto add(slice key) noexcept -> atom_type {
            auto value = table.find(key);
            return value != table.end() ? value->second : table[key] = next++;
        }

        void add(slice key, atom_type value) {
            if (haskey(key)) throw std::runtime_error("pdf::tools::atom_table::add() - duplicate key");
            table[key] = value;
        }

        auto operator[](slice key) noexcept -> atom_type {
            return add(key);
        }

        auto find(slice key) const noexcept -> atom_type {
            auto value = table.find(key);
            return value != table.end() ? value->second : 0; // assume no atom == 0
        }

        // brute force reverse lookup: for debugging purposes only
        auto lookup(atom_type value) const noexcept -> slice {
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

        std::unordered_map<slice, atom_type, hash> table;
        atom_type next = 0x10000;

        auto haskey(slice key) const noexcept -> bool {
            return table.find(key) != table.end();
        }
    };

}}

#endif
