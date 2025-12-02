#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace DataParser {
    enum class ValueType {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    class Value {
        public:
            Value() : type(ValueType::Null) {}
            Value(bool b) : type(ValueType::Boolean), bool_value(b) {}
            Value(double d) : type(ValueType::Number), number_value(d) {}
            Value(const std::string& s) : type(ValueType::String), string_value(new std::string(s)) {}
            Value(ValueType t) : type(t) {
                switch (t) {
                    case ValueType::String:
                        string_value = new std::string();
                        break;
                    case ValueType::Array:
                        array_value = new std::vector<Value>();
                        break;
                    case ValueType::Object:
                        object_value = new std::unordered_map<std::string, Value>();
                        break;
                    default:
                        break;
                }
            }
            Value(std::initializer_list<std::pair<std::string, Value>> init);
            Value(std::initializer_list<Value> init);
            
            ~Value();
            Value(const Value& other);
            Value& operator=(const Value& other);
            Value(Value&& other) noexcept;
            Value& operator=(Value&& other) noexcept;
            
            ValueType getType() const { return type; }
            
            bool isNull() const { return type == ValueType::Null; }
            bool isBoolean() const { return type == ValueType::Boolean; }
            bool isNumber() const { return type == ValueType::Number; }
            bool isString() const { return type == ValueType::String; }
            bool isArray() const { return type == ValueType::Array; }
            bool isObject() const { return type == ValueType::Object; }
            
            bool asBoolean() const;
            double asNumber() const;
            int asInt() const;
            float asFloat() const;
            const std::string& asString() const;
            const std::vector<Value>& asArray() const;
            const std::unordered_map<std::string, Value>& asObject() const;
            
            Value& operator[](size_t index);
            const Value& operator[](size_t index) const;
            
            Value& operator[](const std::string& key);
            const Value& operator[](const std::string& key) const;
            
            bool hasKey(const std::string& key) const;
            std::vector<std::string> getKeys() const;
            
            size_t size() const;
            void push_back(const Value& value);
            
            std::string toString(bool pretty = false, int indent = 0) const;
            
        private:
            ValueType type;
            union {
                bool bool_value;
                double number_value;
                std::string* string_value;
                std::vector<Value>* array_value;
                std::unordered_map<std::string, Value>* object_value;
            };
            
            void cleanup();
            void copyFrom(const Value& other);
    };

    class Parser {
        public:
            static Value parse(const std::string& json);
            static Value parseFile(const std::string& filename);
            
            static std::string decodeString(const std::string& str);
            static std::string encodeString(const std::string& str);
        private:
            static Value parseValue(const std::string& json, size_t& pos);
            static Value parseObject(const std::string& json, size_t& pos);
            static Value parseArray(const std::string& json, size_t& pos);
            static Value parseString(const std::string& json, size_t& pos);
            static Value parseNumber(const std::string& json, size_t& pos);
            static Value parseKeyword(const std::string& json, size_t& pos);
            
            static void skipWhitespace(const std::string& json, size_t& pos);
            static char getNextChar(const std::string& json, size_t& pos);
    };
    class ParseException : public std::runtime_error {
        public:
            ParseException(const std::string& msg, size_t position)
                : std::runtime_error(msg + " at position " + std::to_string(position)),
                position(position) {}
            
            size_t getPosition() const { return position; }
            
        private:
            size_t position;
    };
}