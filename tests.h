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


#include "storage.h"

void test_sqlite_storage(AbstractStorage* s) {
    SqliteCppStorage *sqlite = static_cast<SqliteCppStorage *>(s);

    std::string query;

    query = "select avg(v.mark) from visits v where v.location = 7161";
    assert(true == sqlite->tryQuery(query));
    query = "select avg(v.mark) from visits v inner join users u on u.id = v.user where v.location = 7161 and v.visited_at < 1156032000";
    assert(true == sqlite->tryQuery(query));
    query = "select avg(v.mark) from visits v inner join users u on u.id = v.user where v.location = 7161 and u.gender = 'f'";
    assert(true == sqlite->tryQuery(query));
    query = "select avg(v.mark) from visits v inner join users u on u.id = v.user where v.location = 7309 and u.birth_date > 1126000000";
    assert(true == sqlite->tryQuery(query));
    query = "select avg(v.mark) from visits v inner join users u on u.id = v.user where v.location = 7161 and u.birth_date < 1156032000";
    assert(true == sqlite->tryQuery(query));



    query = "update locations set country = 'Аргентина', place = 'Пруд' where id = 1739";
    assert(true == sqlite->tryQuery(query));

    //query = "select avg(v.mark) from visits v inner join users u on u.id = v.user where v.location = 13252 and v.visited_at > 1429833600 and age < 54";
    //assert(false == sqlite->tryQuery(query));
}
