#include "mysqlstorage.h"
#include <fstream>
#include <string>

const double seconds_in_year = 31556926.0;

MysqlStorage::MysqlStorage()
: driver(get_driver_instance())
, con(driver->connect("tcp://127.0.0.1:3306", "root", "")) 
{
    con->setSchema("hlcup2017");

    createTables();

    unzipData();

    populateUsersTable();
    populateLocationsTable();
    populateVisitsTable();
}

MysqlStorage::~MysqlStorage() {
}

void MysqlStorage::createTables() {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("drop table if exists users");
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("drop table if exists locations");
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("drop table if exists visits");
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery(
        "create table if not exists users ("
        "  id integer primary key not null,"
        "  email text,"
        "  first_name text,"
        "  last_name text,"
        "  gender text,"
        "  birth_date integer"
        ");"
        );
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }


    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery(
        "create table if not exists locations ("
        "  id integer primary key not null,"
        "  place text,"
        "  country text,"
        "  city text,"
        "  distance integer"
        ");"
        );
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery(
        "create table if not exists visits ("
        "  id integer primary key not null,"
        "  location integer,"
        "  user integer,"
        "  visited_at integer,"
        "  mark text"
        ");"
        );
    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
}

json MysqlStorage::populateStructFromFile(std::string baseName, int index) {
    std::stringstream namestream;
    namestream << data_path << baseName << "_" << index << ".json";
    std::ifstream is(namestream.str());
    if (is.fail()) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        return json();
    }
    //cout << "read file: " << namestream.str() << endl;

    json data;
    is >> data;
    is.close();

    return data;
}

void MysqlStorage::populateUsersTable() {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("begin;");
    } catch(...) {}
    for (int i = 1;; i++) {
        json data = populateStructFromFile("users", i);
        if (data.empty())
            break;
        
        std::unique_ptr< sql::PreparedStatement >  pstmt;
        pstmt.reset(con->prepareStatement(
            "insert into users (id, email, first_name, last_name, gender, birth_date) "
            "values (?, ?, ?, ?, ?, ?);"));
        for (auto entry : data["users"]) {
            int id = static_cast<int>(entry["id"]);
            pstmt->setString(1, std::to_string(id));
            pstmt->setString(2,entry["email"].get<std::string>());
            pstmt->setString(3,entry["first_name"].get<std::string>());
            pstmt->setString(4,entry["last_name"].get<std::string>());
            pstmt->setString(5,entry["gender"].get<std::string>());
            int birth_date = static_cast<int>(entry["birth_date"]);
            pstmt->setString(6,std::to_string(birth_date));
            pstmt->execute();
        }
    }
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("commit;");
    } catch(...) {}
}
void MysqlStorage::populateLocationsTable() {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("begin;");
    } catch(...) {}
    for (int i = 1;; i++) {
        json data = populateStructFromFile("locations", i);
        if (data.empty())
            break;
        
        std::unique_ptr< sql::PreparedStatement >  pstmt;
        pstmt.reset(con->prepareStatement(
            "insert into locations (id, place, country, city, distance) "
            "values (?, ?, ?, ?, ?);"));
        for (auto entry : data["locations"]) {
            int id = static_cast<int>(entry["id"]);
            pstmt->setString(1, std::to_string(id));
            pstmt->setString(2,entry["place"].get<std::string>());
            pstmt->setString(3,entry["country"].get<std::string>());
            pstmt->setString(4,entry["city"].get<std::string>());
            int distance = static_cast<int>(entry["distance"]);
            pstmt->setString(5,std::to_string(distance));
            pstmt->execute();
        }
    }
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("commit;");
    } catch(...) {}
}

