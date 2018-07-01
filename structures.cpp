#include "structures.h"

#include <iostream>
#include <regex>
#include <algorithm>


bool User::validate(json o, bool needAllFields) {
    try {
        int dummy;
        int fields = 0;
        if (o.find("id") != o.end()) {
            dummy = o["id"];
            fields++;
        }
        if (o.find("email") != o.end()) {
            std::string s = o["email"];
            std::regex re("(\\w|\\.)+@\\w+\\.(\\w){1,3}");
            std::smatch m;
            if (false == regex_match(s, m, re))
                throw std::invalid_argument("email is incorrect");
            fields++;
        }
        if (o.find("first_name") != o.end()) {
            std::string s = o["first_name"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("first_name is incorrect");
            fields++;
        }
        if (o.find("last_name") != o.end()) {
            std::string s = o["last_name"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("last_name is incorrect");
            fields++;
        }
        if (o.find("gender") != o.end()) {
            std::string s = o["gender"];
            if (s.length() < 1 || s.length() > 2)
                throw std::invalid_argument("gender is incorrect");
            fields++;
        }
        if (o.find("birth_date") != o.end()) {
            dummy = o["birth_date"];
            fields++;
        }

        if (needAllFields && fields < _max_fields_)
            return false;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "User::validate: "  << e.what() << std::endl;
        return false;
    }
}

void from_json(const json& o, User& user) {
    // TODO: check json on correctness
    try {
        if (!User::validate(o))
            throw std::invalid_argument("validation failed.");

        if (o.find("id") != o.end())
            user.id = o["id"];

        if (o.find("email") != o.end())
            user.email = o["email"];
        if (o.find("first_name") != o.end())
            user.first_name = o["first_name"];
        if (o.find("last_name") != o.end())
            user.last_name = o["last_name"];
        if (o.find("gender") != o.end())
            user.gender = o["gender"];
        if (o.find("birth_date") != o.end())
            user.birth_date = o["birth_date"];
    } catch (std::exception& e) {
        std::cerr << "User::from_json "  << e.what() << std::endl;
    }
}

void to_json(json& o, const User& user) {
    try {
        o["id"] = user.id;
        o["email"] = user.email;
        o["first_name"] = user.first_name;
        o["last_name"] = user.last_name;
        o["gender"] = user.gender;
        o["birth_date"] = user.birth_date;
    } catch (std::exception& e) {
        std::cerr << "User::to_json "  << e.what() << std::endl;
    }
}

bool Location::validate(json o, bool needAllFields)
{
    try {
        long int dummy;
        int fields = 0;

        if (o.find("id") != o.end()) {
            dummy = o["id"];
            fields++;
        }
        if (o.find("place") != o.end()) {
            std::string s = o["email"];
            fields++;
        }
        if (o.find("country") != o.end()) {
            std::string s = o["country"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("country is incorrect");
            fields++;
        }
        if (o.find("city") != o.end()) {
            std::string s = o["city"];
            if (s.length() < 1 || s.length() > 50)
                throw std::invalid_argument("city is incorrect");
            fields++;
        }

        if (o.find("distance") != o.end()) {
            dummy = o["distance"];
            if (dummy < 0)
                throw std::invalid_argument("distance is invalid");
            fields++;
        }

        if (needAllFields && fields < _max_fields_)
            return false;

        return true;
    }
    catch (const std::exception& e) {
        //std::cerr << "Location::validate: "  << e.what() << std::endl;
        return false;
    }
}


void from_json(const json& o, Location& location) {
    try {
        // TODO: check json on correctness
        if (o.find("id") != o.end())
            location.id = o["id"];

        if (o.find("place") != o.end())
            location.place = o["place"];
        if (o.find("country") != o.end())
            location.country = o["country"];
        if (o.find("city") != o.end())
            location.city = o["city"];
        if (o.find("distance") != o.end())
            location.distance = o["distance"];
    } catch (std::exception& e) {
        std::cerr << "Location::from_json "  << e.what() << std::endl;
    }
}

void to_json(json& o, const Location& location) {
    try {
        o["id"] = location.id;
        o["place"] = location.place;
        o["country"] = location.country;
        o["city"] = location.city;
        o["distance"] = location.distance;
    } catch (std::exception& e) {
        std::cerr << "Location::to_json "  << e.what() << std::endl;
    }
}

bool Visit::validate(json o, bool needAllFields)
{
    try {
        long int dummy;
        int fields = 0;

        if (o.find("id") != o.end()) {
            dummy = o["id"];
            if (dummy < 0) throw std::invalid_argument("id is invalid");
            fields++;
        }

        if (o.find("location") != o.end()) {
            dummy = o["location"];
            if (dummy < 0) throw std::invalid_argument("location is invalid");
            fields++;
        }

        if (o.find("user") != o.end()) {
            dummy = o["user"];
            if (dummy < 0) throw std::invalid_argument("user is invalid");
            fields++;
        }
        if (o.find("visited_at") != o.end()) {
            dummy = o["visited_at"];
            if (dummy < 0) throw std::invalid_argument("visited_at is invalid");
            fields++;
        }

        if (o.find("mark") != o.end()) {
            dummy = o["mark"];
            if (dummy < 0) throw std::invalid_argument("mark is invalid");
            fields++;
        }

        if (needAllFields && fields < _max_fields_)
            return false;

        return true;
    }
    catch (const std::exception& e) {
        //std::cerr << "Visit::validate: "  << e.what() << std::endl;
        return false;
    }
}

void from_json(const json& o, Visit& visit) {
    try {
        if (o.find("id") != o.end())
            visit.id = o["id"];

        if (o.find("location") != o.end())
            visit.location = o["location"];
        if (o.find("user") != o.end())
            visit.user = o["user"];
        if (o.find("visited_at") != o.end())
            visit.visited_at = o["visited_at"];
        if (o.find("mark") != o.end())
            visit.mark = o["mark"];
    } catch (std::exception& e) {
        std::cerr << "Visit::from_json "  << e.what() << std::endl;
    }
}

void to_json(json& o, const Visit& visit) {
    try {
        o["id"] = visit.id;
        o["location"] = visit.location;
        o["user"] = visit.user;
        o["visited_at"] = visit.visited_at;
        o["mark"] = visit.mark;
    } catch (std::exception& e) {
        std::cerr << "Visit::to_json "  << e.what() << std::endl;
    }
}
