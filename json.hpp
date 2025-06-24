// https://github.com/m6vrm/json.hpp
#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

class JSON {
   public:
    enum Type {
        TYPE_NULL,
        TYPE_BOOL,
        TYPE_LONG,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_ARRAY,
        TYPE_OBJECT,
    };

    enum Status {
        SUCCESS,
        END,
        DEPTH_EXCEEDED,
        INVALID_KEY_TYPE,
        INVALID_STRING_ESCAPE,
        INVALID_TOKEN,
        UNEXPECTED_STRING_END,
#ifdef JSON_STRICT
        INVALID_NUMBER,
        TRAILING_CONTENT,
        UNEXPECTED_ARRAY,
        UNEXPECTED_ARRAY_END,
        UNEXPECTED_COLON,
        UNEXPECTED_COMMA,
        UNEXPECTED_END,
        UNEXPECTED_NUMBER,
        UNEXPECTED_OBJECT,
        UNEXPECTED_OBJECT_END,
        UNEXPECTED_STRING,
        UNEXPECTED_TOKEN,
#endif  // JSON_STRICT
    };

    static const char* status_string(Status status);

    static JSON array(const std::vector<JSON>& array = {});
    static JSON object(const std::map<std::string, JSON>& object = {});

    JSON(const std::nullptr_t = nullptr);
    JSON(bool value);
    JSON(int value);
    JSON(long long value);
    JSON(float value);
    JSON(double value);
    JSON(const char* value);
    JSON(std::string&& value);
    JSON(const std::string& value);
    ~JSON();

    JSON(const JSON& other);
    JSON& operator=(const JSON& other);
    JSON(JSON&& other) noexcept;
    JSON& operator=(JSON&& other) noexcept;

    Type type() const;

    bool is_null() const;
    bool is_bool() const;
    bool is_long() const;
    bool is_double() const;
    bool is_string() const;
    bool is_array() const;
    bool is_object() const;

    bool get_bool() const;
    long long get_long() const;
    double get_double() const;
    std::string& get_string();
    std::vector<JSON>& get_array();
    std::map<std::string, JSON>& get_object();

    JSON& operator[](std::size_t idx);
    JSON& operator[](const std::string& key);

    std::size_t size() const;
    bool empty() const;
    bool has(const std::string& key) const;

    void clear();

    bool parse(const std::string& src, Status* status = nullptr);
    std::string dump(bool indent = false) const;

   private:
    void init_string();
    void init_array();
    void init_object();

    Status decode(const char*& start, const char* end, int ctx, std::size_t depth);
    void encode(std::string& dst, bool pretty, int indent) const;

    Type type_;
    union {
        bool as_bool_;
        long long as_long_;
        double as_double_;
        std::string as_string_;
        std::vector<JSON> as_array_;
        std::map<std::string, JSON> as_object_;
    };
};

#ifdef JSON_IMPLEMENTATION

#include <cassert>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <utility>

#ifndef JSON_MAX_DEPTH
#define JSON_MAX_DEPTH 16
#endif  // JSON_MAX_DEPTH

static void string_escape(std::string& dst, const std::string& src);

const char* JSON::status_string(Status status) {
    switch (status) {
        case SUCCESS:
            return "SUCCESS";
        case END:
            return "END";
        case DEPTH_EXCEEDED:
            return "DEPTH_EXCEEDED";
        case INVALID_KEY_TYPE:
            return "INVALID_KEY_TYPE";
        case INVALID_STRING_ESCAPE:
            return "INVALID_STRING_ESCAPE";
        case INVALID_TOKEN:
            return "INVALID_TOKEN";
        case UNEXPECTED_STRING_END:
            return "UNEXPECTED_STRING_END";
#ifdef JSON_STRICT
        case INVALID_NUMBER:
            return "INVALID_NUMBER";
        case TRAILING_CONTENT:
            return "TRAILING_CONTENT";
        case UNEXPECTED_ARRAY:
            return "UNEXPECTED_ARRAY";
        case UNEXPECTED_ARRAY_END:
            return "UNEXPECTED_ARRAY_END";
        case UNEXPECTED_COLON:
            return "UNEXPECTED_COLON";
        case UNEXPECTED_COMMA:
            return "UNEXPECTED_COMMA";
        case UNEXPECTED_END:
            return "UNEXPECTED_END";
        case UNEXPECTED_NUMBER:
            return "UNEXPECTED_NUMBER";
        case UNEXPECTED_OBJECT:
            return "UNEXPECTED_OBJECT";
        case UNEXPECTED_OBJECT_END:
            return "UNEXPECTED_OBJECT_END";
        case UNEXPECTED_STRING:
            return "UNEXPECTED_STRING";
        case UNEXPECTED_TOKEN:
            return "UNEXPECTED_TOKEN";
#endif  // JSON_STRICT
        default:
            assert(false);
            return "UNKNOWN_STATUS";
    }
}

