#pragma once

#include <map>
#include "structures.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using Id = unsigned int;

using ConditionMap = std::map<std::string, std::string>;

//enum Struct {USER, LOCATION, VISIT};

class AbstractStorage {
public:
    virtual bool user(Id id, User& o) = 0;
    virtual bool location(Id id, Location& o) = 0;
    virtual bool visit(Id id, Visit& o) = 0;

    virtual json userVisits(Id id, ConditionMap conditions = {}) = 0;
    virtual double locationAvgRate(Id id, ConditionMap conditions = {}) = 0;

    virtual bool updateUser(Id id, json user) = 0;
    virtual bool updateLocation(Id id, json location) = 0;
    virtual bool updateVisit(Id id, json visit) = 0;
};


class JsonStorage : public AbstractStorage {
public:
    JsonStorage();

    bool user(Id id, User& o);
    bool location(Id id, Location& o);
    bool visit(Id id, Visit& o);

    json userVisits(Id id, ConditionMap conditions = {});
    double locationAvgRate(Id id, ConditionMap conditions = {});

    bool updateUser(Id id, json user);
    bool updateLocation(Id id, json location);
    bool updateVisit(Id id, json visit);


private:
    void populateStructFromFiles(json& _struct, std::string baseName);

    json::iterator findById(json& objects, Id id);

    template <typename T>
    bool getStruct(Id id, json& _list, T& o) {
        bool ret = false;
        auto it = findById(_list, id);
        if (it != _list.end()) {
            o = *it;
            ret = true;
        }
        return ret;
    }

    bool updateStruct(json& _struct, Id id, const json& newObj);

    json _users, _locations, _visits;
};


#ifdef SQLITE_ORM

#include "sqlite_orm/sqlite_orm.h"
class SqliteOrmStorage : public AbstractStorage {
    bool user(Id id, User& o);
    bool location(Id id, Location& o);
    bool visit(Id id, Visit& o);
    SqliteOrmStorage();
};

#endif
