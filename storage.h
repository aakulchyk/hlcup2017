#pragma once

#include "structures.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using Id = unsigned int;

//enum Struct {USER, LOCATION, VISIT};

class AbstractStorage {
public:
    virtual User user(Id id) = 0;
    virtual Location location(Id id) = 0;
    virtual Visit visit(Id id) = 0;

    virtual json userVisits(Id id) = 0;
    virtual double locationAvgRate(Id id) = 0;

    virtual bool updateUser(Id id, json user) = 0;
    virtual bool updateLocation(Id id, json location) = 0;
    virtual bool updateVisit(Id id, json visit) = 0;
};


class JsonStorage : public AbstractStorage {
public:
    JsonStorage();

    User user(Id id);
    Location location(Id id);
    Visit visit(Id id);

    json userVisits(Id id);
    double locationAvgRate(Id id);

    bool updateUser(Id id, json user);
    bool updateLocation(Id id, json location);
    bool updateVisit(Id id, json visit);


private:
    json& findById(json objects, Id id);
    bool updateStruct(json& _struct, Id id, const json& newObj);

    json _users, _locations, _visits;
};


#ifdef SQLITE_ORM

#include "sqlite_orm/sqlite_orm.h"
class SqliteOrmStorage : public AbstractStorage {
    User user(Id id);
    Location location(Id id);
    Visit visit(Id id);
    SqliteOrmStorage();
};

#endif
