#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "storage.h"

using namespace sqlite;
using namespace std;

decltype(auto) start_measure(string msg) {
    cout << "start measuring: " << msg << endl;
    return std::chrono::system_clock::now();
}

void end_measure(auto start) {
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed.count() << '\n';
}

SqliteCppStorage::SqliteCppStorage()
    : db("db.sqlite")
{
    try {

        cout << "creating database...";
        // CREATE TABLES
        db << "drop table if exists users";
        db << "drop table if exists locations";
        db << "drop table if exists visits";

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

        auto start = start_measure("read data");

        for (int i = 1;; i++) {
            json data = populateStructFromFile("users", i);
            if (data.empty())
                break;

            db << "begin;";

            auto prepared_statement =
                db <<
                "insert into users (id, email, first_name, last_name, gender, birth_date) "
                "values (?, ?, ?, ?, ?, ?);";

            for (auto entry : data["users"]) {
                prepared_statement
                    << static_cast<int>(entry["id"])
                    << entry["email"].get<string>()
                    << entry["first_name"].get<string>()
                    << entry["last_name"].get<string>()
                    << entry["gender"].get<string>()
                    << static_cast<int>(entry["birth_date"]);
                prepared_statement.execute();
            }

            db << "commit;";
        }

        for (int i = 1;; i++) {
            json data = populateStructFromFile("locations", i);
            if (data.empty())
                break;

            db << "begin;";

            auto prepared_statement =
                db <<
                "insert into locations (id, place, country, city, distance) "
                "values (?, ?, ?, ?, ?);";

            for (auto entry : data["locations"]) {
                prepared_statement
                    << static_cast<int>(entry["id"])
                    << entry["place"].get<string>()
                    << entry["country"].get<string>()
                    << entry["city"].get<string>()
                    << static_cast<int>(entry["distance"]);
                prepared_statement.execute();
            }

            db << "commit;";
        }

        for (int i = 1;; i++) {
            json data = populateStructFromFile("visits", i);
            if (data.empty())
                break;

            db << "begin;";
            auto prepared_statement =
                db <<
                "insert into visits (id, location, user, visited_at, mark) "
                "values (?, ?, ?, ?, ?);";

            for (auto entry : data["visits"]) {
                prepared_statement
                    << static_cast<int>(entry["id"])
                    << static_cast<int>(entry["location"])
                    << static_cast<int>(entry["user"])
                    << static_cast<int>(entry["visited_at"])
                    << static_cast<int>(entry["mark"]);
                prepared_statement.execute();
            }
            db << "commit;";
        }

        end_measure(start);
    }
    catch (sqlite_exception& e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
    }
    catch(exception& e) {
        throw e;
    }
}

json SqliteCppStorage::populateStructFromFile(std::string baseName, int index) {
    std::stringstream namestream;
    namestream << data_path << baseName << "_" << index << ".json";
    std::ifstream is(namestream.str());
    if (is.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return json();
    }

    std::cout << "read file: " << namestream.str() << std::endl;

    json data;
    is >> data;
    is.close();

    return data;
    /*auto baseEntry = data[baseName][0];

    vector keys;
    string valuesString = "(";
    for (auto it = baseEntry.begin(); it != baseEntry.end(); ++it) {
        keys.push_back(it.key())
    }*/
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
