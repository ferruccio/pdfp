#include "catch.hpp"
#include "tools.hpp"

using namespace pdf::tools;

TEST_CASE("slice: constructors", "[slice]") {
    slice a("");
    CHECK(a.empty());
    CHECK(a.length() == 0);

    slice b(".");
    CHECK(!b.empty());
    CHECK(b.length() == 1);
    CHECK(*b == '.');

    slice c("xyzzy", 3);
    CHECK(!c.empty());
    CHECK(c.length() == 3);
    CHECK(c == slice("xyz"));
    CHECK(*c == 'x');
}

TEST_CASE("slice: left", "[slice]") {
    slice s("xyzzy");
    CHECK(s.left(0) == slice(""));
    CHECK(s.left(1) == slice("x"));
    CHECK(s.left(2) == slice("xy"));
    CHECK(s.left(3) == slice("xyz"));
    CHECK(s.left(4) == slice("xyzz"));
    CHECK(s.left(5) == slice("xyzzy"));
    CHECK(s.left(6) == slice("xyzzy"));
}

TEST_CASE("slice: remove_left", "[slice]") {
    slice s("xyzzy");
    CHECK(s.remove_left(0) == slice("xyzzy"));
    CHECK(s.remove_left(1) == slice("yzzy"));
    CHECK(s.remove_left(2) == slice("zzy"));
    CHECK(s.remove_left(3) == slice("zy"));
    CHECK(s.remove_left(4) == slice("y"));
    CHECK(s.remove_left(5) == slice(""));
    CHECK(s.remove_left(6) == slice(""));
}

TEST_CASE("slice: operator=", "[slice]") {
    CHECK(slice("") == slice(""));
    CHECK(slice("a") == slice("a"));
    CHECK(slice("xyzzy") == slice("xyzzy"));
}

TEST_CASE("slice: operator!=", "[slice]") {
    CHECK(slice("") != slice("."));
    CHECK(slice("xyz") != slice("xy"));
    CHECK(slice("xy") != slice("xyz"));
    CHECK(slice("xyzzy") != slice("XYZZY"));
}

TEST_CASE("slice: operator*", "[slice]") {
    CHECK(*slice(".") == '.');
    CHECK(*slice("xyzzy") == 'x');
}

TEST_CASE("slice: first", "[slice]") {
    CHECK(slice(".").first() == '.');
    CHECK(slice("xyzzy").first() == 'x');
    CHECK_THROWS(slice("").first());
}

TEST_CASE("slice: rest", "[slice]") {
    CHECK(slice("").rest() == slice(""));
    CHECK(slice("x").rest() == slice(""));
    CHECK(slice("xy").rest() == slice("y"));
    CHECK(slice("xyz").rest() == slice("yz"));
    CHECK(slice("xyzz").rest() == slice("yzz"));
    CHECK(slice("xyzzy").rest() == slice("yzzy"));
}

TEST_CASE("slice: starts_with", "[slice]") {
    CHECK(slice("xyzzy").starts_with(""));
    CHECK(slice("xyzzy").starts_with("x"));
    CHECK(slice("xyzzy").starts_with("xy"));
    CHECK(slice("xyzzy").starts_with("xyz"));
    CHECK(slice("xyzzy").starts_with("xyzz"));
    CHECK(slice("xyzzy").starts_with("xyzzy"));
    CHECK(!slice("xyzzy").starts_with("xyzzyx"));
    CHECK(!slice("xyzzy").starts_with("plugh"));
}

TEST_CASE("slice: skip", "[slice]") {
    CHECK(slice("").skip(0) == slice(""));
    CHECK(slice("").skip(1) == slice(""));
    CHECK(slice("xyzzy").skip(0) == slice("xyzzy"));
    CHECK(slice("xyzzy").skip(1) == slice("yzzy"));
    CHECK(slice("xyzzy").skip(2) == slice("zzy"));
    CHECK(slice("xyzzy").skip(3) == slice("zy"));
    CHECK(slice("xyzzy").skip(4) == slice("y"));
    CHECK(slice("xyzzy").skip(5) == slice(""));
    CHECK(slice("xyzzy").skip(6) == slice(""));
}

namespace {
    auto isz(char c) -> bool { return c == 'z'; }
    auto notz(char c) -> bool { return c != 'z'; }
    auto fail(char) -> bool { return false; }
    auto succeed(char) -> bool { return true; }
}

TEST_CASE("slice: skip_until", "[slice]") {
    CHECK(slice("xyzzy").skip_until(isz) == slice("zzy"));
    CHECK(slice("xyzzy").skip_until(notz) == slice("xyzzy"));
    CHECK(slice("xyzzy").skip_until(fail) == slice(""));
    CHECK(slice("xyzzy").skip_until(succeed) == slice("xyzzy"));
}

TEST_CASE("slice: skip_while", "[slice]") {
    CHECK(slice("xyzzy").skip_while(isz) == slice("xyzzy"));
    CHECK(slice("xyzzy").skip_while(notz) == slice("zzy"));
    CHECK(slice("xyzzy").skip_while(fail) == slice("xyzzy"));
    CHECK(slice("xyzzy").skip_while(succeed) == slice(""));
}

TEST_CASE("slice: take_until", "[slice]") {
    CHECK(slice("xyzzy").take_until(isz) == slice("xy"));
    CHECK(slice("xyzzy").take_until(notz) == slice(""));
    CHECK(slice("xyzzy").take_until(fail) == slice("xyzzy"));
    CHECK(slice("xyzzy").take_until(succeed) == slice(""));
}

TEST_CASE("slice: take_while", "[slice]") {
    CHECK(slice("xyzzy").take_while(isz) == slice(""));
    CHECK(slice("xyzzy").take_while(notz) == slice("xy"));
    CHECK(slice("xyzzy").take_while(fail) == slice(""));
    CHECK(slice("xyzzy").take_while(succeed) == slice("xyzzy"));
}
