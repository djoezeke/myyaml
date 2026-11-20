/**
 * @file Main.test.cpp
 * @brief Unit tests for the C++ API.
 */

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <myyaml/myyaml.hpp>

#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace myyaml;

namespace
{
    std::string temp_path(const char *name)
    {
        return std::string("myyaml-test-") + name;
    }

    struct file_guard
    {
        explicit file_guard(std::string path) : path(std::move(path)) {}
        ~file_guard()
        {
            std::remove(path.c_str());
        }
        std::string path;
    };
} // namespace

TEST_CASE("version information", "[version]")
{
    CHECK(MYYAML_VERSION_MAJOR == 0);
    CHECK(MYYAML_VERSION_MINOR == 2);
    CHECK(MYYAML_VERSION_PATCH == 0);
    CHECK(std::string(MYYAML_VERSION) == "0.2.0");
    CHECK(library_version() == version(0, 2, 0));
    CHECK(std::string(library_version_string()) == "0.2.0");
    CHECK(version(0, 2, 0) < version(0, 3, 0));
    CHECK(version(1, 0, 0) > version(0, 9, 9));
    CHECK(version(0, 2, 0) <= version(0, 2, 0));
}

TEST_CASE("core schema scalar inference", "[scalar]")
{
    CHECK(yaml::parse("null").is_null());
    CHECK(yaml::parse("~").is_null());
    CHECK(yaml::parse("Null").is_null());
    CHECK(yaml::parse("NULL").is_null());
    CHECK(yaml::parse("").is_null());

    CHECK(yaml::parse("true").as_boolean());
    CHECK_FALSE(yaml::parse("False").as_boolean());
    CHECK(yaml::parse("TRUE").as_boolean());

    CHECK(yaml::parse("42").as_integer() == 42);
    CHECK(yaml::parse("+17").as_integer() == 17);
    CHECK(yaml::parse("-17").as_integer() == -17);
    CHECK(yaml::parse("0").as_integer() == 0);
    CHECK(yaml::parse("0x1F").as_integer() == 31);
    CHECK(yaml::parse("0o17").as_integer() == 15);
    CHECK(yaml::parse("0b1010").as_integer() == 10);
    CHECK(yaml::parse("1_000_000").as_integer() == 1000000);
    CHECK(yaml::parse("9223372036854775807").as_integer() ==
          std::numeric_limits<std::int64_t>::max());
    CHECK(yaml::parse("-9223372036854775808").as_integer() ==
          std::numeric_limits<std::int64_t>::min());

    CHECK(yaml::parse("3.5").as_floating() == Catch::Approx(3.5));
    CHECK(yaml::parse("-0.25").as_floating() == Catch::Approx(-0.25));
    CHECK(yaml::parse("1e3").as_floating() == Catch::Approx(1000.0));
    CHECK(yaml::parse("1.5e-2").as_floating() == Catch::Approx(0.015));
    CHECK(std::isinf(yaml::parse(".inf").as_floating()));
    CHECK(std::isinf(yaml::parse("-.Inf").as_floating()));
    CHECK(std::isnan(yaml::parse(".nan").as_floating()));

    CHECK(yaml::parse("hello").as_string() == "hello");
    CHECK(yaml::parse("hello world").as_string() == "hello world");
    CHECK(yaml::parse("0xZZ").as_string() == "0xZZ");
    CHECK(yaml::parse("9223372036854775808").is_string());
    CHECK(yaml::parse("yes").is_string());
    CHECK(yaml::parse("on").is_string());
}

TEST_CASE("quoted scalar decoding", "[scalar][string]")
{
    CHECK(yaml::parse("'hello world'").as_string() == "hello world");
    CHECK(yaml::parse("'it''s'").as_string() == "it's");
    CHECK(yaml::parse("\"hello\\nworld\"").as_string() == "hello\nworld");
    CHECK(yaml::parse("\"tab\\there\"").as_string() == "tab\there");
    CHECK(yaml::parse("\"\\x41\\u00e9\\U0001F600\"").as_string() ==
          std::string("A\xC3\xA9\xF0\x9F\x98\x80"));
    CHECK(yaml::parse("\"quote: \\\" slash: \\\\\"").as_string() ==
          "quote: \" slash: \\");
    CHECK(yaml::parse("\"line1\n  line2\"").as_string() == "line1 line2");
    CHECK(yaml::parse("\"line1\n\n  line2\"").as_string() == "line1\nline2");
    CHECK(yaml::parse("'line1\n  line2'").as_string() == "line1 line2");
    CHECK(yaml::parse("\"line1 \\\n  line2\"").as_string() == "line1 line2");
    CHECK(yaml::parse("\"\\0\"").as_string().size() == 1);
    CHECK(yaml::parse("\"\\0\"").as_string()[0] == '\0');

    CHECK_THROWS_AS(yaml::parse("\"unterminated"), parse_error);
    CHECK_THROWS_AS(yaml::parse("\"bad \\q escape\""), parse_error);
    CHECK_THROWS_AS(yaml::parse("\"\\uD800\""), parse_error);
    CHECK_THROWS_AS(yaml::parse("\"\\U00110000\""), parse_error);
    CHECK_THROWS_AS(yaml::parse("\"\\u12\""), parse_error);
}

