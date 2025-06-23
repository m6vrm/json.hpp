#include <cassert>
#include <iostream>

#define JSON_IMPLEMENTATION
#include "json.hpp"

int main() {
    JSON json;
    json["hello"] = JSON::array();
    json["hello"].get_array().push_back(false);
    json["hello"].get_array().push_back(nullptr);
    json["hello"].get_array().push_back("hello");
    json["hello"].get_array().push_back(0.10001);
    json["hello"].get_array().push_back(200);
    json["hello"].get_array().push_back(JSON::object());
    json["object"] = JSON::object();

    std::string string = json.dump(true);
    std::cout << string << std::endl;

    JSON json2;
    std::cout << json2.parse(string) << std::endl;
    std::cout << json2.dump(true) << std::endl;
    return 0;
}
