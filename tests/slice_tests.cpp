#include "catch.hpp"
#include "tools.hpp"

using pdf::tools::slice;

TEST_CASE("slice: constructors", "[slice]") {
    slice a("");
    CHECK(a == "");
    CHECK(a.empty());
    CHECK(a.length() == 0);

    slice b(".");
    CHECK(b == ".");
    CHECK(!b.empty());
    CHECK(b.length() == 1);
    CHECK(*b == '.');

    slice c("xyzzy", 3);
    CHECK(c == "xyz");
    CHECK(!c.empty());
    CHECK(c.length() == 3);
    CHECK(c == slice("xyz"));
    CHECK(*c == 'x');
    CHECK(c[0] == 'x');
    CHECK(c[1] == 'y');
    CHECK(c[2] == 'z');
}

TEST_CASE("slice: left", "[slice]") {
    slice s("xyzzy");
    CHECK(s.left(0) == "");
    CHECK(s.left(1) == "x");
    CHECK(s.left(2) == "xy");
    CHECK(s.left(3) == "xyz");
    CHECK(s.left(4) == "xyzz");
    CHECK(s.left(5) == "xyzzy");
    CHECK(s.left(6) == "xyzzy");
}

TEST_CASE("slice: remove_left", "[slice]") {
    slice s("xyzzy");
    CHECK(s.remove_left(0) == "xyzzy");
    CHECK(s.remove_left(1) == "yzzy");
    CHECK(s.remove_left(2) == "zzy");
    CHECK(s.remove_left(3) == "zy");
    CHECK(s.remove_left(4) == "y");
    CHECK(s.remove_left(5) == "");
    CHECK(s.remove_left(6) == "");
}

TEST_CASE("slice: operator==", "[slice]") {
    CHECK(slice("") == "");
    CHECK(slice("a") == "a");
    CHECK(slice("xyzzy") == "xyzzy");
}

TEST_CASE("slice: operator!=", "[slice]") {
    CHECK(slice("") != ".");
    CHECK(slice("xyz") != "xy");
    CHECK(slice("xy") != "xyz");
    CHECK(slice("xyzzy") != "XYZZY");
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
    CHECK(slice("").rest() == "");
    CHECK(slice("x").rest() == "");
    CHECK(slice("xy").rest() == "y");
    CHECK(slice("xyz").rest() == "yz");
    CHECK(slice("xyzz").rest() == "yzz");
    CHECK(slice("xyzzy").rest() == "yzzy");
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
    CHECK(slice("").skip(0) == "");
    CHECK(slice("").skip(1) == "");
    CHECK(slice("xyzzy").skip(0) == "xyzzy");
    CHECK(slice("xyzzy").skip(1) == "yzzy");
    CHECK(slice("xyzzy").skip(2) == "zzy");
    CHECK(slice("xyzzy").skip(3) == "zy");
    CHECK(slice("xyzzy").skip(4) == "y");
    CHECK(slice("xyzzy").skip(5) == "");
    CHECK(slice("xyzzy").skip(6) == "");
}

namespace {
    auto isz(char c) -> bool { return c == 'z'; }
    auto notz(char c) -> bool { return c != 'z'; }
    auto fail(char) -> bool { return false; }
    auto succeed(char) -> bool { return true; }
}

TEST_CASE("slice: skip_until", "[slice]") {
    CHECK(slice("xyzzy").skip_until(isz) == "zzy");
    CHECK(slice("xyzzy").skip_until(notz) == "xyzzy");
    CHECK(slice("xyzzy").skip_until(fail) == "");
    CHECK(slice("xyzzy").skip_until(succeed) == "xyzzy");
}

TEST_CASE("slice: skip_while", "[slice]") {
    CHECK(slice("xyzzy").skip_while(isz) == "xyzzy");
    CHECK(slice("xyzzy").skip_while(notz) == "zzy");
    CHECK(slice("xyzzy").skip_while(fail) == "xyzzy");
    CHECK(slice("xyzzy").skip_while(succeed) == "");
}

TEST_CASE("slice: take_until", "[slice]") {
    CHECK(slice("xyzzy").take_until(isz) == "xy");
    CHECK(slice("xyzzy").take_until(notz) == "");
    CHECK(slice("xyzzy").take_until(fail) == "xyzzy");
    CHECK(slice("xyzzy").take_until(succeed) == "");
}

TEST_CASE("slice: take_while", "[slice]") {
    CHECK(slice("xyzzy").take_while(isz) == "");
    CHECK(slice("xyzzy").take_while(notz) == "xy");
    CHECK(slice("xyzzy").take_while(fail) == "");
    CHECK(slice("xyzzy").take_while(succeed) == "xyzzy");
}

TEST_CASE("slice: find_last", "[slice]") {
    CHECK(slice("hic haec hoc").find_last("huic").empty());
    CHECK(slice("hic haec hoc").find_last("hoc") == "hoc");
    CHECK(slice("hic haec hoc").find_last("haec") == "haec hoc");
    CHECK(slice("hic haec hic hoc").find_last("hic") == "hic hoc");
}