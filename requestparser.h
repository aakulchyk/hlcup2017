#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <map>
#include <utility>

class UrlParser {

public:
    enum RequestType {
        INVALID_REQUEST_TYPE = -1,
        GET_USER, GET_VISIT, GET_LOCATION, GET_USER_VISITS, GET_LOCATION_AVG_RATE,
        UPDATE_USER, UPDATE_VISIT, UPDATE_LOCATION,
        CREATE_USER, CREATE_VISIT, CREATE_LOCATION, TEST
    };


    // TODO: parse GET_USER_VISITS and GET_LOCATION_AVG_RATE requests with GET parameters
    RequestType parse(std::string input, unsigned int& id) {
        try {
            auto rt = INVALID_REQUEST_TYPE;
            std::smatch m;

            for (auto re : regex_map) {
                if (regex_search(input, m, re.second)) {
                    auto s = std::string(m[1]);
                    id = std::stoi(s);
                    // don't break bcause need to find matching with with longer pattern
                    // (exclusively for GET_LOCATION & GET_LOCATION_AVG_RATE)
                    rt = re.first;
                }
            }
            return rt;
        }
        catch (std::exception& e) {
            std::cout << "parse: " << e.what() << std::endl;
        }
    }

    std::string extractJson(std::string input) {
        try {
            std::string result("{}");
            std::regex re("(\\{.+\\})");
            std::smatch m;
            if (regex_search(input, m, re)) {
                result = std::string(m[1]);
            }
            return result;
        }
        catch (std::exception& e) {
            std::cout << "extractJson: " << e.what() << std::endl;
        }
    }

    using ConditionMap = std::map<std::string, std::string>;

    ConditionMap extractGetParams(std::string input, bool& valid) {
        try {
            ConditionMap result;
            std::smatch match;

            valid = true;

            while (regex_search(input, match, regex_get_params)) {
                std::string paramName = match[1];
                std::string paramValue = match[2];
                if (paramValue.empty())
                    valid = false;

                if (paramName == "gender") {
                    if (paramValue != "m" && paramValue != "f")
                        valid = false;
                }
                else if (paramName == "country") {
                   if (paramValue.length() > 50)
                       valid = false;
                }
                else {
                    // all the rest parameters are integer - check
                    std::regex re("-?[0-9]+");
                    std::smatch m;
                    if (!regex_match(paramValue, m, re))
                        valid = false;

                    try {
                        long int dummy = std::stoi(paramValue);
                    }
                    catch (std:: exception& e) {
                        valid = false;
                    }
                }
                result.insert(std::make_pair(match[1], match[2]));
                input = match.suffix().str();
            }

            return result;
        }
        catch (std::exception& e) {
            std::cout << "extractGetParams: " << e.what() << std::endl;
        }
    }

    static UrlParser& instance() {
        static UrlParser inst;
        return inst;
    }

private:
    UrlParser() {
        //regex_map[TEST] = "^TEST/[[:digit:]]+";
        regex_map[GET_USER] = "^GET[[:space:]]/users/([[:digit:]]+)";
        regex_map[GET_VISIT] = "^GET[[:space:]]/visits/([[:digit:]]+)";
        regex_map[GET_LOCATION] = "^GET[[:space:]]/locations/([[:digit:]]+)";
        regex_map[GET_USER_VISITS] = "^GET[[:space:]]/users/([[:digit:]]+)/visits";
        regex_map[GET_LOCATION_AVG_RATE] = "^GET[[:space:]]/locations/([[:digit:]]+)/avg";

        regex_map[UPDATE_USER] = "POST[[:space:]]/users/([[:digit:]]+)";
        regex_map[UPDATE_VISIT] = "POST[[:space:]]/visits/([[:digit:]]+)";
        regex_map[UPDATE_LOCATION] = "POST[[:space:]]/locations/([[:digit:]]+)";

        regex_map[CREATE_USER] = "POST[[:space:]]/users/new";
        regex_map[CREATE_VISIT] = "POST[[:space:]]/visits/new";
        regex_map[CREATE_LOCATION] = "POST[[:space:]]/locations/new";

        //regex_get_params = "[\\?\\&]([^\\&]+)";
        regex_get_params = "[\\?\\&]([[:alpha:]]+)=([[:alnum:]]*)";
    }

    static UrlParser *_instance;
    std::map<RequestType, std::regex> regex_map;
    std::regex regex_get_params;
};

UrlParser *UrlParser::_instance = nullptr;
