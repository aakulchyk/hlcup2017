#include "mysqlstorage.h"
#include <fstream>
#include <string>

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
}
void MysqlStorage::populateLocationsTable() {
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
}

void MysqlStorage::populateVisitsTable() {
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
}

bool MysqlStorage::user(Id id, User& o) {
    bool _found = false;
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::stringstream queryStream;
        queryStream << "select * from users where id = " << id << ";";
        sql::ResultSet *res = stmt->executeQuery(queryStream.str());

        while (res->next()) {
            o.id = std::stoi(res->getString("id"));
            o.email = res->getString("email");
            o.first_name = res->getString("first_name");
            o.last_name = res->getString("last_name");
            o.gender = res->getString("gender");
            o.birth_date = std::stoi(res->getString("birth_date"));
            _found = true;
        }

    } catch (sql::SQLException &e) {
        std::cout << " (MySQL error code: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
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