void MysqlStorage::populateVisitsTable() {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("begin;");
    } catch(...) {}

    for (int i = 1;; i++) {
        json data = populateStructFromFile("visits", i);
        if (data.empty())
            break;
        
        std::unique_ptr< sql::PreparedStatement >  pstmt;
        pstmt.reset(con->prepareStatement(
            "insert into visits (id, location, user, visited_at, mark) "
            "values (?, ?, ?, ?, ?);"));
        for (auto entry : data["visits"]) {
            int id = static_cast<int>(entry["id"]);
            pstmt->setString(1, std::to_string(id));
            int location = static_cast<int>(entry["location"]);
            pstmt->setString(2, std::to_string(location));
            int user = static_cast<int>(entry["user"]);
            pstmt->setString(3, std::to_string(user));
            int visited_at = static_cast<int>(entry["visited_at"]);
            pstmt->setString(4, std::to_string(visited_at));
            int mark = static_cast<int>(entry["mark"]);
            pstmt->setString(5,std::to_string(mark));
            pstmt->execute();
        }
    }

    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->executeQuery("commit;");
    } catch(...) {}
}

bool MysqlStorage::user(Id id, User& o) {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::stringstream queryStream;
        queryStream << "select * from users where id = " << id << ";";
        sql::ResultSet *res = stmt->executeQuery(queryStream.str());

        while (res->next()) {
            o.id = res->getInt("id");
            o.email = res->getString("email");
            o.first_name = res->getString("first_name");
            o.last_name = res->getString("last_name");
            o.gender = res->getString("gender");
            o.birth_date = res->getInt("birth_date");

            return o.id > 0;
        }

    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
    return false;
}
bool MysqlStorage::location(Id id, Location& o) {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::stringstream queryStream;
        queryStream << "select * from locations where id = " << id << ";";
        sql::ResultSet *res = stmt->executeQuery(queryStream.str());

        while (res->next()) {
            o.id = std::stoi(res->getString("id"));
            o.place = res->getString("place");
            o.country = res->getString("country");
            o.city = res->getString("city");
            o.distance = std::stoi(res->getString("distance"));
            return o.id > 0;
        }

    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    return false;
}
bool MysqlStorage::visit(Id id, Visit& o) {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::stringstream queryStream;
        queryStream << "select * from visits where id = " << id << ";";
        sql::ResultSet *res = stmt->executeQuery(queryStream.str());

        while (res->next()) {
            o.id = std::stoi(res->getString("id"));
            o.location = std::stoi(res->getString("location"));
            o.user = std::stoi(res->getString("user"));
            o.visited_at = std::stoi(res->getString("visited_at"));
            o.mark = std::stoi(res->getString("mark"));
            return o.id > 0;
        }

    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
    return false;
}

json MysqlStorage::userVisits(Id id, ConditionMap conditions) {
    json visits = json::array();

    std::stringstream queryStream;

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

    queryStream << " order by v.visited_at;";

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    sql::ResultSet *res = stmt->executeQuery(queryStream.str());
    
    while (res->next()) {
        json visit;
        visit["place"] = res->getInt("place");
        visit["visited_at"] = res->getInt("visited_at");
        visit["mark"] = res->getInt("mark");
        visits.push_back(visit);
    }
    
    return visits;
}
double MysqlStorage::locationAvgRate(Id id, ConditionMap conditions) {
    std::stringstream queryStream;

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

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    sql::ResultSet *res = stmt->executeQuery(queryStream.str());

    double avg = 0.0;
    while (res->next()) {
        avg = res->getDouble(1);
    }

    return avg;
}

bool MysqlStorage::updateUser(Id id, json user) {
    std::stringstream queryStream;
    if (user.empty())
        return false;

    User u;
    if (!this->user(id, u))
        return false;

    queryStream << "update users set ";
    for (auto it = user.begin(); it != user.end(); ++it) {
        if (it != user.begin())
            queryStream << ", ";
        queryStream << it.key() << "=" << it.value();
    }
    queryStream << " where id = " << id << ";";
    //lock_guard<mutex> lock(Sqlite::storage_mutex);
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    sql::ResultSet *res = stmt->executeQuery(queryStream.str());
    return true;

}
bool MysqlStorage::updateLocation(Id id, json location) {
    std::stringstream queryStream;
    if (location.empty())
        return false;

    Location l;
    if (!this->location(id, l))
        return false;

    queryStream << "update locations set ";
    for (auto it = location.begin(); it != location.end(); ++it) {
        if (it != location.begin())
            queryStream << ", ";
        queryStream << it.key() << "=" << it.value();
    }
    queryStream << " where id = " << id << ";";
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    sql::ResultSet *res = stmt->executeQuery(queryStream.str());
    return true;
}