JSON JSON::array(const std::vector<JSON>& array) {
    JSON json;
    json.init_array();
    json.as_array_ = std::move(array);
    return json;
}

JSON JSON::object(const std::map<std::string, JSON>& object) {
    JSON json;
    json.init_object();
    json.as_object_ = std::move(object);
    return json;
}

JSON::JSON(const std::nullptr_t) : type_{TYPE_NULL} {}
JSON::JSON(bool value) : type_{TYPE_BOOL}, as_bool_{value} {}
JSON::JSON(int value) : type_{TYPE_LONG}, as_long_{value} {}
JSON::JSON(long long value) : type_{TYPE_LONG}, as_long_{value} {}
JSON::JSON(float value) : type_{TYPE_DOUBLE}, as_double_{value} {}
JSON::JSON(double value) : type_{TYPE_DOUBLE}, as_double_{value} {}
JSON::JSON(const char* value) : type_{TYPE_STRING}, as_string_{value} {}
JSON::JSON(std::string&& value) : type_{TYPE_STRING}, as_string_{std::move(value)} {}
JSON::JSON(const std::string& value) : type_{TYPE_STRING}, as_string_{std::move(value)} {}

JSON::~JSON() {
    clear();
}

JSON::JSON(const JSON& other) : type_{other.type_} {
    switch (type_) {
        case TYPE_NULL:
            break;
        case TYPE_BOOL: {
            as_bool_ = other.as_bool_;
        } break;
        case TYPE_LONG: {
            as_long_ = other.as_long_;
        } break;
        case TYPE_DOUBLE: {
            as_double_ = other.as_double_;
        } break;
        case TYPE_STRING: {
            new (&as_string_) std::string{other.as_string_};
        } break;
        case TYPE_ARRAY: {
            new (&as_array_) std::vector{other.as_array_};
        } break;
        case TYPE_OBJECT: {
            new (&as_object_) std::map{other.as_object_};
        } break;
        default:
            assert(false);
    }
}

JSON& JSON::operator=(const JSON& other) {
    if (this != &other) {
        clear();
        type_ = other.type_;

        switch (type_) {
            case TYPE_NULL:
                break;
            case TYPE_BOOL: {
                as_bool_ = other.as_bool_;
            } break;
            case TYPE_LONG: {
                as_long_ = other.as_long_;
            } break;
            case TYPE_DOUBLE: {
                as_double_ = other.as_double_;
            } break;
            case TYPE_STRING: {
                new (&as_string_) std::string{other.as_string_};
            } break;
            case TYPE_ARRAY: {
                new (&as_array_) std::vector{other.as_array_};
            } break;
            case TYPE_OBJECT: {
                new (&as_object_) std::map{other.as_object_};
            } break;
            default:
                assert(false);
        }
    }

    return *this;
}

JSON::JSON(JSON&& other) noexcept : type_{other.type_} {
    other.type_ = TYPE_NULL;

    switch (type_) {
        case TYPE_NULL:
            break;
        case TYPE_BOOL: {
            as_bool_ = other.as_bool_;
        } break;
        case TYPE_LONG: {
            as_long_ = other.as_long_;
        } break;
        case TYPE_DOUBLE: {
            as_double_ = other.as_double_;
        } break;
        case TYPE_STRING: {
            new (&as_string_) std::string{std::move(other.as_string_)};
        } break;
        case TYPE_ARRAY: {
            new (&as_array_) std::vector{std::move(other.as_array_)};
        } break;
        case TYPE_OBJECT: {
            new (&as_object_) std::map{std::move(other.as_object_)};
        } break;
        default:
            assert(false);
    }
}

JSON& JSON::operator=(JSON&& other) noexcept {
    if (this != &other) {
        clear();
        type_ = other.type_;
        other.type_ = TYPE_NULL;

        switch (type_) {
            case TYPE_NULL:
                break;
            case TYPE_BOOL: {
                as_bool_ = other.as_bool_;
            } break;
            case TYPE_LONG: {
                as_long_ = other.as_long_;
            } break;
            case TYPE_DOUBLE: {
                as_double_ = other.as_double_;
            } break;
            case TYPE_STRING: {
                new (&as_string_) std::string{std::move(other.as_string_)};
            } break;
            case TYPE_ARRAY: {
                new (&as_array_) std::vector{std::move(other.as_array_)};
            } break;
            case TYPE_OBJECT: {
                new (&as_object_) std::map{std::move(other.as_object_)};
            } break;
            default:
                assert(false);
        }
    }

    return *this;
}

