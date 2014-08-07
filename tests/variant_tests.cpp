#include "catch.hpp"
#include "tools.hpp"

using pdf::tools::slice;
using pdf::tools::atom_type;
using pdf::tools::atom_table;
using pdf::tools::variant;

#include <sstream>
#include <string>

using std::stringstream;
using std::string;

TEST_CASE("variant: simple", "[variant]") {
    auto v = variant::make_null();
    CHECK(v.is_null());

    v = variant::make_boolean(true);
    CHECK(v.is_boolean());
    CHECK(v.get_boolean() == true);

    v = variant::make_integer(12345);
    CHECK(v.is_integer());
    CHECK(v.is_numeric());
    CHECK(v.get_integer() == 12345);

    v = variant::make_real(3.1415926);
    CHECK(v.is_real());
    CHECK(v.is_numeric());
    CHECK(v.get_real() == 3.1415926);

    v = variant::make_ref(10, 1);
    CHECK(v.is_ref());
    CHECK(v.get_ref().id == 10);
    CHECK(v.get_ref().gen == 1);
}

TEST_CASE("variant: strings", "[variant]") {
    auto v = variant::make_string("xyzzy");
    CHECK(v.is_string());
    CHECK(v.is_text());
    CHECK(v.get_string() == "xyzzy");

    v = variant::make_hexstring("deadbeef");
    CHECK(v.is_hexstring());
    CHECK(v.is_text());
    CHECK(v.get_hexstring() == "deadbeef");
}

TEST_CASE("variant: arrays", "[variant]") {
    auto v = variant::make_array();
    CHECK(v.is_array());
    CHECK(v.size() == 0);

    auto& a = v.get_array();
    a.push_back(variant::make_null());
    a.push_back(variant::make_boolean(false));
    a.push_back(variant::make_integer(3));
    a.push_back(variant::make_real(2.5));
    a.push_back(variant::make_string("xyzzy"));
    a.push_back(variant::make_hexstring("deadbeef"));
    a.push_back(variant::make_array());
    a.push_back(variant::make_dict());
    CHECK(v.size() == 8);

    CHECK(v[0].is_null());
    CHECK(v[1].is_boolean());
    CHECK(v[1].get_boolean() == false);
    CHECK(v[2].is_integer());
    CHECK(v[2].get_integer() == 3);
    CHECK(v[3].is_real());
    CHECK(v[3].get_real() == 2.5);
    CHECK(v[4].is_string());
    CHECK(v[4].get_string() == "xyzzy");
    CHECK(v[5].is_hexstring());
    CHECK(v[5].get_hexstring() == "deadbeef");
    CHECK(v[6].is_array());
    CHECK(v[6].get_array().size() == 0);
    CHECK(v[7].is_dict());
    CHECK(v[7].get_dict().size() == 0);
}

TEST_CASE("variant: dicts", "[variant]") {
    enum test : atom_type { null, boolean, integer, real, string, hexstring, array, dict };
    auto v = variant::make_dict();
    CHECK(v.is_dict());
    CHECK(v.size() == 0);

    auto& d = v.get_dict();
    d[test::null] = variant::make_null();
    d[test::boolean] = variant::make_boolean(true);
    d[test::integer] = variant::make_integer(123);
    d[test::real] = variant::make_real(3.0);
    d[test::string] = variant::make_string("plover");
    d[test::hexstring] = variant::make_hexstring("beefc0c0");
    d[test::array] = variant::make_array();
    d[test::dict] = variant::make_dict();
    CHECK(d.size() == 8);

    CHECK(v[test::null].is_null());
    CHECK(v[test::boolean].is_boolean());
    CHECK(v[test::boolean].get_boolean() == true);
    CHECK(v[test::integer].is_integer());
    CHECK(v[test::integer].get_integer() == 123);
    CHECK(v[test::real].is_real());
    CHECK(v[test::real].get_real() == 3.0);
    CHECK(v[test::string].is_string());
    CHECK(v[test::string].get_string() == "plover");
    CHECK(v[test::hexstring].is_hexstring());
    CHECK(v[test::hexstring].get_hexstring() == "beefc0c0");
    CHECK(v[test::array].is_array());
    CHECK(v[test::array].get_array().size() == 0);
    CHECK(v[test::dict].is_dict());
    CHECK(v[test::dict].get_dict().size() == 0);

    CHECK(v.haskey(test::boolean));
    CHECK(v.haskey(test::dict));
    CHECK(!v.haskey(static_cast<atom_type>(100)));
}

TEST_CASE("variant: << simple", "[variant]") {
    stringstream ss;
    ss << variant::make_null();
    CHECK(ss.str() == "null");

    ss.str("");
    ss << variant::make_boolean(true);
    CHECK(ss.str() == "true");

    ss.str("");
    ss << variant::make_boolean(false);
    CHECK(ss.str() == "false");

    ss.str("");
    ss << variant::make_integer(12345);
    CHECK(ss.str() == "12345");

    ss.str("");
    ss << variant::make_real(2.5);
    CHECK(ss.str() == "2.5");

    ss.str("");
    ss << variant::make_string("xyzzy");
    CHECK(ss.str() == "(xyzzy)");

    ss.str("");
    ss << variant::make_hexstring("deadbeef");
    CHECK(ss.str() == "<deadbeef>");

    ss.str("");
    ss << variant::make_keyword(123);
    CHECK(ss.str() == "@123");

    ss.str("");
    ss << variant::make_name(123);
    CHECK(ss.str() == "/123");
}

TEST_CASE("variant: << array/dict", "[variant]") {
    auto v = variant::make_array();
    auto& a = v.get_array();
    a.push_back(variant::make_null());
    a.push_back(variant::make_keyword(10));
    a.push_back(variant::make_name(20));
    a.push_back(variant::make_boolean(false));
    a.push_back(variant::make_integer(3));
    a.push_back(variant::make_real(2.5));
    a.push_back(variant::make_string("xyzzy"));
    a.push_back(variant::make_hexstring("deadbeef"));

    auto v2 = variant::make_array();
    auto& a2 = v2.get_array();
    a2.push_back(variant::make_string("xyzzy"));
    a2.push_back(variant::make_integer(32));
    a2.push_back(variant::make_boolean(false));
    a.push_back(v2);

    auto v3 = variant::make_dict();
    auto& d3 = v3.get_dict();
    d3[10] = variant::make_string("plover");
    d3[11] = variant::make_real(3.5);
    d3[13] = variant::make_boolean(true);
    a.push_back(v3);

    stringstream ss;
    ss << v;
    CHECK(ss.str() == "[null @10 /20 false 3 2.5 (xyzzy) <deadbeef> [(xyzzy) 32 false] <</10 (plover) /11 3.5 /13 true>>]");
}

TEST_CASE("variant: << dict + atoms", "[variant]") {
    atom_table t;
    t.add("keyword");
    t.add("Name");
    t.add("Type");
    t.add("Value");

    auto v = variant::make_dict();
    auto& d = v.get_dict();
    d[t["Name"]] = variant::make_string("Charlie Brown");
    d[t["Type"]] = variant::make_keyword(t["keyword"]);
    d[t["Value"]] = variant::make_ref(100, 0);

    stringstream ss;
    ss << v(t);
    CHECK(ss.str() == "<</Name (Charlie Brown) /Type keyword /Value 100 0 R>>");
}