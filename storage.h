#pragma once

#include <map>
#include <string>
#include "structures.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using Id = unsigned int;

using ConditionMap = std::map<std::string, std::string>;


const std::string zip_path = "/tmp/data/";
const std::string data_path = "./data/";


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

    virtual bool createUser(json user) = 0;
    virtual bool createLocation(json location) = 0;
    virtual bool createVisit(json visit) = 0;

    static void unzipData();
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

    bool createUser(json user);
    bool createLocation(json location);
    bool createVisit(json visit);

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

#include <sqlite_modern_cpp.h>

class SqliteCppStorage : public AbstractStorage {

public:
    SqliteCppStorage();

    bool user(Id id, User& o);
    bool location(Id id, Location& o);
    bool visit(Id id, Visit& o);

    virtual json userVisits(Id id, ConditionMap conditions = {});
    virtual double locationAvgRate(Id id, ConditionMap conditions = {});

    virtual bool updateUser(Id id, json user);
    virtual bool updateLocation(Id id, json location);
    virtual bool updateVisit(Id id, json visit);

    virtual bool createUser(json user);
    virtual bool createLocation(json location);
    virtual bool createVisit(json visit);

    bool tryQuery(std::string);

private:
    sqlite::database db;

    json populateStructFromFile(std::string baseName, int index);
};


#undef SQLITE_ORM
#ifdef SQLITE_ORM

#include "sqlite_orm/sqlite_orm.h"
class SqliteOrmStorage : public AbstractStorage {
    bool user(Id id, User& o);
    bool location(Id id, Location& o);
    bool visit(Id id, Visit& o);
    SqliteOrmStorage();
};



#endif
