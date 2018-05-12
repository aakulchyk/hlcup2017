#include "structures.h"
#include <regex>

User::User(json &o) {
    if (!o.empty()) {
        from_json(o);
        _isSet_ = true;
    }
}

bool User::validate(json o) {
    try {
        int dummy;
        if (o.find("id") != o.end()) {
            dummy = o["id"];
        }
        if (o.find("email") != o.end()) {
            std::string s = o["email"];
            std::regex re("(\\w|\\.)+@\\w+\\.(\\w){1,3}");
            std::smatch m;
            if (false == regex_match(s, m, re))
                throw std::invalid_argument("email is incorrect");
        }
        if (o.find("first_name") != o.end()) {
            std::string s = o["first_name"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("first_name is incorrect");
        }
        if (o.find("last_name") != o.end()) {
            std::string s = o["last_name"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("last_name is incorrect");
        }
        if (o.find("gender") != o.end()) {
            std::string s = o["gender"];
            if (s.length() < 1 || s.length() > 2)
                throw std::invalid_argument("gender is incorrect");
        }
        if (o.find("birth_date") != o.end()) {
            dummy = o["birth_date"];
        }

    }
    catch (.../*const std::invalid_argument& ia*/) {
        std::cerr << "User::validate "  << e.what() << std::endl;
        return false;
    }
}

void User::from_json(json o) {
    // TODO: check json on correctness
    try {
        if (!User::validate(o))
            throw std::invalid_argument("validation failed.");

        if (o.find("id") != o.end())
            id = o["id"];

        if (o.find("email") != o.end())
            email = o["email"];
        if (o.find("first_name") != o.end())
            first_name = o["first_name"];
        if (o.find("last_name") != o.end())
            last_name = o["last_name"];
        if (o.find("gender") != o.end())
            gender = o["gender"];
        if (o.find("birth_date") != o.end())
            birth_date = o["birth_date"];
    } catch (std::exception& e) {
        std::cerr << "User::from_json "  << e.what() << std::endl;
    }
}

json User::to_json() {
    try {
        json o;
        o["id"] = id;
        o["email"] = email;
        o["first_name"] = first_name;
        o["last_name"] = last_name;
        o["gender"] = gender;
        o["birth_date"] = birth_date;

        return o;
    } catch (std::exception& e) {
        std::cerr << "User::to_json "  << e.what() << std::endl;
    }
}

Location::Location(json &o) {
    if (!o.empty()) {
        from_json(o);
        _isSet_ = true;
    }
}

bool Location::validate(json o)
{
    return true;
}

void Location::from_json(json o) {
    try {
        // TODO: check json on correctness
        if (o.find("id") != o.end())
            id = o["id"];

        if (o.find("place") != o.end())
            place = o["place"];
        if (o.find("country") != o.end())
            country = o["country"];
        if (o.find("city") != o.end())
            city = o["city"];
        if (o.find("distance") != o.end())
            distance = o["distance"];
    } catch (std::exception& e) {
        std::cerr << "Location::from_json "  << e.what() << std::endl;
    }
}

json Location::to_json() {
    try {
        json o;
        o["id"] = id;
        o["place"] = place;
        o["country"] = country;
        o["city"] = city;
        o["distance"] = distance;

        return o;
    } catch (std::exception& e) {
        std::cerr << "Location::to_json "  << e.what() << std::endl;
    }
}

Visit::Visit(json &o) {
    if (!o.empty()) {
        from_json(o);
        _isSet_ = true;
    }
}

bool Visit::validate(json o)
{
    return true;
}

void Visit::from_json(json o) {
    // TODO: check json on correctness
    try {
        if (o.find("id") != o.end())
            id = o["id"];

        if (o.find("location") != o.end())
            location = o["location"];
        if (o.find("user") != o.end())
            user = o["user"];
        if (o.find("visited_at") != o.end())
            visited_at = o["visited_at"];
        if (o.find("mark") != o.end())
            mark = o["mark"];
    } catch (std::exception& e) {
        std::cerr << "Visit::from_json "  << e.what() << std::endl;
    }
}

json Visit::to_json() {
    try {
        json o;
        o["id"] = id;
        o["location"] = location;
        o["user"] = user;
        o["visited_at"] = visited_at;
        o["mark"] = mark;

        return o;
    } catch (std::exception& e) {
        std::cerr << "Visit::to_json "  << e.what() << std::endl;
    }
}
