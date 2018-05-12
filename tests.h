#pragma once

#include <iostream>
#include <assert.h>

#include "urlparser.h"

// TEST URLPARSER
void test_extract_json() {
    std::string s1 = "abcde {\"a\",0}fffff";
    std::string s2 = "abcde {\"llllz";
    std::string s3 = "abcde llllz";

    assert(UrlParser::instance().extractJson(s1) == "{\"a\",0}");
    assert(UrlParser::instance().extractJson(s2) == "{}");
    assert(UrlParser::instance().extractJson(s3) == "{}");
}

void test_urlparser() {
    test_extract_json();
}
