#include <cassert>
#include <iostream>
#include <string>

#define JSON_IMPLEMENTATION
#define JSON_STRICT  // strict parsing is a bit slower, but doesn't allow invalid json
#include "json.hpp"

int main() {
    // parse json
    JSON::Status status;  // status is optional
    JSON json1;
    std::string string = R"(
        {
          // comments are ignored
          "key": "value", // trailing comma is allowed
        }
    )";
    bool success = json1.parse(string, &status);
    assert(success);
    std::cout << JSON::status_string(status) << std::endl;

    // get values
    assert(json1.empty() == false);
    assert(json1.size() == 1);
    assert(json1.has("key"));
    assert(json1["key"].get_string() == "value");

    // create json
    JSON json2;
    json2["key"] = "value";

    // dump to string
    std::cout << json2.dump(true) << std::endl;
    assert(json1.dump() == json2.dump());

    return 0;
}
