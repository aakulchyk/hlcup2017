#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>

#include <stdio.h>
#include "storage.h"

std::mutex storage_mutex;  // protects storage

const std::string zip_path = "/tmp/data/";
const std::string data_path = "./data/";

void unzipData() {
    std::stringstream commandstream;
    commandstream << "unzip -u " << zip_path << "data.zip -d " << data_path;
    system(commandstream.str().c_str());
}

void JsonStorage::populateStructFromFiles(json& _struct, std::string baseName)
{

    for (int i = 1;; i++) {
        std::stringstream namestream;
        namestream << data_path << baseName << "_" << i << ".json";

        std::ifstream is(namestream.str());

        std::cout << "open: " << namestream.str() << std::endl;
        if (is.fail()) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            //std::cout << "failed opening file. " << std::endl;
            break;
        }

        json data;
        is >> data;

        for (auto entry : data[baseName]) {
            _struct.push_back(entry);
        }
        is.close();
    }

    std::cout << "read " << baseName << ", " << _struct.size() << " records" << std::endl;
}


JsonStorage::JsonStorage()
{
    unzipData();

    populateStructFromFiles(_users, "users");
    populateStructFromFiles(_locations, "locations");
    populateStructFromFiles(_visits, "visits");
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

        json userVisit;
        userVisit["mark"] = v["mark"];
        userVisit["place"] = visitLocation.place;
        userVisit["visited_at"] = v["visited_at"];

        visits.push_back(userVisit);
    }

    return visits;
}


const double seconds_in_year = 31556926.0;

double JsonStorage::locationAvgRate(Id id, ConditionMap conditions)
{
    double rate = 0.0;
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
            int userAge = std::difftime(std::time(0), visitUser.birth_date) / seconds_in_year;

            if (userAge <= fromAge)
                continue;
        }

        if (conditions.find("toAge") != conditions.end()) {
            int toAge = std::stoi(conditions["toAge"]);
            int userAge = std::difftime(std::time(0), visitUser.birth_date) / seconds_in_year;

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

    return count > 0 ? rate / count  :  0.0;
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

bool JsonStorage::createUser(json user)
{
    if (!User::validate(user, true))
        return false;
    Id id = user["id"];
    if (findById(_users, id) != _users.end())
        return false;

    std::lock_guard<std::mutex> lock(storage_mutex);
    _users.push_back(user);
    return true;
}

bool JsonStorage::createLocation(json location)
{
    if (!Location::validate(location, true))
        return false;
    Id id = location["id"];
    if (findById(_locations, id) != _locations.end())
        return false;

    std::lock_guard<std::mutex> lock(storage_mutex);
    _locations.push_back(location);
    return true;
}

bool JsonStorage::createVisit(json visit)
{
    if (!Visit::validate(visit, true))
        return false;
    Id id = visit["id"];
    if (findById(_visits, id) != _visits.end())
        return false;

    std::lock_guard<std::mutex> lock(storage_mutex);
    _visits.push_back(visit);
    return true;
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
