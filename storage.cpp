#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>
#include "storage.h"

std::mutex storage_mutex;  // protects storage


void JsonStorage::populateStructFromFiles(json& _struct, std::string baseName, int number)
{

    for (int i = 1; i <= number; i++) {
        std::stringstream namestream;
        namestream << "data/" << baseName << "_" << i << ".json";
        std::cout << "open: " << namestream.str() << std::endl;

        std::ifstream is(namestream.str());

        json data;
        is >> data;

        for (auto entry : data[baseName]) {
            //std::cout << "entry: " << entry << std::endl;
            _struct.push_back(entry);
        }
        is.close();
    }

    std::cout << "read " << baseName << ", " << _struct.size() << " records" << std::endl;
}


JsonStorage::JsonStorage()
{
    populateStructFromFiles(_users, "users", 11);

    populateStructFromFiles(_locations, "locations", 8);

    populateStructFromFiles(_visits, "visits", 11);
}


bool JsonStorage::user(Id id, User& o) {
    return getStruct<User>(id, _users, o);
}

bool JsonStorage::location(Id id, Location& o) {
    return getStruct<Location>(id, _locations, o);
}

bool JsonStorage::visit(Id id, Visit& o) {
    return getStruct<Visit>(id, _visits, o);
}

json JsonStorage::userVisits(Id id, ConditionMap conditions)
{
    json visits;
    for (auto v : _visits) {
        if (v.empty())
            continue;
        if (v["user"] != id)
            continue;

        if (conditions.find("fromDate") != conditions.end()) {
            time_t fromDate = static_cast<time_t>(std::stoi(conditions["fromDate"]));
            if (static_cast<time_t>(v["visited_at"]) <= fromDate)
                continue;
        }

        if (conditions.find("toDate") != conditions.end()) {
            time_t fromDate = static_cast<time_t>(std::stoi(conditions["toDate"]));
            if (static_cast<time_t>(v["visited_at"]) >= fromDate)
                continue;
        }

        Location visitLocation;
        int locationId = v["location"];
        if (false == location(locationId, visitLocation))
            continue;

        if (conditions.find("country") != conditions.end()) {
            std::string visitCountry  = conditions["country"];
            if (visitCountry != visitLocation.country)
                continue;
        }

        if (conditions.find("toDistance") != conditions.end()) {
            long toDistance = std::stoi(conditions["toDistance"]);
            if (toDistance >= visitLocation.distance)
                continue;
        }

        visits.push_back(v);
    }

    return visits;
}

double JsonStorage::locationAvgRate(Id id, ConditionMap conditions)
{
    double rate = 0;
    int count = 0;
    for (auto v : _visits) {
        if (v.empty()) continue;
        if (v["location"] != id)
            continue;


        if (conditions.find("fromDate") != conditions.end()) {
            time_t fromDate = static_cast<time_t>(std::stoi(conditions["fromDate"]));
            if (static_cast<time_t>(v["visited_at"]) <= fromDate)
                continue;
        }

        if (conditions.find("toDate") != conditions.end()) {
            time_t fromDate = static_cast<time_t>(std::stoi(conditions["toDate"]));
            if (static_cast<time_t>(v["visited_at"]) >= fromDate)
                continue;
        }

        User visitUser;
        int userId = v["user"];
        if  (false == user(userId, visitUser))
            continue;

        if (conditions.find("fromAge") != conditions.end()) {
            int fromAge = std::stoi(conditions["fromAge"]);
            int userAge = std::time(0) - visitUser.birth_date;

            if (userAge <= fromAge)
                continue;
        }

        if (conditions.find("toAge") != conditions.end()) {
            int toAge = std::stoi(conditions["toAge"]);
            int userAge = std::time(0) - visitUser.birth_date;

            if (userAge >= toAge)
                continue;
        }

        if (conditions.find("gender") != conditions.end()) {
            std::string visitGender = conditions["gender"];
            if (visitGender != visitUser.gender)
                continue;
        }

        rate += static_cast<double>(v["mark"]);
        count++;

    }

    rate /= count;

    return rate;
}

bool JsonStorage::updateUser(Id id, json uuser)
{
    std::lock_guard<std::mutex> lock(storage_mutex);
    auto it = findById(_users, id);
    return it != _users.end() ? updateStruct(*it, id, uuser) : false;
}

bool JsonStorage::updateLocation(Id id, json ulocation)
{
    std::lock_guard<std::mutex> lock(storage_mutex);
    auto it = findById(_locations, id);
    return it != _locations.end() ? updateStruct(*it, id, ulocation) : false;
}

bool JsonStorage::updateVisit(Id id, json uvisit)
{
    std::lock_guard<std::mutex> lock(storage_mutex);
    auto it = findById(_visits, id);
    return it != _visits.end() ? updateStruct(*it, id, uvisit) : false;
}

json::iterator JsonStorage::findById(json& objects, Id id) {
    return std::find_if(objects.begin(), objects.end(), [id](const json& o) { return o["id"] == id; });;
}

bool JsonStorage::updateStruct(json &_struct, Id id, const json& newObj)
{
    if (_struct.empty() || newObj.empty())
        return false;

    for (auto it = newObj.begin(); it != newObj.end(); ++it) {
        _struct[it.key()] = newObj[it.key()];
    }

    return true;
}


// ======================================
#ifdef SQLITE_ORM

using namespace sqlite_orm;

User SqliteOrmStorage::user(Id id)
{

}

Location SqliteOrmStorage::location(Id id)
{

}

Visit SqliteOrmStorage::visit(Id id)
{

}

SqliteOrmStorage::SqliteOrmStorage()
{
    // create schema
    auto storage = make_storage("db.sqlite",
        make_table("USERS",
            make_column("ID", &User::id, primary_key()),
            make_column("EMAIL", &User::email),
            make_column("FIRST_NAME", &User::first_name),
            make_column("LAST_NAME", &User::last_name),
            make_column("GENDER", &User::gender),
            make_column("BIRTH_DATE", &User::birth_date)),
        make_table("LOCATIONS",
            make_column("ID", &Location::id, primary_key()),
            make_column("PLACE", &Location::place),
            make_column("COUNTRY", &Location::country),
            make_column("CITY", &Location::city),
            make_column("DISTANCE", &Location::distance)),
        make_table("VISITS",
            make_column("ID", &Visit::id, primary_key()),
            make_column("LOCATION", &Visit::location),
            make_column("USER", &Visit::user),
            make_column("VISITED_AT", &Visit::visited_at),
            make_column("MARK", &Visit::mark)));

    // read from files
    json data;
    std::ifstream i("data/users_1.json");
    i >> data;
    i.close();

    i.open("data/locations_1.json");
    i.close();


    i.open("data/visits_1.json");
    i.close();

}

#endif
