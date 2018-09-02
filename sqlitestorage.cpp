#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>

#include "storage.h"

using namespace sqlite;
using namespace std;

const double seconds_in_year = 31556926.0;

namespace Sqlite {
    mutex storage_mutex;  // protects storage
}

decltype(auto) start_measure(string msg) {
    cout << "start measuring: " << msg << endl;
    return chrono::system_clock::now();
}

void end_measure(auto start) {
    auto end = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Elapsed time: " << elapsed.count() << '\n';
}

SqliteCppStorage::SqliteCppStorage()
    : db("db.sqlite")
{
    try {

        cout << "(db";
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

        //auto start = start_measure("read data");

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

        cout << ")";
        //end_measure(start);
    }
    catch (sqlite_exception& e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
    }
    catch(exception& e) {
        throw e;
    }
}

json SqliteCppStorage::populateStructFromFile(string baseName, int index) {
    stringstream namestream;
    namestream << data_path << baseName << "_" << index << ".json";
    ifstream is(namestream.str());
    if (is.fail()) {
        cerr << "Error: " << strerror(errno) << endl;
        return json();
    }
    //cout << "read file: " << namestream.str() << endl;

    json data;
    is >> data;
    is.close();

    return data;
}


bool SqliteCppStorage::user(Id id, User& o) {
    try {
        //lock_guard<mutex> lock(Sqlite::storage_mutex);
        // id, email, first_name, last_name, gender, birth_date
        bool _found = false;
        db << "select * from users where id = ? ;" << id
             >> [&](int id, string email, string first_name, string last_name, string gender, int birth_date) {
                o.id = id;
                o.email = email;
                o.first_name = first_name;
                o.last_name = last_name;
                o.gender = gender;
                o.birth_date = birth_date;
                _found = true;
             };
        return _found;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}

bool SqliteCppStorage::location(Id id, Location& o) {
    try {
        //lock_guard<mutex> lock(Sqlite::storage_mutex);
        // id, place, country, city, distance
        bool _found = false;
        db << "select * from locations where id = ? ;" << id
             >> [&](int id, string place, string country, string city, int distance) {
                o.id = id;
                o.place = place;
                o.country = country;
                o.city = city;
                o.distance = distance;
                _found = true;
             };
        return _found;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}
bool SqliteCppStorage::visit(Id id, Visit& o) {
    try {
        //lock_guard<mutex> lock(Sqlite::storage_mutex);
        // id, location, user, visited_at, mark
        bool _found = false;
        db << "select * from visits where id = ? ;" << id
             >> [&](int id, int location, int user, int visited_at, int mark) {
                o.id = id;
                o.location = location;
                o.user = user;
                o.visited_at = visited_at;
                o.mark = mark;
                _found = true;
             };
             
        return _found;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}

json SqliteCppStorage::userVisits(Id id, ConditionMap conditions) {
    json visits = json::array();
    try {
        //lock_guard<mutex> lock(Sqlite::storage_mutex);
        // id, location, user, visited_at, mark
        stringstream queryStream;

        queryStream << "select v.mark, v.visited_at, l.place from visits v";
        queryStream << " inner join locations l on l.id = v.location";
        queryStream << " where v.user = " << id;

        if (conditions.find("country") != conditions.end()) {
            queryStream << " and l.country = '" << conditions["country"] << "'";
        }

        if (conditions.find("fromDate") != conditions.end()) {
            queryStream << " and v.visited_at > " << conditions["fromDate"];
        }

        if (conditions.find("toDate") != conditions.end()) {
            queryStream << " and v.visited_at < " << conditions["toDate"];
        }

        if (conditions.find("toDistance") != conditions.end()) {
            queryStream << " and l.distance < " << conditions["toDistance"];
        }

        queryStream << " order by v.visited_at";

        string query = queryStream.str();
        //cout << "Query: " << queryStream.str();

        db << query >> [&visits](int mark, int visited_at, string place) {
                json visit = {  {"mark", mark}, {"visited_at", visited_at}, {"place", place} };
                visits.push_back(visit);
             };
        //cout << "uv ok.";
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " /*<< e.what() << " during "*/
         << e.get_sql() << endl;
    }
    catch(std::exception& e) {
        cerr << "avg : " << e.what();
    }
    return visits;
}

double SqliteCppStorage::locationAvgRate(Id id, ConditionMap conditions){
    try {
        //lock_guard<mutex> lock(Sqlite::storage_mutex);
        stringstream queryStream;

        queryStream << "select avg(v.mark) from visits v";
        queryStream << " inner join users u on u.id = v.user";

        queryStream << " where v.location = " << id;

        if (conditions.find("fromDate") != conditions.end()) {
            queryStream << " and v.visited_at > " << conditions["fromDate"];
        }

        if (conditions.find("toDate") != conditions.end()) {
            queryStream << " and v.visited_at < " << conditions["toDate"];
        }

        if (conditions.find("fromAge") != conditions.end()) {
            int fromAge_sec = std::stoi(conditions["fromAge"]) * seconds_in_year;
            int fromDate_sec = std::difftime(std::time(0), fromAge_sec);
            queryStream << " and u.birth_date < " << fromDate_sec;
        }

        if (conditions.find("toAge") != conditions.end()) {
            int toAge_sec = std::stoi(conditions["toAge"]) * seconds_in_year;
            int toDate_sec = std::difftime(std::time(0), toAge_sec);
            queryStream << " and u.birth_date > " << toDate_sec;
        }

        if (conditions.find("gender") != conditions.end()) {
            queryStream << " and u.gender = '" << conditions["gender"] << "'";
        }

        string query = queryStream.str();
        //cout << "Query: " << query << endl;

        double avg = 0.;
        db << query >> [&avg](double result) {
                avg = result;
             };
        //cout << "avg ok.";
        return avg;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " /*<< e.what() << " during "*/
         << e.get_sql() << endl;
         return 0.;
    }
    catch(std::exception& e) {
        cerr << "avg : " << e.what();
        return 0;
    }
}

bool SqliteCppStorage::updateUser(Id id, json user) {
    try {
        stringstream queryStream;
        if (user.empty())
            return false;

        User u;
        if (!this->user(id, u))
            return false;

        //cout << "user dump: " << user.dump() << endl;
        queryStream << "update users set ";
        for (auto it = user.begin(); it != user.end(); ++it) {
            if (it != user.begin())
                queryStream << ", ";
            queryStream << it.key() << "=" << it.value();
        }
        queryStream << " where id = ?;";

        string query = queryStream.str();
        //cout << "Query : " << queryStream.str() << endl;

        lock_guard<mutex> lock(Sqlite::storage_mutex);
        db << query << id;
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}
bool SqliteCppStorage::updateLocation(Id id, json location) {
    try {
        stringstream queryStream;
        if (location.empty())
            return false;

        Location l;
        if (!this->location(id, l))
            return false;

        //cout << "user dump: " << user.dump() << endl;
        queryStream << "update locations set ";
        for (auto it = location.begin(); it != location.end(); ++it) {
            if (it != location.begin())
                queryStream << ", ";
            queryStream << it.key() << "=" << it.value();
        }
        queryStream << " where id = ?;";

        string query = queryStream.str();
        //cout << "Query : " << queryStream.str() << endl;
        lock_guard<mutex> lock(Sqlite::storage_mutex);
        db << query << id;
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}
bool SqliteCppStorage::updateVisit(Id id, json visit) {
    try {
        stringstream queryStream;
        if (visit.empty())
            return false;

        Visit v;
        if (!this->visit(id, v))
            return false;

        //cout << "user dump: " << user.dump() << endl;
        queryStream << "update visits set ";
        for (auto it = visit.begin(); it != visit.end(); ++it) {
            if (it != visit.begin())
                queryStream << ", ";
            queryStream << it.key() << "=" << it.value();
        }
        queryStream << " where id = ?;";

        string query = queryStream.str();
        //cout << "Query : " << queryStream.str() << endl;
        lock_guard<mutex> lock(Sqlite::storage_mutex);
        db << query << id;
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}

bool SqliteCppStorage::createUser(json user) {
    if (!User::validate(user, true))
        return false;

    try {
        Id id = user["id"];
        bool exists;

        db << "select count(*) from users where id = ?" << id
        >> [&exists](int count) { exists = count > 0; };
        if (exists) return false;

        lock_guard<mutex> lock(Sqlite::storage_mutex);
        auto prepared_statement =
            db <<
            "insert into users (id, email, first_name, last_name, gender, birth_date) "
            "values (?, ?, ?, ?, ?, ?);";

        prepared_statement
            << static_cast<int>(user["id"])
            << user["email"].get<string>()
            << user["first_name"].get<string>()
            << user["last_name"].get<string>()
            << user["gender"].get<string>()
            << static_cast<int>(user["birth_date"]);
        prepared_statement.execute();
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
        return false;
    }
}

bool SqliteCppStorage::createLocation(json location) {
    if (!Location::validate(location, true))
        return false;

    try {
        Id id = location["id"];
        bool exists;
        db << "select count(*) from locations where id = ?" << id
        >> [&exists](int count) { exists = count > 0; };
        if (exists) return false;

        lock_guard<mutex> lock(Sqlite::storage_mutex);
        auto prepared_statement =
            db <<
            "insert into locations (id, place, country, city, distance) "
            "values (?, ?, ?, ?, ?);";

        prepared_statement
            << static_cast<int>(location["id"])
            << location["place"].get<string>()
            << location["country"].get<string>()
            << location["city"].get<string>()
            << static_cast<int>(location["distance"]);
        prepared_statement.execute();
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
        return false;
    }
}

bool SqliteCppStorage::createVisit(json visit) {
    if (!Visit::validate(visit, true))
        return false;

    try {
        Id id = visit["id"];
        bool exists;
        db << "select count(*) from visits where id = ?" << id
        >> [&exists](int count) { exists = count > 0; };
        if (exists) return false;

        lock_guard<mutex> lock(Sqlite::storage_mutex);
        auto prepared_statement =
            db <<
            "insert into visits (id, location, user, visited_at, mark) "
            "values (?, ?, ?, ?, ?);";

        prepared_statement
            << static_cast<int>(visit["id"])
            << static_cast<int>(visit["location"])
            << static_cast<int>(visit["user"])
            << static_cast<int>(visit["visited_at"])
            << static_cast<int>(visit["mark"]);
        prepared_statement.execute();
        return true;
    }
    catch (sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
        return false;
    }
}

bool SqliteCppStorage::tryQuery(string query) {
    try {
        db << query;
        return true;
    }
    catch(sqlite_exception &e) {
        cerr  << e.get_code() << ": " << e.what() << " during "
         << e.get_sql() << endl;
         return false;
    }
}
