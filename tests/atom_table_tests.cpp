#include "catch.hpp"

#include "parser.hpp"
#include "pdf_atoms.hpp"
#include "tools.hpp"

using pdf::tools::slice;
using pdf::tools::atom_type;
using pdf::tools::atom_table;

TEST_CASE("atom_table: add", "[atom_table]") {
    atom_table t;

    CHECK(t.add("xyzzy") >= 0);
    CHECK(t.add("plugh") >= 0);
    CHECK(t.add("plover") >= 0);
    CHECK_THROWS(t.add("plugh", 0));

    CHECK(t["xyzzy"] == t["xyzzy"]);
    CHECK(t["plugh"] == t["plugh"]);
    CHECK(t["plover"] == t["plover"]);

    CHECK(t["xyzzy"] != t["plugh"]);
    CHECK(t["xyzzy"] != t["plover"]);

    CHECK(t["plugh"] != t["xyzzy"]);
    CHECK(t["plugh"] != t["plover"]);

    CHECK(t["plover"] != t["xyzzy"]);
    CHECK(t["plover"] != t["plugh"]);
}

TEST_CASE("atom_table: pre-defined", "[atom_table]") {
    enum atom : atom_type { xyzzy, plugh, plover };

    atom_table t;
    t.add("xyzzy", atom::xyzzy);
    t.add("plugh", atom::plugh);
    t.add("plover", atom::plover);

    CHECK(t["xyzzy"] == atom::xyzzy);
    CHECK(t["plugh"] == atom::plugh);
    CHECK(t["plover"] == atom::plover);
}

TEST_CASE("atom_table: initialized", "[atom_table]") {
    enum atom : atom_type { xyzzy, plugh, plover };

    atom_table t{
        { "xyzzy", atom::xyzzy },
        { "plugh", atom::plugh },
        { "plover", atom::plover }
    };

    CHECK(t["xyzzy"] == atom::xyzzy);
    CHECK(t["plugh"] == atom::plugh);
    CHECK(t["plover"] == atom::plover);
}

TEST_CASE("atom_table: pdf_atoms", "[atom_table]") {
    using namespace pdf;

    const auto& pdf_atoms = get_pdf_atoms();

    CHECK(pdf_atoms.find("trailer") == keywords::trailer);
    CHECK(pdf_atoms.find("Root") == names::Root);

    atom_table t { pdf_atoms };

    CHECK(t["trailer"] == keywords::trailer);
    CHECK(t["Root"] == names::Root);
}