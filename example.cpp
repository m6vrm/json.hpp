#include <cassert>
#include <iostream>
#include <string>

#define JSON_IMPLEMENTATION
#define JSON_STRICT
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

    // get values
    assert(json1["null"].is_null());
    assert(json1["bool"].get_bool() == true);
    assert(json1["long"].get_long() == -69);
    assert(json1["double"].get_double() == -1.42);
    assert(json1["string"].get_string() == "value");

    assert(json1["array"].empty() == false);
    assert(json1["array"].size() == 5);
    assert(json1["array"][0].is_null());
    assert(json1["array"][1].is_bool());
    assert(json1["array"][2].is_long());
    assert(json1["array"][3].is_double());
    assert(json1["array"][4].is_string());

    assert(json1["empty_array"].empty());
    assert(json1.has("object"));

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
