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

#include "storage.h"
#include "requestparser.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using socket_ptr = boost::shared_ptr<ip::tcp::socket>;
io_service service;

AbstractStorage *storage;

void client_session(socket_ptr sock) {
    std::cout << "new client session" << std::endl;

    // read data
    boost::array<char, 512> buf;
    buf.assign(0);
    boost::system::error_code error;
    unsigned int id = 0;
    json content;
    //std::string status_code("200 OK");
    int status_code = 200;
    try {
        size_t len = sock->read_some(buffer(buf), error);
        std::string request(buf.data());
        auto requestType = UrlParser::instance().parse(request, id);

        std::cout << "REQUEST: " << std::endl << buf.data() << std::endl
                  << "---------------" << std::endl;
        //std::cout << "TYPE: " << requestType << std::endl;

        bool request_correct = false;

        switch (requestType) {
            case UrlParser::GET_USER: {
                content = storage->user(id).to_json();
                if (content.empty())
                    status_code = 404;
                break;
            }
            case UrlParser::GET_LOCATION: {
                content = storage->location(id).to_json();
                if (content.empty())
                    status_code = 404;
                break;
            }
            case UrlParser::GET_VISIT: {
                content = storage->visit(id).to_json();
                if (content.empty())
                    status_code = 404;
                break;
            }
            case UrlParser::GET_USER_VISITS: {
                content["visits"] = storage->userVisits(id);
                if (content.empty())
                    status_code = 404;
                break;
            }
            case UrlParser::GET_LOCATION_AVG_RATE: {
                content["avg"] = storage->locationAvgRate(id);
                if (static_cast<double>(content["avg"]) < 0.1)
                    status_code = 404;
                break;
            }
            case UrlParser::UPDATE_USER: {
                json user = UrlParser::instance().extractJson(request);
                if (User::validate(user))
                    status_code = storage->updateUser(id, user) ? 200 : 404;
                else
                    status_code = 400;
                break;
            }
            case UrlParser::UPDATE_LOCATION: {
                json location = UrlParser::instance().extractJson(request);
                status_code = storage->updateLocation(id, location) ? 200 : 404;
                break;
            }
            case UrlParser::UPDATE_VISIT: {
                json visit = UrlParser::instance().extractJson(request);
                status_code = storage->updateVisit(id, visit) ? 200 : 404;
                break;
            }
            case UrlParser::CREATE_USER:
            case UrlParser::CREATE_LOCATION:
            case UrlParser::CREATE_VISIT:
                break;
            default:
                status_code = 400;
        }
    }
    catch (std::exception& e) {
        std::cerr << "1 "  << e.what() << std::endl;
    }

    std::stringstream ss;
    ss << "HTTP/1.1 " << status_code << "\nContent-Length: "
       << content.dump().size()
       << "\nContent-Type: text/json; charset=utf-8"
       << "\n\n"
       << content;

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
