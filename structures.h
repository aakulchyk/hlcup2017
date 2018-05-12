#pragma once

#include <string>
#include <ctime>
#include <string>


#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct User {
    unsigned int id = 0;
    std::string email;
    std::string first_name, last_name;
    std::string gender;
    time_t birth_date;

    bool _isSet_ = false;
    User() {}
    User(json& o);

    static bool validate(json o);

    void from_json(json o);
    json to_json();
};

struct Location {
    unsigned int id = 0;
    std::string place;
    std::string country;
    std::string city;
    unsigned int distance;

    bool _isSet_ = false;

    Location() {}
    Location(json& o);

    static bool validate(json o);

    void from_json(json o);
    json to_json();
};

struct Visit {
    unsigned int id = 0;
    unsigned int location = 0;
    unsigned int user = 0;
    time_t visited_at = 0;
    unsigned int mark = 0;

    bool _isSet_ = false;

    Visit() {}
    Visit(json& o);

    static bool validate(json o);

    void from_json(json o);
    json to_json();
};