JSON::Type JSON::type() const {
    return type_;
}

bool JSON::is_null() const {
    return type_ == TYPE_NULL;
}

bool JSON::is_bool() const {
    return type_ == TYPE_BOOL;
}

bool JSON::is_long() const {
    return type_ == TYPE_LONG;
}

bool JSON::is_double() const {
    return type_ == TYPE_DOUBLE;
}

bool JSON::is_string() const {
    return type_ == TYPE_STRING;
}

bool JSON::is_array() const {
    return type_ == TYPE_ARRAY;
}

bool JSON::is_object() const {
    return type_ == TYPE_OBJECT;
}

bool JSON::get_bool() const {
    switch (type_) {
        case TYPE_BOOL:
            return as_bool_;
        default:
            return false;
    }
}

long long JSON::get_long() const {
    switch (type_) {
        case TYPE_LONG:
            return as_long_;
        case TYPE_DOUBLE:
            return as_double_;
        default:
            return 0;
    }
}

double JSON::get_double() const {
    switch (type_) {
        case TYPE_LONG:
            return as_long_;
        case TYPE_DOUBLE:
            return as_double_;
        default:
            return 0;
    }
}

std::string& JSON::get_string() {
    if (type_ != TYPE_STRING)
        init_string();
    return as_string_;
}

std::vector<JSON>& JSON::get_array() {
    if (type_ != TYPE_ARRAY)
        init_array();
    return as_array_;
}

std::map<std::string, JSON>& JSON::get_object() {
    if (type_ != TYPE_OBJECT)
        init_object();
    return as_object_;
}

JSON& JSON::operator[](std::size_t idx) {
    if (type_ != TYPE_ARRAY)
        init_array();
    if (idx >= as_array_.size())
        as_array_.resize(idx + 1);
    return as_array_[idx];
}

JSON& JSON::operator[](const std::string& key) {
    if (type_ != TYPE_OBJECT)
        init_object();
    return as_object_[key];
}

std::size_t JSON::size() const {
    switch (type_) {
        case TYPE_STRING:
            return as_string_.size();
        case TYPE_ARRAY:
            return as_array_.size();
        case TYPE_OBJECT:
            return as_object_.size();
        default:
            return 0;
    }
}

bool JSON::empty() const {
    return size() == 0;
}

bool JSON::has(const std::string& key) const {
    if (type_ != TYPE_OBJECT)
        return false;
    return as_object_.find(key) != as_object_.end();
}

void JSON::clear() {
    switch (type_) {
        case TYPE_STRING: {
            as_string_.~basic_string();
        } break;
        case TYPE_ARRAY: {
            as_array_.~vector();
        } break;
        case TYPE_OBJECT: {
            as_object_.~map();
        } break;
        default:
            break;
    }
}

bool JSON::parse(const std::string& src, Status* status) {
    const char* start = src.data();
    const char* end = src.data() + src.size();
    Status s1 = decode(start, end, 0, 0);
#ifdef JSON_STRICT
    if (s1 == SUCCESS) {
        JSON j2;
        Status s2 = j2.decode(start, end, 0, 0);
        if (s2 != END)
            s1 = TRAILING_CONTENT;
    }
#endif  // JSON_STRICT
    if (status != nullptr)
        *status = s1;
    return s1 == SUCCESS;
}

std::string JSON::dump(bool pretty) const {
    std::string string;
    encode(string, pretty, 1);
    return string;
}

void JSON::init_string() {
    clear();
    type_ = TYPE_STRING;
    new (&as_string_) std::string{};
}

void JSON::init_array() {
    clear();
    type_ = TYPE_ARRAY;
    new (&as_array_) std::vector<JSON>{};
}

void JSON::init_object() {
    clear();
    type_ = TYPE_OBJECT;
    new (&as_object_) std::map<std::string, JSON>{};
}

