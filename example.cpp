#include <cassert>
#include <iostream>
#include <string>

#define JSON_IMPLEMENTATION
#include "json.hpp"
int main() {
    // parse JSON
    JSON::Status status;  // status is optional
    JSON json1;
    bool success = json1.parse(R"(
        {
          // comments are ignored
          "null": null,
          "bool": true,
          "long": -69,
          "double": -1.42,
          "string": "value",
          "array": [null, true, -69, -1.42, "value"],
          "empty_array": [],
          "object": {
            "null": null,
            "bool": true,
            "long": -69,
            "double": -1.42,
            "string": "value"
          },
          "empty_object": {},
          "another_object": {"key": "value"}, // trailing comma is allowed
        }
    )",
                               &status);
    assert(success);
    std::cout << JSON::status_string(status) << std::endl;

    // dump to string
    std::cout << json1.dump(true) << std::endl;

    // create JSON
    JSON json2;
    json2["null"] = nullptr;
    json2["bool"] = true;
    json2["long"] = -69;
    json2["double"] = -1.42;
    json2["string"] = "value";
    json2["array"] = JSON::array({nullptr, true, -69, -1.42, "value"});
    json2["empty_array"] = JSON::array();
    json2["object"] = JSON::object({
        {"null", nullptr},
        {"bool", true},
        {"long", -69},
        {"double", -1.42},
        {"string", "value"},
    });
    json2["empty_object"] = JSON::object();
    json2["another_object"]["key"] = "value";

    // dump to string
    std::cout << json2.dump(true) << std::endl;

    assert(json1.dump() == json2.dump());

    return 0;
}