bool MysqlStorage::updateVisit(Id id, json visit) {
    std::stringstream queryStream;
    if (visit.empty())
        return false;

    Visit v;
    if (!this->visit(id, v))
        return false;

    queryStream << "update visits set ";
    for (auto it = visit.begin(); it != visit.end(); ++it) {
        if (it != visit.begin())
            queryStream << ", ";
        queryStream << it.key() << "=" << it.value();
    }
    queryStream << " where id = " << id << ";";
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    sql::ResultSet *res = stmt->executeQuery(queryStream.str());
    return true;
}

bool MysqlStorage::checkRecordExists(std::string table, Id id) {
    try {
        std::stringstream queryStream;
        queryStream << "select count(*) from " << table << " where id = " << id << ";";

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        sql::ResultSet *res = stmt->executeQuery(queryStream.str());

        if (res->next()) {
            return res->getInt(1) > 0;
        }
    } catch(...) {}
}

bool MysqlStorage::createUser(json user) {
    if (!User::validate(user, true))
        return false;

    Id id = user["id"];

    // check existence
    if (checkRecordExists("users", id))
        return false;

    // create a record
    try {
        std::stringstream queryStream;
        std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(
            "insert into users (id, email, first_name, last_name, gender, birth_date) "
            "values (?, ?, ?, ?, ?, ?);"));

        prep_stmt->setInt(1, user["id"]);
        prep_stmt->setString(2, user["email"].get<std::string>());
        prep_stmt->setString(3, user["first_name"].get<std::string>());
        prep_stmt->setString(4, user["last_name"].get<std::string>());
        prep_stmt->setString(5, user["gender"].get<std::string>());
        prep_stmt->setInt(6, user["birth_date"]);
        prep_stmt->execute();
    } catch(...) {
        std::cout << "XX catch ";
    }

    return true;
}
bool MysqlStorage::createLocation(json location) {
    if (!Location::validate(location, true))
        return false;
    
    // check existence
    Id id = location["id"];
    if (checkRecordExists("locations", id))
        return false;

    // create a record
    try {
        std::stringstream queryStream;
        std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(
            "insert into locations (id, place, country, city, distance) "
            "values (?, ?, ?, ?, ?);"));

        prep_stmt->setInt(1, location["id"]);
        prep_stmt->setString(2, location["place"].get<std::string>());
        prep_stmt->setString(3, location["country"].get<std::string>());
        prep_stmt->setString(4, location["city"].get<std::string>());
        prep_stmt->setInt(5, location["distance"]);
        prep_stmt->execute();
    } catch(...) {}
    return true;
}
bool MysqlStorage::createVisit(json visit) {
    if (!Visit::validate(visit, true))
        return false;
    
    // check existence
    Id id = visit["id"];
    if (checkRecordExists("visits", id))
        return false;
    // create a record
    try {
        std::stringstream queryStream;
        std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(
            "insert into visits (id, location, user, visited_at, mark) "
            "values (?, ?, ?, ?, ?);"));

        prep_stmt->setInt(1, visit["id"]);
        prep_stmt->setInt(2, visit["location"]);
        prep_stmt->setInt(3, visit["user"]);
        prep_stmt->setInt(4, visit["visited_at"]);
        prep_stmt->setInt(5, visit["mark"]);
        prep_stmt->execute();
    } catch(...) {}
    return true;
}