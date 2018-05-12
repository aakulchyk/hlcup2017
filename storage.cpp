#include <algorithm>
#include <fstream>
#include <iostream>

#include "storage.h"

JsonStorage::JsonStorage()
{
    json data;
    std::ifstream i("data/users_1.json");
    i >> data;
    _users = data["users"];
    std::cout << "read users, " << _users.size() << " records" << std::endl;

    i.close();
    i.open("data/locations_1.json");
    i >> data;
    _locations = data["locations"];
    std::cout << "read locations, " << _locations.size() << " records" << std::endl;

    i.close();
    i.open("data/visits_1.json");
    i >> data;
    _visits = data["visits"];
    std::cout << "read visits, " << _visits.size() << " records" << std::endl;
}


User JsonStorage::user(Id id) {
    auto o = findById(_users, id);
    return User(o);
}

Location JsonStorage::location(Id id) {
    auto o = findById(_locations, id);
    return Location(o);
}

Visit JsonStorage::visit(Id id) {
    auto o = findById(_visits, id);
    return Visit(o);
}

json JsonStorage::userVisits(Id id)
{
    json visits;
    for (auto v : _visits) {
        if (v.empty()) continue;
        if (v["user"] == id)
            visits.push_back(v);
    }

    return visits;
}

double JsonStorage::locationAvgRate(Id id)
{
    double rate;
    int count = 0;
    for (auto v : _visits) {
        if (v.empty()) continue;
        if (v["location"] == id) {
            rate += static_cast<double>(v["mark"]);
            count++;
        }
    }

    rate /= count;

    return rate;
}

bool JsonStorage::updateUser(Id id, json uuser)
{
    auto user = findById(_users, id);
    return updateStruct(user, id, uuser);
}

bool JsonStorage::updateLocation(Id id, json ulocation)
{
    auto location = findById(_locations, id);
    return updateStruct(location, id, ulocation);
}

bool JsonStorage::updateVisit(Id id, json uvisit)
{
    auto visit = findById(_visits, id);
    return updateStruct(visit, id, uvisit);
}

json& JsonStorage::findById(json objects, Id id) {
    auto it = std::find_if(objects.begin(), objects.end(), [id](const json& o) { return o["id"] == id; });
    return it != objects.end() ? *it : json();
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
