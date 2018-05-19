#pragma once

#include <iostream>
#include <assert.h>

#include "requestparser.h"

// TEST URLPARSER
void test_extract_json() {
    std::string s1 = "abcde {\"a\",0}fffff";
    std::string s2 = "abcde {\"llllz";
    std::string s3 = "abcde llllz";

    assert(UrlParser::instance().extractJson(s1) == "{\"a\",0}");
    assert(UrlParser::instance().extractJson(s2) == "{}");
    assert(UrlParser::instance().extractJson(s3) == "{}");
}

void test_extract_get_params() {
    std::string s1 = "/users/1/visits?toDate=234555&fromDate=1223445&toDistange=345";
    bool paramsValid;
    auto parVal = UrlParser::instance().extractGetParams(s1, paramsValid);

    for (auto pr : parVal) {
        std::cout << pr.first << " " << pr.second << std::endl;
    }
}

void test_urlparser() {
    test_extract_json();
    test_extract_get_params();
}
