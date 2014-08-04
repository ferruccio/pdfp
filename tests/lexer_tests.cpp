#include "catch.hpp"
#include "tools.hpp"
#include "lexer.hpp"

#include <tuple>

using pdf::tools::slice;
using pdf::token_type;
using pdf::token;
using pdf::next_token;

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