TEST_CASE("block mappings", "[collection][mapping]")
{
    const yaml doc = yaml::parse("name: demo\ncount: 3\nenabled: true\nmissing:\n");
    REQUIRE(doc.is_mapping());
    CHECK(doc.size() == 4);
    CHECK(doc["name"].as_string() == "demo");
    CHECK(doc["count"].as_integer() == 3);
    CHECK(doc["enabled"].as_boolean());
    CHECK(doc["missing"].is_null());
    CHECK(doc.keys() == std::vector<std::string>{"name", "count", "enabled", "missing"});

    const yaml nested = yaml::parse("a:\n  b:\n    c: 1\n  d: 2\ne: 3\n");
    CHECK(nested["a"]["b"]["c"].as_integer() == 1);
    CHECK(nested["a"]["d"].as_integer() == 2);
    CHECK(nested["e"].as_integer() == 3);

    const yaml compact = yaml::parse("- a: 1\n  b: 2\n- c: 3\n");
    REQUIRE(compact.is_sequence());
    CHECK(compact[0]["a"].as_integer() == 1);
    CHECK(compact[0]["b"].as_integer() == 2);
    CHECK(compact[1]["c"].as_integer() == 3);

    const yaml quoted_keys = yaml::parse("\"a: b\": 1\n'c''d': 2\na.b: 3\n");
    CHECK(quoted_keys["a: b"].as_integer() == 1);
    CHECK(quoted_keys["c'd"].as_integer() == 2);
    CHECK(quoted_keys["a.b"].as_integer() == 3);
}

TEST_CASE("block sequences", "[collection][sequence]")
{
    const yaml doc = yaml::parse("- one\n- two\n- 3\n- true\n- null\n");
    REQUIRE(doc.is_sequence());
    CHECK(doc.size() == 5);
    CHECK(doc[0].as_string() == "one");
    CHECK(doc[1].as_string() == "two");
    CHECK(doc[2].as_integer() == 3);
    CHECK(doc[3].as_boolean());
    CHECK(doc[4].is_null());

    const yaml nested = yaml::parse("- - 1\n  - 2\n- - 3\n  - 4\n");
    CHECK(nested[0][0].as_integer() == 1);
    CHECK(nested[0][1].as_integer() == 2);
    CHECK(nested[1][0].as_integer() == 3);

    const yaml empty_item = yaml::parse("-\n  a: 1\n-\n");
    CHECK(empty_item[0]["a"].as_integer() == 1);
    CHECK(empty_item[1].is_null());
}

TEST_CASE("flow collections", "[collection][flow]")
{
    const yaml seq = yaml::parse("[1, 2, three, true, null, [4, 5], {a: 6}]");
    REQUIRE(seq.is_sequence());
    CHECK(seq.size() == 7);
    CHECK(seq[2].as_string() == "three");
    CHECK(seq[5][1].as_integer() == 5);
    CHECK(seq[6]["a"].as_integer() == 6);

    const yaml map = yaml::parse("{a: 1, b: {c: 2}, d: [3, 4], e: \"x, y\"}");
    CHECK(map["a"].as_integer() == 1);
    CHECK(map["b"]["c"].as_integer() == 2);
    CHECK(map["d"][0].as_integer() == 3);
    CHECK(map["e"].as_string() == "x, y");

    const yaml quoted = yaml::parse("[\"a]b\", 'c}d', don't, a\"b]");
    CHECK(quoted.size() == 4);
    CHECK(quoted[0].as_string() == "a]b");
    CHECK(quoted[1].as_string() == "c}d");
    CHECK(quoted[2].as_string() == "don't");
    CHECK(quoted[3].as_string() == "a\"b");

    const yaml multiline = yaml::parse("[\n  1,\n  2, # comment\n  3\n]");
    CHECK(multiline.size() == 3);

    const yaml trailing = yaml::parse("[1, 2, ]");
    CHECK(trailing.size() == 2);
    const yaml trailing_map = yaml::parse("{a: 1, }");
    CHECK(trailing_map.size() == 1);

    const yaml implicit = yaml::parse("[a: 1, b: 2]");
    REQUIRE(implicit.size() == 1);
    CHECK(implicit[0]["a"].as_integer() == 1);
    CHECK(implicit[0]["b"].as_integer() == 2);
}

