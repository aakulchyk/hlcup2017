#include <fstream>
#include <iostream>
#include <boost/asio.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <algorithm>
#include <functional>
//#include <boost/beast/http/parser.hpp>
#include <stdlib.h>
#include <boost/array.hpp>

#include <boost/locale/encoding_utf.hpp>

#include "storage.h"
#include "requestparser.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using socket_ptr = boost::shared_ptr<ip::tcp::socket>;
io_service service;

AbstractStorage *storage;

std::map<int, std::string> status = {
    {200, "200 OK"},
    {400, "400 Bad Request"},
    {404, "404 Not Found"}};

void client_session(socket_ptr sock) {
    std::cout << "new client session" << std::endl;

    // read data
    boost::array<char, 512> buf;
    buf.assign(0);
    boost::system::error_code error;
    unsigned int id = 0;
    json content;
    int code = 200;
    try {
        size_t len = sock->read_some(buffer(buf), error);
        std::string request(buf.data());
        auto requestType = UrlParser::instance().parse(request, id);

        std::cout << "REQUEST: " << std::endl << buf.data() << std::endl
                  << "---------------" << std::endl;

        switch (requestType) {
            case UrlParser::GET_USER: {
                User u;
                if (storage->user(id, u))
                    content = u;
                else
                    code = 404;
                break;
            }
            case UrlParser::GET_LOCATION: {
                Location l;
                if (storage->location(id, l))
                    content = l;
                else
                    code = 404;
                break;
            }
            case UrlParser::GET_VISIT: {
                Visit v;
                if (storage->visit(id, v))
                    content = v;
                else
                    code = 404;
                break;
            }
            case UrlParser::GET_USER_VISITS: {
                User u;
                if (storage->user(id, u)) {
                    auto conditions = UrlParser::instance().extractGetParams(request);
                    content["visits"] = storage->userVisits(id, conditions);
                }
                else
                    code = 404;
                break;
            }
            case UrlParser::GET_LOCATION_AVG_RATE: {
                Location l;
                if (storage->location(id, l)) {
                    auto conditions = UrlParser::instance().extractGetParams(request);
                    content["avg"] = storage->locationAvgRate(id, conditions);
                }
                else
                    code = 404;
                break;
            }
            case UrlParser::UPDATE_USER: {
                json user = UrlParser::instance().extractJson(request);
                if (User::validate(user))
                    code = storage->updateUser(id, user) ? 200 : 404;
                else
                    code = 400;
                break;
            }
            case UrlParser::UPDATE_LOCATION: {
                json location = UrlParser::instance().extractJson(request);
                code = storage->updateLocation(id, location) ? 200 : 404;
                break;
            }
            case UrlParser::UPDATE_VISIT: {
                json visit = UrlParser::instance().extractJson(request);
                code = storage->updateVisit(id, visit) ? 200 : 404;
                break;
            }
            case UrlParser::CREATE_USER:
            case UrlParser::CREATE_LOCATION:
            case UrlParser::CREATE_VISIT:
                break;
            default:
                code = 400;
        }
    }
    catch (std::exception& e) {
        std::cerr << "1 "  << e.what() << std::endl;
    }

    std::string dump = content.dump();
    std::stringstream ss;
    ss << "HTTP/1.1 " << status[code]
       << "\nContent-Length: " << dump.size()
       << "\nContent-Type: text/json; charset=utf-8"
       << std::endl << std::endl
       << dump;

    std::cout << std::endl << "RESPONSE: " << std::endl << ss.str() << std::endl
              << "---------------" << std::endl;;

    try {
        write(*sock, buffer(ss.str()));
    }
    catch (std::exception& e) {
        std::cerr << "2 " << e.what() << std::endl;
        sock->close();
    }
    std::cout << "Closing socket." << std::endl;
    sock->close();
}

#include "tests.h"
int main(int argc, char **argv) {
    // TESTS
    test_urlparser();

    if (argc < 2) {
        std::cout << "usage: server <port>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]);
    if (port < 0 || port > 9999) {
        std::cout << "Incorrect port!" << std::endl;
        return 2;
    }

    try {
        //Here is a simple synchronous server:using boost::asio;
        std::cout << "Starting server..." << std::endl;

        storage = new JsonStorage();

        ip::tcp::endpoint ep( ip::tcp::v4(), port); // listen on selected port
        ip::tcp::acceptor acc(service, ep);
        std::cout << "Entering cycle..." << std::endl;
        while (true)
        {
            socket_ptr sock(new ip::tcp::socket(service));
            acc.accept(*sock);
            std::thread( std::bind(client_session, sock)).detach();
        }
    }
    catch (std::exception& e) {
        std::cout << "0 " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "unknown exception" << std::endl;
    }

    return 0;
}
