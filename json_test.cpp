#include <cassert>
#include <cstdio>
#include <string>

#define JSON_IMPLEMENTATION
#include "json.hpp"

int main() {
    JSON::Status status;

    {
        std::printf("empty: ");
        JSON json;
        std::string string = "";
        assert(json.parse(string, &status) == false);
        assert(status == JSON::END);
        assert(json.dump() == "null");
        std::printf("success\n");
    }

    {
        std::printf("whitespace: ");
        JSON json;
        std::string string = " ";
        assert(json.parse(string, &status) == false);
        assert(status == JSON::END);
        assert(json.dump() == "null");
        std::printf("success\n");
    }

    {
        std::printf("comment: ");
        JSON json;
        std::string string = "// comment";
        assert(json.parse(string, &status) == false);
        assert(status == JSON::END);
        assert(json.dump() == "null");
        std::printf("success\n");
    }

    {
        std::printf("null: ");
        JSON json;
        std::string string = "null";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_null());
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("true: ");
        JSON json;
        std::string string = "true";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_bool());
        assert(json.is_bool());
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("false: ");
        JSON json;
        std::string string = "false";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_bool());
        assert(json.get_bool() == false);
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("int64: ");
        JSON json;
        std::string string = "-69";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_int64());
        assert(json.get_int64() == -69);
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("double: ");
        JSON json;
        std::string string = "1.42";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_double());
        assert(json.get_double() == 1.42);
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("string: ");
        JSON json;
        std::string string = R"("hello \"\\\/\b\f\n\r\t world")";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_string());
        assert(json.get_string() == "hello \"\\/\b\f\n\r\t world");
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("empty array: ");
        JSON json;
        std::string string = "[]";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_array());
        assert(json.empty() == true);
        assert(json.size() == 0);
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("array trailing comma: ");
        JSON json;
        assert(json.parse("[ -69, ]", &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_array());
        assert(json.empty() == false);
        assert(json.size() == 1);
        assert(json[0].get_int64() == -69);
        std::printf("success\n");
    }

    {
        std::printf("array: ");
        JSON json;
        std::string string = R"([null,true,false,-69,1.42,"hello world",[],{}])";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_array());
        assert(json.empty() == false);
        assert(json.size() == 8);
        assert(json[0].is_null());
        assert(json[1].get_bool() == true);
        assert(json[2].get_bool() == false);
        assert(json[3].get_int64() == -69);
        assert(json[4].get_double() == 1.42);
        assert(json[5].get_string() == "hello world");
        assert(json[6].is_array());
        assert(json[7].is_object());
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("empty object: ");
        JSON json;
        std::string string = "{}";
        assert(json.parse(string, &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_object());
        assert(json.empty() == true);
        assert(json.size() == 0);
        assert(json.dump() == string);
        std::printf("success\n");
    }

    {
        std::printf("object trailing comma: ");
        JSON json;
        assert(json.parse(R"({ "key": "value", })", &status) == true);
        assert(status == JSON::SUCCESS);
        assert(json.is_object());
        assert(json.empty() == false);
        assert(json.size() == 1);
        assert(json["key"].get_string() == "value");
        assert(json.has("key"));
        assert(json.has("missing_key") == false);
        std::printf("success\n");
    }

    {
        std::printf("complex object: ");
        JSON json;
        std::string string = R"(
            {
              // comments are ignored
              "null": null,
              "bool": true,
              "int64": -69,
              "double": -1.42,
              "string": "value",
              "array": [null, true, -69, -1.42, "value"],
              "object": {
                "null": null,
                "bool": true,
                "int64": -69,
                "double": -1.42,
                "string": "value"
              },
              "empty_array": [],
              "empty_object": {},
              "another_object": {"key": "value"}, // trailing comma is allowed
            }
        )";
        assert(json.parse(string, &status));
        assert(status == JSON::SUCCESS);

        assert(json["null"].is_null());
        assert(json["bool"].get_bool() == true);
        assert(json["int64"].get_int64() == -69);
        assert(json["double"].get_double() == -1.42);
        assert(json["string"].get_string() == "value");

        assert(json.has("array"));
        assert(json["array"].empty() == false);
        assert(json["array"].size() == 5);
        assert(json["array"][0].is_null());
        assert(json["array"][1].is_bool());
        assert(json["array"][2].is_int64());
        assert(json["array"][3].is_double());
        assert(json["array"][4].is_string());

        assert(json.has("object"));
        assert(json["object"].empty() == false);
        assert(json["object"].size() == 5);
        assert(json["object"]["null"].is_null());
        assert(json["object"]["bool"].get_bool() == true);
        assert(json["object"]["int64"].get_int64() == -69);
        assert(json["object"]["double"].get_double() == -1.42);
        assert(json["object"]["string"].get_string() == "value");

        assert(json["empty_array"].empty());
        assert(json["empty_object"].empty());
        std::printf("success\n");
    }

    {
        std::printf("fallback: ");
        JSON json;
        assert(json.get_bool(true) == true);
        assert(json.get_int64(-69) == -69);
        assert(json.get_double(-1.42) == -1.42);
        assert(json.get_string("value") == "value");
        std::printf("success\n");
    }

    std::printf("success\n");
    return 0;
}
