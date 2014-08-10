#include "catch.hpp"
#include "tools.hpp"
#include "parser.hpp"
#include "pdf_atoms.hpp"

#include <tuple>

using pdf::tools::slice;
using pdf::token_type;
using pdf::token;
using pdf::next_token;
using pdf::parser;
using pdf::keywords;

using std::tie;

TEST_CASE("next_token: simple", "[lexer]") {
    slice s("keyword /name (string) <deadbeef> 1 1.0 +1 -1.0 [ ] << >>");
    token t;

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::string);
    CHECK(t.value() == "(string)");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::hexstring);
    CHECK(t.value() == "<deadbeef>");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "1");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "1.0");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "+1");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "-1.0");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::array_begin);
    CHECK(t.value() == "[");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::array_end);
    CHECK(t.value() == "]");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::dict_begin);
    CHECK(t.value() == "<<");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::dict_end);
    CHECK(t.value() == ">>");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::nothing);
}

TEST_CASE("next_token: minimal spacing", "[lexer]") {
    slice s("keyword/name(string)<deadbeef>1 1.0 +1 -1.0[]<<>>");
    token t;

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::string);
    CHECK(t.value() == "(string)");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::hexstring);
    CHECK(t.value() == "<deadbeef>");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "1");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "1.0");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "+1");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::number);
    CHECK(t.value() == "-1.0");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::array_begin);
    CHECK(t.value() == "[");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::array_end);
    CHECK(t.value() == "]");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::dict_begin);
    CHECK(t.value() == "<<");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::dict_end);
    CHECK(t.value() == ">>");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::nothing);
}

TEST_CASE("next_token: newlines", "[lexer]") {
    slice s("keyword \n /name");
    token t;

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");

    s = "keyword \r /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");

    s = "keyword \r\n /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");

    s = "keyword %comment\n /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "/name");
}

TEST_CASE("next_token: strings", "[lexer]") {
    auto tok = next_token(" (simple (nested) string) ");
    CHECK(std::get<0>(tok).value() == "(simple (nested) string)");

    tok = next_token("\n(quoted \\( lparen))");
    CHECK(std::get<0>(tok).value() == "(quoted \\( lparen)");

    tok = next_token("\t(quoted \\) rparen)");
    CHECK(std::get<0>(tok).value() == "(quoted \\) rparen)");

    tok = next_token("(quoted \\\\ backslash)");
    CHECK(std::get<0>(tok).value() == "(quoted \\\\ backslash)");

    tok = next_token("(ignored \\backslash)");
    CHECK(std::get<0>(tok).value() == "(ignored \\backslash)");

    tok = next_token("(\\n \\r \\t \\b \\f \\( \\) \\\\ \\000 \\020 \\200 \\377)");
    CHECK(std::get<0>(tok).value() == "(\\n \\r \\t \\b \\f \\( \\) \\\\ \\000 \\020 \\200 \\377)");
}

TEST_CASE("next_token: pdf keywords", "[lexer]") {
    using namespace pdf;

    const atom_table& t = get_pdf_atoms();

    auto tok = std::get<0>(next_token("null"));
    CHECK(tok.type() == token_type::keyword);
    CHECK(t.find(tok.value()) == keywords::null);
    tok = std::get<0>(next_token("  true  "));
    CHECK(tok.type() == token_type::keyword);
    CHECK(t.find(tok.value()) == keywords::_true);
    tok = std::get<0>(next_token("\nfalse\t"));
    CHECK(tok.type() == token_type::keyword);
    CHECK(t.find(tok.value()) == keywords::_false);
}

TEST_CASE("next_object: simple", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p(" null true false 1 2.5 (string) <deadbeef>", t);
    CHECK(p.next_object().is_null());
    CHECK(p.next_object().is_boolean(true));
    CHECK(p.next_object().is_boolean(false));
    CHECK(p.next_object().is_integer(1));
    CHECK(p.next_object().is_real(2.5));
    CHECK(p.next_object().is_string("(string)"));
    CHECK(p.next_object().is_hexstring("<deadbeef>"));
    CHECK(p.next_object().is_nothing());
}

TEST_CASE("next_object: array", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p(" [null true false 1 2.5 (string) <deadbeef>]", t);
    auto o = p.next_object();
    CHECK(o.is_array());
    CHECK(p.next_object().is_nothing());
    auto& a = o.get_array();
    CHECK(a.size() == 7);
    CHECK(a[0].is_null());
    CHECK(a[1].is_boolean(true));
    CHECK(a[2].is_boolean(false));
    CHECK(a[3].is_integer(1));
    CHECK(a[4].is_real(2.5));
    CHECK(a[5].is_string("(string)"));
    CHECK(a[6].is_hexstring("<deadbeef>"));
}

TEST_CASE("next_object: nested array", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p(" [1 2 [3 4 5] 6 7]", t);
    auto o = p.next_object();
    CHECK(o.is_array());
    CHECK(p.next_object().is_nothing());
    auto& a = o.get_array();
    CHECK(a.size() == 5);
    CHECK(a[0].is_integer(1));
    CHECK(a[1].is_integer(2));
    CHECK(a[2].is_array());
    CHECK(a[3].is_integer(6));
    CHECK(a[4].is_integer(7));
    auto& a2 = a[2].get_array();
    CHECK(a2.size() == 3);
    CHECK(a2[0].is_integer(3));
    CHECK(a2[1].is_integer(4));
    CHECK(a2[2].is_integer(5));
}

TEST_CASE("next_object: dict", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p("<</Name (Fred) /Age 35 /Obj 10 20 R>>", t);
    auto o = p.next_object();
    CHECK(o.is_dict());
    CHECK(o.size() == 3);
    CHECK(o[t["/Name"]].is_string("(Fred)"));
    CHECK(o[t["/Age"]].is_integer(35));
    CHECK(o[t["/Obj"]].is_ref(10, 20));
}

TEST_CASE("next_object: nested dict", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p("<</Name (Fred) /Vec [1 2 3] /Stuff <</Start 10 0 R /End 11 0 R>>>>", t);
    auto o = p.next_object();
    CHECK(o.is_dict());
    CHECK(o[t["/Name"]].is_string("(Fred)"));
    CHECK(o[t["/Vec"]].is_array());
    CHECK(o[t["/Stuff"]].is_dict());

    auto a = o[t["/Vec"]].get_array();
    CHECK(a.size() == 3);
    CHECK(a[0].is_integer(1));
    CHECK(a[1].is_integer(2));
    CHECK(a[2].is_integer(3));

    auto d = o[t["/Stuff"]];
    CHECK(d.size() == 2);
    CHECK(d[t["/Start"]].is_ref(10, 0));
    CHECK(d[t["/End"]].is_ref(11, 0));
}