TEST_CASE("block scalars", "[scalar][block]")
{
    CHECK(yaml::parse("a: |\n  line1\n  line2\n")["a"].as_string() == "line1\nline2\n");
    CHECK(yaml::parse("a: |-\n  line1\n  line2\n")["a"].as_string() == "line1\nline2");
    CHECK(yaml::parse("a: |+\n  line1\n\n\n")["a"].as_string() == "line1\n\n\n");
    CHECK(yaml::parse("a: |\n  one\n\n  two\n")["a"].as_string() == "one\n\ntwo\n");
    CHECK(yaml::parse("a: >\n  line1\n  line2\n")["a"].as_string() == "line1 line2\n");
    CHECK(yaml::parse("a: >-\n  line1\n  line2\n")["a"].as_string() == "line1 line2");
    CHECK(yaml::parse("a: >\n  line1\n\n  line2\n")["a"].as_string() == "line1\nline2\n");
    CHECK(yaml::parse("a: |2\n    indented\n")["a"].as_string() == "  indented\n");
    CHECK(yaml::parse("a: |\n  keep # not a comment\n")["a"].as_string() ==
          "keep # not a comment\n");
    CHECK(yaml::parse("- |\n  a\n- >\n  b\n  c\n")[1].as_string() == "b c\n");
}

TEST_CASE("anchors and aliases", "[anchor][alias]")
{
    const yaml doc = yaml::parse(
        "base: &base\n  x: 1\n  y: [1, 2]\ncopy: *base\nscalar: &s hello\nagain: *s\n");
    CHECK(doc["copy"]["x"].as_integer() == 1);
    CHECK(doc["copy"]["y"][1].as_integer() == 2);
    CHECK(doc["again"].as_string() == "hello");

    const yaml flow = yaml::parse("[&a 1, *a, *a]");
    CHECK(flow.size() == 3);
    CHECK(flow[2].as_integer() == 1);

    const yaml flow_map = yaml::parse("{a: &x {k: 2}, b: *x}");
    CHECK(flow_map["b"]["k"].as_integer() == 2);

    CHECK_THROWS_AS(yaml::parse("a: *missing\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: &x 1\nb: *x\nc: *y\n"), parse_error);
}

TEST_CASE("tags", "[tag]")
{
    const yaml doc = yaml::parse(
        "s: !!str 123\ni: !!int '42'\nf: !!float '3.5'\nb: !!bool 'true'\nn: !!null x\n");
    CHECK(doc["s"].is_string());
    CHECK(doc["s"].as_string() == "123");
    CHECK(doc["i"].as_integer() == 42);
    CHECK(doc["f"].as_floating() == Catch::Approx(3.5));
    CHECK(doc["b"].as_boolean());
    CHECK(doc["n"].is_null());
    CHECK(yaml::parse("a: !<tag:yaml.org,2002:str> 12\n")["a"].is_string());
    CHECK_THROWS_AS(yaml::parse("a: !!int nope\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: !!unknown value\n"), parse_error);
}