JSON::Status JSON::decode(const char*& start, const char* end, int ctx, std::size_t depth) {
    assert(start <= end);

    if (depth > JSON_MAX_DEPTH)
        return DEPTH_EXCEEDED;

    enum {  // ctx, represents where we are and what to expect
        CTX_OBJECT = 1 << 1,
        CTX_ARRAY = 1 << 2,
        CTX_KEY = 1 << 3,
        CTX_COLON = 1 << 4,
        CTX_COMMA = 1 << 5,
    };

    int sign = +1;
    while (start < end) {
        switch (*start++) {
            case ' ':  // whitespace
            case '\n':
            case '\r':
            case '\t':
                break;

            case '/': {  // comment
#ifdef JSON_STRICT
                if (*start++ != '/')
                    return INVALID_TOKEN;
#endif  // JSON_STRICT
                while (*start != '\n' && start < end)
                    ++start;
            } break;

            case '{': {  // object
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_OBJECT;
#endif  // JSON_STRICT
                init_object();
                ctx = CTX_OBJECT | CTX_KEY;
                JSON key, value;
                for (;;) {
                    Status status = key.decode(start, end, ctx, depth + 1);
                    if (status == END)
                        return SUCCESS;
                    if (status != SUCCESS)
                        return status;
                    if (key.type() != TYPE_STRING)
                        return INVALID_KEY_TYPE;
                    status = value.decode(start, end, CTX_COLON, depth + 1);
                    if (status != SUCCESS)
                        return status;
                    as_object_.emplace(std::move(key.as_string_), std::move(value));
                    ctx = CTX_OBJECT | CTX_KEY | CTX_COMMA;
                }
            } break;
            case '}':
#ifdef JSON_STRICT
                if (ctx & CTX_OBJECT)
                    return END;
                return UNEXPECTED_OBJECT_END;
#else   // JSON_STRICT
                return END;
#endif  // JSON_STRICT

            case '[': {  // array
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_ARRAY;
#endif  // JSON_STRICT
                init_array();
                ctx = CTX_ARRAY;
                JSON value;
                for (;;) {
                    Status status = value.decode(start, end, ctx, depth + 1);
                    if (status == END)
                        return SUCCESS;
                    if (status != SUCCESS)
                        return status;
                    as_array_.emplace_back(std::move(value));
                    ctx = CTX_ARRAY | CTX_COMMA;
                }
            } break;
            case ']':
#ifdef JSON_STRICT
                if (ctx & CTX_ARRAY)
                    return END;
                return UNEXPECTED_ARRAY_END;
#else   // JSON_STRICT
                return END;
#endif  // JSON_STRICT

            case ',': {  // array or object
#ifdef JSON_STRICT
                if ((ctx & CTX_COMMA) == 0)
                    return UNEXPECTED_COMMA;
                ctx &= ~CTX_COMMA;
#endif  // JSON_STRICT
            } break;

            case ':': {  // object
#ifdef JSON_STRICT
                if ((ctx & CTX_COLON) == 0)
                    return UNEXPECTED_COLON;
                ctx &= ~CTX_COLON;
#endif  // JSON_STRICT
            } break;

            case '"': {  // string
#ifdef JSON_STRICT
                if (ctx & (CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_STRING;
#endif  // JSON_STRICT
                init_string();
                while (start < end) {
                    switch (*start++) {
                        case '"':  // end
                            return SUCCESS;
                        case '\\': {  // escape
                            switch (*start++) {
                                case '"':
                                case '\\':
                                case '/': {
                                    as_string_ += *(start - 1);
                                } break;
                                case 'b': {
                                    as_string_ += '\b';
                                } break;
                                case 'f': {
                                    as_string_ += '\f';
                                } break;
                                case 'n': {
                                    as_string_ += '\n';
                                } break;
                                case 'r': {
                                    as_string_ += '\r';
                                } break;
                                case 't': {
                                    as_string_ += '\t';
                                } break;
                                default:
                                    return INVALID_STRING_ESCAPE;
                            }
                        } break;
                        default: {
                            as_string_ += *(start - 1);
                        } break;
                    }
                }
                return UNEXPECTED_STRING_END;
            } break;

            case 't': {  // true
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_TOKEN;
                if (std::strncmp(start, "rue", 3) != 0)
                    return INVALID_TOKEN;
#endif  // JSON_STRICT
                start += 3;
                start = start < end ? start : end;
                type_ = TYPE_BOOL;
                as_bool_ = true;
                return SUCCESS;
            } break;

            case 'f': {  // false
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_TOKEN;
                if (std::strncmp(start, "alse", 4) != 0)
                    return INVALID_TOKEN;
#endif  // JSON_STRICT
                start += 4;
                start = start < end ? start : end;
                type_ = TYPE_BOOL;
                as_bool_ = false;
                return SUCCESS;
            } break;

            case 'n': {  // null
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_TOKEN;
                if (std::strncmp(start, "ull", 3) != 0)
                    return INVALID_TOKEN;
#endif  // JSON_STRICT
                start += 3;
                start = start < end ? start : end;
                type_ = TYPE_NULL;
                return SUCCESS;
            } break;

            case '-': {  // negative
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_TOKEN;
                if (!std::isdigit(*start))
                    return UNEXPECTED_TOKEN;
#endif  // JSON_STRICT
                sign = -1;
            } break;

            case '0': {  // zero or double
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_NUMBER;
                if (std::isdigit(*start))
                    return INVALID_NUMBER;
#endif  // JSON_STRICT
                if (*start == '.' || *start == 'e' || *start == 'E')
                    goto decode_double;
                type_ = TYPE_LONG;
                as_long_ = 0;
                return SUCCESS;
            } break;

            case '1':  // long
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_NUMBER;
#endif  // JSON_STRICT
                for (const char* c = start; c < end; ++c) {
                    if (*c == '.' || *c == 'e' || *c == 'E')
                        goto decode_double;
                    if (!std::isdigit(*c))
                        break;
                }
                type_ = TYPE_LONG;
                std::from_chars_result result = std::from_chars(start - 1, end, as_long_);
#ifdef JSON_STRICT
                if (result.ec != std::errc{})
                    return INVALID_NUMBER;
#endif  // JSON_STRICT
                as_long_ *= sign;
                start = result.ptr;
                return SUCCESS;
            } break;

            decode_double: {  // double
#ifdef JSON_STRICT
                if (ctx & (CTX_KEY | CTX_COLON | CTX_COMMA))
                    return UNEXPECTED_NUMBER;
#endif  // JSON_STRICT
                type_ = TYPE_DOUBLE;
                std::from_chars_result result = std::from_chars(start - 1, end, as_double_);
#ifdef JSON_STRICT
                if (result.ec != std::errc{})
                    return INVALID_NUMBER;
#endif  // JSON_STRICT
                as_double_ *= sign;
                start = result.ptr;
                return SUCCESS;
            } break;

            default:
                return INVALID_TOKEN;
        }
    }

#ifdef JSON_STRICT
    if (depth > 0)
        return UNEXPECTED_END;
#endif  // JSON_STRICT
    return END;
}

void JSON::encode(std::string& dst, bool pretty, int indent) const {
    switch (type_) {
        case TYPE_NULL: {
            dst += "null";
        } break;
        case TYPE_BOOL: {
            dst += as_bool_ ? "true" : "false";
        } break;
        case TYPE_LONG: {
            char buf[64];
            std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), as_long_);
            dst.append(buf, result.ptr);
        } break;
        case TYPE_DOUBLE: {
            char buf[128];
            std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), as_double_);
            dst.append(buf, result.ptr);
        } break;
        case TYPE_STRING: {
            string_escape(dst, as_string_);
        } break;
        case TYPE_ARRAY: {
            dst += pretty ? "[\n" : "[";
            for (auto it = as_array_.begin(); it != as_array_.end(); ++it) {
                if (pretty) {
                    for (int i = 0; i < indent; ++i)
                        dst += "  ";
                }
                it->encode(dst, pretty, indent + 1);
                if (std::next(it) != as_array_.end())
                    dst += ',';
                if (pretty)
                    dst += '\n';
            }
            if (pretty) {
                for (int i = 0; i < indent - 1; ++i)
                    dst += "  ";
            }
            dst += ']';
        } break;
        case TYPE_OBJECT: {
            dst += pretty ? "{\n" : "{";
            for (auto it = as_object_.begin(); it != as_object_.end(); ++it) {
                if (pretty) {
                    for (int i = 0; i < indent; ++i)
                        dst += "  ";
                }
                string_escape(dst, it->first);
                dst += pretty ? ": " : ":";
                it->second.encode(dst, pretty, indent + 1);
                if (std::next(it) != as_object_.end())
                    dst += ',';
                if (pretty)
                    dst += '\n';
            }
            if (pretty) {
                for (int i = 0; i < indent - 1; ++i)
                    dst += "  ";
            }
            dst += '}';
        } break;
        default:
            assert(false);
            break;
    }
}

static void string_escape(std::string& dst, const std::string& src) {
    dst.reserve(dst.size() + src.size());
    dst += "\"";
    for (char c : src) {
        switch (c) {
            case '"': {
                dst += "\\\"";
            } break;
            case '\\': {
                dst += "\\\\";
            } break;
            case '/': {
                dst += "\\/";
            } break;
            case '\b': {
                dst += "\\b";
            } break;
            case '\f': {
                dst += "\\f";
            } break;
            case '\n': {
                dst += "\\n";
            } break;
            case '\r': {
                dst += "\\r";
            } break;
            case '\t': {
                dst += "\\t";
            } break;
            default: {
                dst += c;
            } break;
        }
    }
    dst += "\"";
}

#endif  // JSON_IMPLEMENTATION
