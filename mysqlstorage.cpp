#include "mysqlstorage.h"


MysqlStorage::MysqlStorage()
: driver(get_driver_instance())
, con(driver->connect("tcp://127.0.0.1:3306", "mysql", "")) 
{
}

bool MysqlStorage::user(Id id, User& o) {
    return true;
}
bool MysqlStorage::location(Id id, Location& o) {
    return true;
}
bool MysqlStorage::visit(Id id, Visit& o) {
    return true;
}

json MysqlStorage::userVisits(Id id, ConditionMap conditions) {
    json visits = json::array();

    return visits;
}
double MysqlStorage::locationAvgRate(Id id, ConditionMap conditions) {
    return 0.0;
}

bool MysqlStorage::updateUser(Id id, json user) {
    return true;
}
bool MysqlStorage::updateLocation(Id id, json location) {
    return true;
}

bool MysqlStorage::updateVisit(Id id, json visit) {
    return true;
}

bool MysqlStorage::createUser(json user) {
    return true;
}
bool MysqlStorage::createLocation(json location) {
    return true;
}
bool MysqlStorage::createVisit(json visit) {
    return true;
}