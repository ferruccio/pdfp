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
    CHECK(t.value() == "name");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::string);
    CHECK(t.value() == "string");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::hexstring);
    CHECK(t.value() == "deadbeef");

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
    CHECK(t.value() == "name");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::string);
    CHECK(t.value() == "string");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::hexstring);
    CHECK(t.value() == "deadbeef");

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
    CHECK(t.value() == "name");

    s = "keyword \r /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "name");

    s = "keyword \r\n /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "name");

    s = "keyword %comment\n /name";
    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::keyword);
    CHECK(t.value() == "keyword");

    tie(t, s) = next_token(s);
    CHECK(t.type() == token_type::name);
    CHECK(t.value() == "name");
}

TEST_CASE("next_token: strings", "[lexer]") {
    auto tok = next_token("(simple (nested) string)");
    CHECK(std::get<0>(tok).value() == "simple (nested) string");

    tok = next_token("(quoted \\( lparen))");
    CHECK(std::get<0>(tok).value() == "quoted \\( lparen");

    tok = next_token("(quoted \\) rparen)");
    CHECK(std::get<0>(tok).value() == "quoted \\) rparen");

    tok = next_token("(quoted \\\\ backslash)");
    CHECK(std::get<0>(tok).value() == "quoted \\\\ backslash");

    tok = next_token("(ignored \\backslash");
    CHECK(std::get<0>(tok).value() == "ignored \\backslash");

    tok = next_token("(\\n \\r \\t \\b \\f \\( \\) \\\\ \\000 \\020 \\200 \\377)");
    CHECK(std::get<0>(tok).value() == "\\n \\r \\t \\b \\f \\( \\) \\\\ \\000 \\020 \\200 \\377");
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

TEST_CASE("next_object: null", "[parser]") {
    using namespace pdf;

    atom_table t { get_pdf_atoms() };
    parser p(" null ", t);
    auto o = p.next_object();
    INFO("no: " << o(t));
    CHECK(o.is_null());
}