TEST_CASE("comments, directives and markers", "[document]")
{
    const yaml doc = yaml::parse(
        "%YAML 1.2\n---\n# leading comment\na: 1 # trailing comment\n\n# another\nb: 2\n...\n");
    CHECK(doc["a"].as_integer() == 1);
    CHECK(doc["b"].as_integer() == 2);

    CHECK(yaml::parse("# only a comment\n").is_null());
    CHECK(yaml::parse("---\n").is_null());
    CHECK_THROWS_AS(yaml::parse("%YAML 2.0\n---\na: 1\n"), parse_error);

    CHECK_THROWS_AS(yaml::parse("a: 1\n---\nb: 2\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("--- inline\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: 1\nb\n"), parse_error);
}

TEST_CASE("type queries and conversions", "[access]")
{
    const yaml doc = yaml::parse("null: null\nbool: true\nint: 1\nfloat: 1.5\nstr: x\nseq: [1]\nmap: {a: 1}\n");

    CHECK(doc["null"].type() == node_t::null);
    CHECK(doc["bool"].type() == node_t::boolean);
    CHECK(doc["int"].type() == node_t::integer);
    CHECK(doc["float"].type() == node_t::floating);
    CHECK(doc["str"].type() == node_t::string);
    CHECK(doc["seq"].type() == node_t::sequence);
    CHECK(doc["map"].type() == node_t::mapping);

    CHECK(std::string(doc["int"].type_name()) == "integer");
    CHECK(doc["int"].is_scalar());
    CHECK_FALSE(doc["seq"].is_scalar());
    CHECK(doc["int"].is_number());
    CHECK(doc["float"].get_number() == Catch::Approx(1.5));
    CHECK(doc["int"].get_number() == Catch::Approx(1.0));

    CHECK(doc["bool"].get_boolean() == true);
    CHECK(doc["int"].get_integer() == 1);
    CHECK(doc["float"].get_floating() == Catch::Approx(1.5));
    CHECK(doc["str"].get_string() == "x");
    CHECK(doc["str"].get_string_view() == std::string_view("x"));
    CHECK(doc["null"].get_integer() == std::nullopt);

    CHECK(doc["int"].as_integer() == 1);
    CHECK(doc["str"].as_c_str() == std::string("x"));
    CHECK(doc["seq"].as_sequence().size() == 1);
    CHECK(doc["map"].as_mapping().size() == 1);

    CHECK_THROWS_AS(doc["int"].as_string(), type_error);
    CHECK_THROWS_AS(doc["str"].as_integer(), type_error);
    CHECK_THROWS_AS(doc["seq"].as_mapping(), type_error);
}

TEST_CASE("mapping and sequence manipulation", "[mutation]")
{
    yaml doc;
    REQUIRE(doc.is_null());

    doc["name"] = "demo";
    doc["count"] = 3;
    doc["nested"]["x"] = true;
    doc["items"].push_back(1);
    doc["items"].push_back("two");
    doc["items"].push_back(3.5);

    REQUIRE(doc.is_mapping());
    CHECK(doc.size() == 4);
    CHECK(doc["name"].as_string() == "demo");
    CHECK(doc["nested"]["x"].as_boolean());
    CHECK(doc["items"].size() == 3);

    CHECK(doc.contains("name"));
    CHECK_FALSE(doc.contains("missing"));
    REQUIRE(doc.find("name") != nullptr);
    CHECK(doc.find("name")->as_string() == "demo");
    CHECK(doc.find("missing") == nullptr);

    doc.set("name", "changed");
    CHECK(doc["name"].as_string() == "changed");
    doc["items"][1] = "second";
    CHECK(doc["items"][1].as_string() == "second");

    CHECK(doc.erase("name"));
    CHECK_FALSE(doc.erase("name"));
    CHECK_FALSE(doc.erase(std::size_t{0}));

    yaml indexed = yaml::make_sequence({10, 20, 30});
    CHECK(indexed.erase(std::size_t{0}));
    CHECK_FALSE(indexed.erase(std::size_t{99}));
    CHECK(indexed.size() == 2);
    CHECK(indexed[0].as_integer() == 20);

    yaml list = yaml::make_sequence({1, 2, 3});
    CHECK(list[0].as_integer() == 1);
    list.push_back(4);
    CHECK(list.size() == 4);
    list.clear();
    CHECK(list.is_sequence());
    CHECK(list.empty());

    yaml object = yaml::make_mapping({{"a", 1}, {"b", 2}});
    CHECK(object.size() == 2);
    object.clear();
    CHECK(object.is_mapping());
    CHECK(object.empty());

    yaml scalar = "text";
    scalar.clear();
    CHECK(scalar.is_string());
    CHECK(scalar.as_string().empty());
}

TEST_CASE("copy and move semantics", "[value]")
{
    const yaml original = yaml::parse("a:\n  b: [1, 2]\n");
    yaml copy = original;
    copy["a"]["b"][0] = 99;
    CHECK(original["a"]["b"][0].as_integer() == 1);
    CHECK(copy["a"]["b"][0].as_integer() == 99);

    yaml moved = std::move(copy);
    CHECK(moved["a"]["b"][0].as_integer() == 99);

    yaml assigned;
    assigned = original;
    CHECK(assigned == original);

    yaml swapped = yaml::make_sequence({1, 2, 3});
    const yaml before = swapped;
    swap(swapped, assigned);
    CHECK(swapped == original);
    CHECK(assigned == before);
}

TEST_CASE("equality", "[value]")
{
    CHECK(yaml::parse("a: 1\nb: 2\n") == yaml::parse("b: 2\na: 1\n"));
    CHECK(yaml::parse("[1, 2, 3]") == yaml::parse("[1, 2, 3]"));
    CHECK(yaml::parse("1") != yaml::parse("1.0"));
    CHECK(yaml::parse(".nan") == yaml::parse(".nan"));
    CHECK(yaml::parse("null") == yaml());
    CHECK(yaml::parse("a: 1") != yaml::parse("a: 2"));
}

TEST_CASE("dumping and round trips", "[emit]")
{
    const std::vector<std::string> documents = {
        "a: 1\nb: hello\nc: true\nd: null\ne:\n  - 1\n  - x\ny:\n  z: 3\n",
        "- 1\n- two\n- false\n- null\n",
        "text: |\n  line1\n  line2\n",
        "a: \"has: colon\"\nb: \"# hash\"\nc: \"true\"\n",
        "empty: {}\nseq: []\n",
        "unicode: caf\xC3\xA9 \xF0\x9F\x98\x80\n",
        "nested:\n  - a: 1\n    b:\n      - 2\n      - 3\n  - c: 4\n",
        "root:\n  - - 1\n    - 2\n",
    };

    dump_options flow_options;
    flow_options.flow = true;

    for (const std::string &document : documents)
    {
        CAPTURE(document);
        const yaml first = yaml::parse(document);
        const std::string block = first.dump();
        CHECK(yaml::parse(block) == first);

        const std::string flow = first.dump(flow_options);
        CHECK(yaml::parse(flow) == first);
    }
}

TEST_CASE("emit options", "[emit]")
{
    const yaml doc = yaml::parse("b: 2\na: 1\nnested:\n  - 1\n  - 2\n");

    dump_options sorted;
    sorted.sort_keys = true;
    const std::string sorted_text = doc.dump(sorted);
    CHECK(sorted_text.find("a: 1") < sorted_text.find("b: 2"));

    dump_options marked;
    marked.explicit_start = true;
    marked.explicit_end = true;
    const std::string marked_text = doc.dump(marked);
    CHECK(marked_text.rfind("---\n", 0) == 0);
    CHECK(marked_text.find("\n...\n") != std::string::npos);
    CHECK(yaml::parse(marked_text) == doc);

    dump_options indented;
    indented.indent = 4;
    const std::string indented_text = doc.dump(indented);
    CHECK(indented_text.find("\n    - 1") != std::string::npos);
    CHECK(yaml::parse(indented_text) == doc);

    dump_options compact;
    compact.trailing_newline = false;
    CHECK(doc.dump(compact).back() != '\n');

    dump_options invalid;
    invalid.indent = 0;
    CHECK_THROWS_AS(doc.dump(invalid), type_error);
    invalid.indent = 99;
    CHECK_THROWS_AS(doc.dump(invalid), type_error);
}

TEST_CASE("tricky strings survive emission", "[emit][string]")
{
    const std::vector<std::string> values = {
        "",        " ",         "a ",        " a",        "a:b",
        "a: b",    "#hash",     "a # b",     "true",      "false",
        "null",    "~",         "123",       "1.5",       "0x1F",
        "- x",     "[x]",       "{x}",       "a,b",       "a\nb",
        "a\nb\n",  "a\nb\n\n",  "\t",        "tab\there", "quote\"and'apos",
        "back\\slash", "caf\xC3\xA9", "\xF0\x9F\x98\x80", "!", "&anchor",
        "*alias",  "? key",     ": value",   "%directive", "@at",
        "`tick",   "|pipe",     ">fold",     "---",       "...",
        "abc:",    "http://example.com", "C:\\path", "don't",
    };

    dump_options flow_options;
    flow_options.flow = true;

    for (const std::string &text : values)
    {
        CAPTURE(text);
        const yaml value = text;
        CHECK(yaml::parse(value.dump()) == value);
        CHECK(yaml::parse(value.dump(flow_options)) == value);

        const yaml mapping = yaml::make_mapping({{text, value}});
        CHECK(yaml::parse(mapping.dump()) == mapping);
        CHECK(yaml::parse(mapping.dump(flow_options)) == mapping);

        const yaml sequence = yaml::make_sequence({value, value});
        CHECK(yaml::parse(sequence.dump()) == sequence);
        CHECK(yaml::parse(sequence.dump(flow_options)) == sequence);
    }
}

TEST_CASE("numeric and scalar emission", "[emit]")
{
    CHECK(yaml().dump() == "null\n");
    CHECK(yaml(true).dump() == "true\n");
    CHECK(yaml(false).dump() == "false\n");
    CHECK(yaml(42).dump() == "42\n");
    CHECK(yaml(-42).dump() == "-42\n");
    CHECK(yaml(1.5).dump() == "1.5\n");
    CHECK(yaml(std::string("text")).dump() == "text\n");
    CHECK(yaml::make_sequence({}).dump() == "[]\n");
    CHECK(yaml::make_mapping({}).dump() == "{}\n");
    CHECK(yaml::make_sequence({1, 2}).to_string() == "[1, 2]");
    CHECK(yaml::make_mapping({{"a", 1}}).to_string() == "{a: 1}");
}

TEST_CASE("construction helpers", "[value]")
{
    CHECK(yaml(nullptr).is_null());
    CHECK(yaml(true).as_boolean());
    CHECK(yaml(1).as_integer() == 1);
    CHECK(yaml(1L).as_integer() == 1);
    CHECK(yaml(1LL).as_integer() == 1);
    CHECK(yaml(1U).as_integer() == 1);
    CHECK(yaml(1UL).as_integer() == 1);
    CHECK(yaml(1ULL).as_integer() == 1);
    CHECK(yaml(1.5f).as_floating() == Catch::Approx(1.5));
    CHECK(yaml(1.5).as_floating() == Catch::Approx(1.5));
    CHECK(yaml("text").as_string() == "text");
    CHECK(yaml(std::string("text")).as_string() == "text");
    CHECK(yaml(std::string_view("text")).as_string() == "text");

    yaml list = yaml::make_sequence();
    list.emplace_back(1);
    list.emplace_back("two");
    CHECK(list.size() == 2);
    CHECK(list[1].as_string() == "two");

    const yaml built = yaml::make_mapping({
        {"name", "demo"},
        {"values", yaml::make_sequence({1, 2, 3})},
    });
    CHECK(built["name"].as_string() == "demo");
    CHECK(built["values"][2].as_integer() == 3);
}

TEST_CASE("typed errors", "[error]")
{
    yaml scalar = 1;
    CHECK_THROWS_AS(scalar["key"], type_error);
    CHECK_THROWS_AS(scalar.at(std::size_t{0}), type_error);
    CHECK_THROWS_AS(scalar.as_string(), type_error);

    yaml mapping = yaml::make_mapping({{"a", 1}});
    CHECK_THROWS_AS(mapping.at("missing"), lookup_error);
    CHECK_THROWS_AS(mapping[std::size_t{0}], type_error);

    yaml sequence = yaml::make_sequence({1});
    CHECK_THROWS_AS(sequence[1], lookup_error);
    CHECK_THROWS_AS(sequence.at(1), lookup_error);
    CHECK_THROWS_AS(sequence["key"], type_error);

    CHECK_NOTHROW(mapping.at("a"));
    CHECK_NOTHROW(sequence.at(0));
}

TEST_CASE("parse errors carry positions", "[error]")
{
    try
    {
        (void)yaml::parse("a: 1\nb: [1, 2\n");
        FAIL("expected parse_error");
    }
    catch (const parse_error &error)
    {
        CHECK(error.line() >= 1);
        CHECK(error.column() >= 1);
        CHECK(error.index() > 0);
        CHECK(std::string(error.what()).find("line") != std::string::npos);
    }

    CHECK_THROWS_AS(yaml::parse("a: 1\na: 2\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("\ta: 1\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a:\n\tb: 1\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: [1, 2\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: {b: 1\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: [1}\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: 'x\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: \"x\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: 1\n  b: 2\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: [1, 2]]\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse(std::string("a: \0b\n", 6)), parse_error);
}

TEST_CASE("duplicate keys", "[error][options]")
{
    CHECK_THROWS_AS(yaml::parse("a: 1\na: 2\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("{a: 1, a: 2}"), parse_error);

    parse_options options;
    options.allow_duplicate_keys = true;
    const yaml block = yaml::parse("a: 1\na: 2\n", options);
    CHECK(block.size() == 2);
    const yaml flow = yaml::parse("{a: 1, a: 2}", options);
    CHECK(flow.size() == 2);
}

TEST_CASE("parser limits", "[error][options]")
{
    parse_options depth;
    depth.max_depth = 4;
    CHECK_THROWS_AS(yaml::parse("[[[[[[1]]]]]]", depth), parse_error);
    CHECK_NOTHROW(yaml::parse("[1, 2]", depth));

    parse_options size;
    size.max_input_size = 4;
    CHECK_THROWS_AS(yaml::parse("12345", size), parse_error);

    parse_options nodes;
    nodes.max_nodes = 2;
    CHECK_THROWS_AS(yaml::parse("[1, 2, 3, 4]", nodes), parse_error);
}

TEST_CASE("non-throwing parse", "[error][try_parse]")
{
    {
        const parse_result result = yaml::try_parse("a: 1\n");
        REQUIRE(static_cast<bool>(result));
        REQUIRE(result.value.has_value());
        CHECK((*result.value)["a"].as_integer() == 1);
        CHECK(result.error.empty());
    }
    {
        const parse_result result = yaml::try_parse("a: [1\n");
        CHECK_FALSE(static_cast<bool>(result));
        CHECK_FALSE(result.value.has_value());
        CHECK_FALSE(result.error.empty());
        CHECK(result.line == 1);
        CHECK(result.column >= 1);
    }
}

TEST_CASE("encoding support", "[encoding]")
{
    SECTION("UTF-8 BOM")
    {
        const std::string text = "\xEF\xBB\xBFkey: value\n";
        CHECK(yaml::parse(text)["key"].as_string() == "value");
    }

    SECTION("UTF-16LE with BOM")
    {
        std::string bytes = "\xFF\xFE";
        for (char c : std::string("key: value\n"))
        {
            bytes.push_back(c);
            bytes.push_back('\0');
        }
        CHECK(yaml::parse(bytes)["key"].as_string() == "value");
    }

    SECTION("UTF-16BE with BOM")
    {
        std::string bytes = "\xFE\xFF";
        for (char c : std::string("key: value\n"))
        {
            bytes.push_back('\0');
            bytes.push_back(c);
        }
        CHECK(yaml::parse(bytes)["key"].as_string() == "value");
    }

    SECTION("UTF-32LE with BOM")
    {
        // Use an explicit length: the UTF-32LE BOM contains NUL bytes, so the
        // const char* constructor would truncate the string at the first NUL.
        std::string bytes("\xFF\xFE\x00\x00", 4);
        for (char c : std::string("key: value\n"))
        {
            bytes.push_back(c);
            bytes.push_back('\0');
            bytes.push_back('\0');
            bytes.push_back('\0');
        }
        CHECK(yaml::parse(bytes)["key"].as_string() == "value");
    }

    SECTION("UTF-32BE with BOM")
    {
        // The BOM starts with NUL bytes; construct it with an explicit length.
        std::string bytes("\x00\x00\xFE\xFF", 4);
        for (char c : std::string("key: value\n"))
        {
            bytes.push_back('\0');
            bytes.push_back('\0');
            bytes.push_back('\0');
            bytes.push_back(c);
        }
        CHECK(yaml::parse(bytes)["key"].as_string() == "value");
    }

    SECTION("invalid UTF-8")
    {
        CHECK_THROWS_AS(yaml::parse(std::string("a: \xC3\x28\n")), encoding_error);
        CHECK_THROWS_AS(yaml::parse(std::string("\xF5\x80\x80\x80")), encoding_error);
    }

    SECTION("invalid UTF-16")
    {
        const std::string bytes = "\xFF\xFE" + std::string("\x00\xD8", 2);
        CHECK_THROWS_AS(yaml::parse(bytes), encoding_error);
    }

    SECTION("invalid strings cannot be emitted")
    {
        const std::string invalid = "\xC3\x28";
        CHECK_THROWS_AS(yaml(invalid).dump(), encoding_error);
    }
}

TEST_CASE("stream and file input", "[io]")
{
    SECTION("std::istream")
    {
        std::istringstream stream("a: 1\nb: 2\n");
        const yaml doc = yaml::parse(stream);
        CHECK(doc["b"].as_integer() == 2);

        std::istringstream second("a: 1\n");
        yaml value;
        second >> value;
        CHECK(value["a"].as_integer() == 1);
    }

    SECTION("std::FILE")
    {
        file_guard guard(temp_path("file-input.yaml"));
        {
            std::FILE *file = std::fopen(guard.path.c_str(), "w");
            REQUIRE(file != nullptr);
            std::fputs("a: 1\n", file);
            std::fclose(file);
        }

        std::FILE *file = std::fopen(guard.path.c_str(), "r");
        REQUIRE(file != nullptr);
        const yaml doc = yaml::parse(file);
        std::fclose(file);
        CHECK(doc["a"].as_integer() == 1);
    }

    SECTION("file path")
    {
        file_guard guard(temp_path("file-input.yaml"));
        {
            std::ofstream file(guard.path, std::ios::binary);
            file << "a: 1\n";
        }
        CHECK(yaml::parse_file(guard.path)["a"].as_integer() == 1);
    }

    SECTION("missing file")
    {
        CHECK_THROWS_AS(yaml::parse_file(temp_path("definitely-missing.yaml")), io_error);
    }

    SECTION("null FILE")
    {
        CHECK_THROWS_AS(yaml::parse(static_cast<std::FILE *>(nullptr)), io_error);
    }

    SECTION("output stream")
    {
        const yaml doc = yaml::parse("a: 1\n");
        std::ostringstream stream;
        doc.dump(stream);
        CHECK(yaml::parse(stream.str()) == doc);

        std::ostringstream second;
        second << doc;
        CHECK(yaml::parse(second.str()) == doc);
    }
}

TEST_CASE("value sizes", "[value]")
{
    CHECK(yaml().size() == 0);
    CHECK(yaml(true).size() == 1);
    CHECK(yaml(1).size() == 1);
    CHECK(yaml(1.5).size() == 1);
    CHECK(yaml("abc").size() == 3);
    CHECK(yaml::make_sequence({1, 2}).size() == 2);
    CHECK(yaml::make_mapping({{"a", 1}}).size() == 1);
    CHECK(yaml().empty());
    CHECK(yaml("").empty());
    CHECK(yaml::make_sequence().empty());
    CHECK_FALSE(yaml(true).empty());
}

TEST_CASE("empty and whitespace-only documents", "[parse][edge]")
{
    CHECK(yaml::parse("").is_null());
    CHECK(yaml::parse("   ").is_null());
    CHECK(yaml::parse("\n\n\n").is_null());
    CHECK(yaml::parse("# only a comment\n\n# another comment\n").is_null());
    CHECK(yaml::parse("---\n").is_null());
    CHECK(yaml::parse("---\n...\n").is_null());
    CHECK(yaml::parse("...\n").is_null());
    CHECK(yaml::parse("null\n").is_null());
}

TEST_CASE("line ending normalization", "[parse][edge]")
{
    const yaml crlf = yaml::parse("a: 1\r\nb:\r\n  - x\r\n  - y\r\n");
    CHECK(crlf["a"].as_integer() == 1);
    CHECK(crlf["b"][1].as_string() == "y");

    const yaml cr = yaml::parse("a: 1\rb: 2\r");
    CHECK(cr["a"].as_integer() == 1);
    CHECK(cr["b"].as_integer() == 2);

    CHECK(yaml::parse("a: |\r\n  one\r\n  two\r\n")["a"].as_string() == "one\ntwo\n");
}

TEST_CASE("empty collections", "[parse][emit][edge]")
{
    const yaml empty_map = yaml::parse("{}");
    REQUIRE(empty_map.is_mapping());
    CHECK(empty_map.empty());
    CHECK(empty_map.dump() == "{}\n");

    const yaml empty_seq = yaml::parse("[]");
    REQUIRE(empty_seq.is_sequence());
    CHECK(empty_seq.empty());
    CHECK(empty_seq.dump() == "[]\n");

    dump_options flow_options;
    flow_options.flow = true;
    CHECK(empty_map.dump(flow_options) == "{}\n");
    CHECK(empty_seq.dump(flow_options) == "[]\n");
}

TEST_CASE("recursive aliases are rejected", "[anchor][alias][error]")
{
    CHECK_THROWS_AS(yaml::parse("a: &a [*a]\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: &a {self: *a}\n"), parse_error);
    CHECK_THROWS_AS(yaml::parse("a: &a\n  b: *a\n"), parse_error);
}

TEST_CASE("exact input size limit", "[parse][limits]")
{
    parse_options options;
    options.max_input_size = 4;
    CHECK_NOTHROW(yaml::parse("1234", options));
    CHECK_THROWS_AS(yaml::parse("12345", options), parse_error);
}

TEST_CASE("emitter option validation boundaries", "[emit][options][error]")
{
    const yaml document = yaml::parse("a: 1\n");

    dump_options minimum;
    minimum.indent = 1;
    CHECK_NOTHROW(document.dump(minimum));

    dump_options maximum;
    maximum.indent = 8;
    CHECK_NOTHROW(document.dump(maximum));

    dump_options too_large;
    too_large.indent = 9;
    CHECK_THROWS_AS(document.dump(too_large), type_error);

    dump_options negative;
    negative.indent = -1;
    CHECK_THROWS_AS(document.dump(negative), type_error);
}

TEST_CASE("stream failures", "[io][error]")
{
    const yaml document = yaml::parse("a: 1\n");

    std::ostringstream broken_output;
    broken_output.setstate(std::ios::badbit);
    CHECK_THROWS_AS(document.dump(broken_output), io_error);

    std::istringstream invalid_input("a: [1\n");
    yaml parsed;
    invalid_input >> parsed;
    CHECK(invalid_input.fail());
}

TEST_CASE("try_parse reports encoding errors", "[try_parse][encoding][error]")
{
    const parse_result result = yaml::try_parse(std::string("\xC3\x28", 2));
    CHECK_FALSE(static_cast<bool>(result));
    CHECK_FALSE(result.value.has_value());
    CHECK_FALSE(result.error.empty());
    CHECK(result.error.find("UTF-8") != std::string::npos);
}

TEST_CASE("large documents round trip", "[performance][roundtrip]")
{
    yaml::mapping entries;
    entries.reserve(500);
    for (int i = 0; i < 500; ++i)
    {
        entries.emplace_back("key" + std::to_string(i),
                             yaml::make_sequence({i, "value" + std::to_string(i), i % 2 == 0}));
    }
    const yaml document(std::move(entries));
    const std::string text = document.dump();
    CHECK(yaml::parse(text) == document);
    CHECK(text.size() > 10000);
}

TEST_CASE("thread-local parsing", "[thread]")
{
    std::vector<std::thread> threads;
    std::vector<int> results(4, 0);
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back([i, &results]()
                             {
                                 const yaml doc = yaml::parse("thread: " + std::to_string(i) + "\n");
                                 results[static_cast<std::size_t>(i)] =
                                     static_cast<int>(doc["thread"].as_integer());
                             });
    }
    for (std::thread &thread : threads)
    {
        thread.join();
    }
    for (int i = 0; i < 4; ++i)
    {
        CHECK(results[static_cast<std::size_t>(i)] == i);
    }
}
