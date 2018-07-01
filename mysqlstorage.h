#pragma once

#include "storage.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
class MysqlStorage : public AbstractStorage {
public:
    MysqlStorage();

    bool user(Id id, User& o) override;
    bool location(Id id, Location& o) override;
    bool visit(Id id, Visit& o) override;

    json userVisits(Id id, ConditionMap conditions = {}) override;
    double locationAvgRate(Id id, ConditionMap conditions = {}) override;

    bool updateUser(Id id, json user) override;
    bool updateLocation(Id id, json location) override;
    bool updateVisit(Id id, json visit) override;

    bool createUser(json user) override;
    bool createLocation(json location) override;
    bool createVisit(json visit) override;

private:
    //std::shared_ptr<sql::Driver> driver;
    sql::Driver *driver;
    std::unique_ptr<sql::Connection> con;
    //sql::Statement *stmt;
    //sql::ResultSet *res;
};