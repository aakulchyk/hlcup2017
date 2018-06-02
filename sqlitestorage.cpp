#include <fstream>
#include <string>

#include "storage.h"

using namespace sqlite;
using namespace std;
using json = nlohmann::json;

SqliteCppStorage::SqliteCppStorage()
    : db("db.sqlite")
{
    cout << "creating database...";
    // CREATE TABLES
    db <<
    "create table if not exists users ("
    "  id integer primary key not null,"
    "  email text,"
    "  first_name text,"
    "  last_name text,"
    "  gender text,"
    "  birth_date integer"
    ");";

    db <<
    "create table if not exists locations ("
    "  id integer primary key not null,"
    "  place text,"
    "  country text,"
    "  city text,"
    "  distance integer"
    ");";

    db <<
    "create table if not exists visits ("
    "  id integer primary key not null,"
    "  location integer,"
    "  user integer,"
    "  visited_at integer,"
    "  mark text"
    ");";

    // READ DATA FROM FILES
    unzipData();

    for (int i = 1;; i++) {
        std::stringstream namestream;
        namestream << data_path << "users" << "_" << i << ".json";

        std::ifstream is(namestream.str());

        std::cout << "open: " << namestream.str() << std::endl;
        if (is.fail()) {
            std::cerr << "Error: " << strerror(errno) << std::endl;
            break;
        }

        json data;
        is >> data;

        for (auto entry : data["users"]) {
            db <<
            "insert into users (id, email, first_name, last_name, gender, birth_date) "
            "values (?, ?, ?, ?, ?, ?);"
            << static_cast<int>(entry["id"])
            << entry["email"].get<string>()
            << entry["first_name"].get<string>()
            << entry["last_name"].get<string>()
            << entry["gender"].get<string>()
            << static_cast<int>(entry["birth_date"]);
        }

        is.close();
    }
}


bool SqliteCppStorage::user(Id id, User& o) {
    return true;
}

bool SqliteCppStorage::location(Id id, Location& o) {
    return true;
}
bool SqliteCppStorage::visit(Id id, Visit& o) {
    return true;
}

json SqliteCppStorage::userVisits(Id id, ConditionMap conditions) {
    return json();
}
double SqliteCppStorage::locationAvgRate(Id id, ConditionMap conditions){

}

bool SqliteCppStorage::updateUser(Id id, json user) {
    return true;
}
bool SqliteCppStorage::updateLocation(Id id, json location) {
    return true;
}
bool SqliteCppStorage::updateVisit(Id id, json visit) {
    return true;
}

bool SqliteCppStorage::createUser(json user) {
    return true;
}
bool SqliteCppStorage::createLocation(json location) {
    return true;
}
bool SqliteCppStorage::createVisit(json visit) {
    return true;
}
