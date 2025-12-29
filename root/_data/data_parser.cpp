#include "data_parser.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <algorithm>

namespace DataParser {
    static std::string readFile(const std::string& filename) {
        std::ifstream file(filename);
        if(!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    Value::Value(std::initializer_list<std::pair<std::string, Value>> init) 
        : type(ValueType::Object) {
        object_value = new std::unordered_map<std::string, Value>();
        for(const auto& pair : init) {
            (*object_value)[pair.first] = pair.second;
        }
    }
 
    Value::Value(std::initializer_list<Value> init) 
        : type(ValueType::Array) {
        array_value = new std::vector<Value>(init);
    }
    
    Value::~Value() {
        cleanup();
    }
    
    void Value::cleanup() {
        switch (type) {
            case ValueType::String:
                delete string_value;
                break;
            case ValueType::Array:
                delete array_value;
                break;
            case ValueType::Object:
                delete object_value;
                break;
            default:
                break;
        }
        type = ValueType::Null;
    }
    
    void Value::copyFrom(const Value& other) {
        type = other.type;
        switch (type) {
            case ValueType::Null:
                break;
            case ValueType::Boolean:
                bool_value = other.bool_value;
                break;
            case ValueType::Number:
                number_value = other.number_value;
                break;
            case ValueType::String:
                string_value = new std::string(*other.string_value);
                break;
            case ValueType::Array:
                array_value = new std::vector<Value>(*other.array_value);
                break;
            case ValueType::Object:
                object_value = new std::unordered_map<std::string, Value>(*other.object_value);
                break;
        }
    }
    
    Value::Value(const Value& other) {
        copyFrom(other);
    }
    
    Value& Value::operator=(const Value& other) {
        if(this != &other) {
            cleanup();
            copyFrom(other);
        }
        return *this;
    }
    
    Value::Value(Value&& other) noexcept 
        : type(other.type) {
        switch (type) {
            case ValueType::Null:
                break;
            case ValueType::Boolean:
                bool_value = other.bool_value;
                break;
            case ValueType::Number:
                number_value = other.number_value;
                break;
            case ValueType::String:
                string_value = other.string_value;
                other.string_value = nullptr;
                break;
            case ValueType::Array:
                array_value = other.array_value;
                other.array_value = nullptr;
                break;
            case ValueType::Object:
                object_value = other.object_value;
                other.object_value = nullptr;
                break;
        }
        other.type = ValueType::Null;
    }
    
    Value& Value::operator=(Value&& other) noexcept {
        if(this != &other) {
            cleanup();
            type = other.type;
            switch (type) {
                case ValueType::Null:
                    break;
                case ValueType::Boolean:
                    bool_value = other.bool_value;
                    break;
                case ValueType::Number:
                    number_value = other.number_value;
                    break;
                case ValueType::String:
                    string_value = other.string_value;
                    other.string_value = nullptr;
                    break;
                case ValueType::Array:
                    array_value = other.array_value;
                    other.array_value = nullptr;
                    break;
                case ValueType::Object:
                    object_value = other.object_value;
                    other.object_value = nullptr;
                    break;
            }
            other.type = ValueType::Null;
        }
        return *this;
    }
    
    bool Value::asBoolean() const {
        if(type != ValueType::Boolean) {
            throw std::runtime_error("Value is not a boolean");
        }
        return bool_value;
    }
    
    double Value::asNumber() const {
        if(type != ValueType::Number) {
            throw std::runtime_error("Value is not a number");
        }
        return number_value;
    }
    
    int Value::asInt() const {
        return static_cast<int>(asNumber());
    }
    
    float Value::asFloat() const {
        return static_cast<float>(asNumber());
    }
    
    const std::string& Value::asString() const {
        if(type != ValueType::String) {
            throw std::runtime_error("Value is not a string");
        }
        return *string_value;
    }
    
    const std::vector<Value>& Value::asArray() const {
        if(type != ValueType::Array) {
            throw std::runtime_error("Value is not an array");
        }
        return *array_value;
    }
    
    const std::unordered_map<std::string, Value>& Value::asObject() const {
        if(type != ValueType::Object) {
            throw std::runtime_error("Value is not an object");
        }
        return *object_value;
    }
    
    Value& Value::operator[](size_t index) {
        if(type != ValueType::Array) {
            throw std::runtime_error("Value is not an array");
        }
        return (*array_value)[index];
    }
    const Value& Value::operator[](size_t index) const {
        if(type != ValueType::Array) {
            throw std::runtime_error("Value is not an array");
        }
        return (*array_value)[index];
    }
    
    Value& Value::operator[](const std::string& key) {
        if(type != ValueType::Object) {
            throw std::runtime_error("Value is not an object");
        }
        return (*object_value)[key];
    }
    const Value& Value::operator[](const std::string& key) const {
        if(type != ValueType::Object) {
            throw std::runtime_error("Value is not an object");
        }
        auto it = object_value->find(key);
        if(it == object_value->end()) {
            throw std::runtime_error("Key not found: " + key);
        }
        return it->second;
    }
    
    bool Value::hasKey(const std::string& key) const {
        if(type != ValueType::Object) return false;
        return object_value->find(key) != object_value->end();
    }
    
    std::vector<std::string> Value::getKeys() const {
        if(type != ValueType::Object) {
            throw std::runtime_error("Value is not an object");
        }
        std::vector<std::string> keys;
        for(const auto& pair : *object_value) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    size_t Value::size() const {
        if(type == ValueType::Array) {
            return array_value->size();
        } else if(type == ValueType::Object) {
            return object_value->size();
        } else if(type == ValueType::String) {
            return string_value->size();
        }
        return 0;
    }
    
    void Value::push_back(const Value& value) {
        if(type != ValueType::Array) {
            throw std::runtime_error("Value is not an array");
        }
        array_value->push_back(value);
    }
    
    std::string Value::toString(bool pretty, int indent) const {
        std::string result;
        std::string indentStr(pretty ? indent * 2 : 0, ' ');
        
        switch (type) {
            case ValueType::Null:
                result = "null";
                break;
            case ValueType::Boolean:
                result = bool_value ? "true" : "false";
                break;
            case ValueType::Number:
                if(std::isnan(number_value)) {
                    result = "null";
                } else if(std::isinf(number_value)) {
                    result = "null";
                } else {
                    double intPart;
                    if(std::modf(number_value, &intPart) == 0.0) {
                        result = std::to_string(static_cast<int>(number_value));
                    } else {
                        result = std::to_string(number_value);
                        while(!result.empty() && result.back() == '0') {
                            result.pop_back();
                        }
                        if(!result.empty() && result.back() == '.') {
                            result.pop_back();
                        }
                    }
                }
                break;
            case ValueType::String:
                result = "\"" + Parser::encodeString(*string_value) + "\"";
                break;
            case ValueType::Array: {
                result = "[";
                if(pretty && !array_value->empty()) {
                    result += "\n";
                }
                for(size_t i = 0; i < array_value->size(); ++i) {
                    if(pretty) {
                        result += std::string((indent + 1) * 2, ' ');
                    }
                    result += (*array_value)[i].toString(pretty, indent + 1);
                    if(i < array_value->size() - 1) {
                        result += ",";
                    }
                    if(pretty) {
                        result += "\n";
                    }
                }
                if(pretty && !array_value->empty()) {
                    result += indentStr;
                }
                result += "]";
                break;
            }
            case ValueType::Object: {
                result = "{";
                if(pretty && !object_value->empty()) {
                    result += "\n";
                }
                size_t i = 0;
                for(const auto& pair : *object_value) {
                    if(pretty) {
                        result += std::string((indent + 1) * 2, ' ');
                    }
                    result += "\"" + Parser::encodeString(pair.first) + "\":";
                    if(pretty) {
                        result += " ";
                    }
                    result += pair.second.toString(pretty, indent + 1);
                    if(++i < object_value->size()) {
                        result += ",";
                    }
                    if(pretty) {
                        result += "\n";
                    }
                }
                if(pretty && !object_value->empty()) {
                    result += indentStr;
                }
                result += "}";
                break;
            }
        }
        
        return result;
    }
    
    Value Parser::parse(const std::string& data) {
        size_t pos = 0;
        skipWhitespace(data, pos);
        Value result = parseValue(data, pos);
        skipWhitespace(data, pos);
        if(pos < data.length()) {
            throw ParseException("Unexpected character after data", pos);
        }
        
        return result;
    }
    
    Value Parser::parseFile(const std::string& filename) {
        std::string content = readFile(filename);
        return parse(content);
    }
    
    Value Parser::parseValue(const std::string& data, size_t& pos) {
        skipWhitespace(data, pos);
        if(pos >= data.length()) {
            throw ParseException("Unexpected end of data", pos);
        }
        
        char c = data[pos];
        if(c == '{') {
            return parseObject(data, pos);
        } else if(c == '[') {
            return parseArray(data, pos);
        } else if(c == '"') {
            return parseString(data, pos);
        } else if(c == '-' || (c >= '0' && c <= '9')) {
            return parseNumber(data, pos);
        } else if(c == 't' || c == 'f' || c == 'n') {
            return parseKeyword(data, pos);
        } else {
            throw ParseException("Unexpected character", pos);
        }
    }
    
    Value Parser::parseObject(const std::string& data, size_t& pos) {
        Value result(ValueType::Object);
        pos++;
        skipWhitespace(data, pos);
        if(data[pos] == '}') {
            pos++;
            return result;
        }
        
        while(pos < data.length()) {
            if(data[pos] != '"') {
                throw ParseException("Expected string key", pos);
            }
            Value key = parseString(data, pos);
            std::string keyStr = key.asString();
            
            skipWhitespace(data, pos);
            if(data[pos] != ':') {
                throw ParseException("Expected ':' after key", pos);
            }
            pos++;
            
            skipWhitespace(data, pos);
            Value value = parseValue(data, pos);
            result[keyStr] = value;
            
            skipWhitespace(data, pos);
            if(data[pos] == '}') {
                pos++;
                break;
            } else if(data[pos] == ',') {
                pos++;
                skipWhitespace(data, pos);
            } else {
                throw ParseException("Expected ',' or '}'", pos);
            }
        }
        
        return result;
    }
    
    Value Parser::parseArray(const std::string& data, size_t& pos) {
        Value result(ValueType::Array);
        pos++;
        skipWhitespace(data, pos);
        if(data[pos] == ']') {
            pos++;
            return result;
        }
        
        while(pos < data.length()) {
            Value value = parseValue(data, pos);
            result.push_back(value);
            
            skipWhitespace(data, pos);
            if(data[pos] == ']') {
                pos++;
                break;
            } else if(data[pos] == ',') {
                pos++;
                skipWhitespace(data, pos);
            } else {
                throw ParseException("Expected ',' or ']'", pos);
            }
        }
        
        return result;
    }
    
    Value Parser::parseString(const std::string& data, size_t& pos) {
        pos++;
        std::string result;
        
        while(pos < data.length()) {
            char c = data[pos++];
            
            if(c == '"') {
                return Value(decodeString(result));
            } else if(c == '\\') {
                if(pos >= data.length()) {
                    throw ParseException("Incomplete escape sequence", pos);
                }
                
                char esc = data[pos++];
                switch (esc) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        if(pos + 3 >= data.length()) {
                            throw ParseException("Incomplete Unicode escape", pos);
                        }
                        result += "\\u";
                        result += data.substr(pos, 4);
                        pos += 4;
                        break;
                    }
                    default:
                        throw ParseException("Invalid escape sequence", pos - 1);
                }
            } else {
                result += c;
            }
        }
        
        throw ParseException("Unterminated string", pos);
    }
    
    Value Parser::parseNumber(const std::string& data, size_t& pos) {
        size_t start = pos;
        if(data[pos] == '-') {
            pos++;
        }
        
        while(pos < data.length() && data[pos] >= '0' && data[pos] <= '9') {
            pos++;
        }
        if(pos < data.length() && data[pos] == '.') {
            pos++;
            while(pos < data.length() && data[pos] >= '0' && data[pos] <= '9') {
                pos++;
            }
        }
        if(pos < data.length() && (data[pos] == 'e' || data[pos] == 'E')) {
            pos++;
            if(pos < data.length() && (data[pos] == '+' || data[pos] == '-')) {
                pos++;
            }
            while(pos < data.length() && data[pos] >= '0' && data[pos] <= '9') {
                pos++;
            }
        }
        
        std::string numStr = data.substr(start, pos - start);
        try {
            double value = std::stod(numStr);
            return Value(value);
        } catch (...) {
            throw ParseException("Invalid number format", start);
        }
    }
    
    Value Parser::parseKeyword(const std::string& data, size_t& pos) {
        if(data.substr(pos, 4) == "true") {
            pos += 4;
            return Value(true);
        } else if(data.substr(pos, 5) == "false") {
            pos += 5;
            return Value(false);
        } else if(data.substr(pos, 4) == "null") {
            pos += 4;
            return Value();
        } else {
            throw ParseException("Unexpected keyword", pos);
        }
    }
    
    void Parser::skipWhitespace(const std::string& data, size_t& pos) {
        while(pos < data.length() && std::isspace(static_cast<unsigned char>(data[pos]))) {
            pos++;
        }
    }
    
    char Parser::getNextChar(const std::string& data, size_t& pos) {
        skipWhitespace(data, pos);
        if(pos >= data.length()) {
            throw ParseException("Unexpected end of data", pos);
        }
        return data[pos++];
    }
    
    std::string Parser::decodeString(const std::string& str) {
        std::string result;
        for(size_t i = 0; i < str.length(); ++i) {
            if(str[i] == '\\' && i + 1 < str.length()) {
                switch (str[++i]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        if(i + 4 < str.length()) {
                            result += "\\u" + str.substr(i + 1, 4);
                            i += 4;
                        }
                        break;
                    }
                    default:
                        result += str[i];
                        break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
    
    std::string Parser::encodeString(const std::string& str) {
        std::string result;
        for(char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if(static_cast<unsigned char>(c) < 0x20) {
                        char buf[7];
                        snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                        result += buf;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }

}