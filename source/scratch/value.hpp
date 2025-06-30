#pragma once
#include <string>
#include <cmath>
#include <iostream>
#include "math.hpp"
#include <nlohmann/json.hpp>

enum class ValueType{
    INTEGER,
    DOUBLE,
    STRING
};

class Value{
private:
    ValueType type;
    union{
        int intValue;
        double doubleValue;
        std::string* stringValue;
    };
public:
    // constructors
    Value() : type(ValueType::INTEGER), intValue(0) {}

    explicit Value(int val) : type(ValueType::INTEGER), intValue(val) {}

    explicit Value(double val) : type(ValueType::DOUBLE), doubleValue(val) {}

    explicit Value(const std::string& val) : type(ValueType::STRING) {
        stringValue = new std::string(val);
    }
    // copy operator
    Value(const Value& other) : type(other.type){
        switch(type){
            case ValueType::INTEGER:
                intValue = other.intValue;
                break;
            case ValueType::DOUBLE:
                doubleValue = other.doubleValue;
                break;
            case ValueType::STRING:
                stringValue = new std::string(*other.stringValue);
                break;
        }
    }
    // Assignment operator
    Value& operator=(const Value& other){
        if(this != &other){
            if(type == ValueType::STRING){
                delete stringValue;
            }
            // copy new value
            type = other.type;
            switch(type){
                case ValueType::INTEGER:
                    intValue = other.intValue;
                    break;
                case ValueType::DOUBLE:
                    doubleValue = other.doubleValue;
                    break;
                case ValueType::STRING:
                    stringValue = new std::string(*other.stringValue);
                    break;
            }
        }
        return *this;
    }
    // destructor
    ~Value() {
        if (type == ValueType::STRING) {
            delete stringValue;
        }
    }
    // type checks
    bool isInteger() const { return type == ValueType::INTEGER; }
    bool isDouble() const { return type == ValueType::DOUBLE; }
    bool isString() const { return type == ValueType::STRING; }
    bool isNumeric() const { 
        return type == ValueType::INTEGER || type == ValueType::DOUBLE ||
               (type == ValueType::STRING && Math::isNumber(*stringValue));
    }

    double asDouble() const {
        switch(type){
            case ValueType::INTEGER:
                return static_cast<double>(intValue);
            case ValueType::DOUBLE:
                return doubleValue;
            case ValueType::STRING:
                return Math::isNumber(*stringValue) ? std:: stod(*stringValue) : 0.0;
        }
        return 0.0;
    }

    int asInt() const{
        switch(type){
            case ValueType::INTEGER:
                return intValue;
            case ValueType::DOUBLE:
                return static_cast<int>(std::round(doubleValue));
            case ValueType::STRING:
                if(Math::isNumber(*stringValue)){
                    double d = std::stod(*stringValue);
                    return static_cast<int>(std::round(d));
                }
        }
        return 0;
    }

    std::string asString() const{
        switch(type){
            case ValueType::INTEGER:
                return std::to_string(intValue);
            case ValueType::DOUBLE:{
                // handle whole numbers too, because scratch i guess
                if(std::floor(doubleValue) == doubleValue){
                    return std::to_string(static_cast<int>(doubleValue));
                }
                return std::to_string(doubleValue);
            }
            case ValueType::STRING:
                return *stringValue;
        }
        return "";
    }

    // Arithmetic operations
    Value operator+(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            // Both integers - keep as integer
            if (type == ValueType::INTEGER && other.type == ValueType::INTEGER) {
                return Value(intValue + other.intValue);
            }
            // At least one is double - return double
            return Value(asDouble() + other.asDouble());
        }
        // String concatenation
        return Value(asString() + other.asString());
    }
    
    Value operator-(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            if (type == ValueType::INTEGER && other.type == ValueType::INTEGER) {
                return Value(intValue - other.intValue);
            }
            return Value(asDouble() - other.asDouble());
        }
        return Value(0);
    }
    
    Value operator*(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            if (type == ValueType::INTEGER && other.type == ValueType::INTEGER) {
                return Value(intValue * other.intValue);
            }
            return Value(asDouble() * other.asDouble());
        }
        return Value(0);
    }
    
    Value operator/(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            double otherVal = other.asDouble();
            if (otherVal == 0.0) return Value(0);  // Division by zero
            return Value(asDouble() / otherVal);
        }
        return Value(0);
    }
    
    // Comparison operators
    bool operator==(const Value& other) const {
        if (type == other.type) {
            switch(type) {
                case ValueType::INTEGER: return intValue == other.intValue;
                case ValueType::DOUBLE: return doubleValue == other.doubleValue;
                case ValueType::STRING: return *stringValue == *other.stringValue;
            }
        }
        // Different types - compare as strings (Scratch behavior)
        return asString() == other.asString();
    }
    
    bool operator<(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            return asDouble() < other.asDouble();
        }
        return asString() < other.asString();
    }
    
    bool operator>(const Value& other) const {
        if (isNumeric() && other.isNumeric()) {
            return asDouble() > other.asDouble();
        }
        return asString() > other.asString();
    }

static Value fromJson(const nlohmann::json& jsonVal){
    if(jsonVal.is_null()) return Value(0);
    std::cout << "from json'ing " << jsonVal.dump() << std::endl; // Use dump() instead of get<string>()
    
    if(jsonVal.is_number_integer()){
        std::cout << "is an int! " << jsonVal.get<int>() << std::endl;
        return Value(jsonVal.get<int>());
    } else if(jsonVal.is_number_float()){
        std::cout << "is a FUCKING float! " << jsonVal.get<double>() << std::endl;
        return Value(jsonVal.get<double>());
    } else if(jsonVal.is_string()){
        std::cout << "is a string!.. " << jsonVal.get<std::string>() << std::endl;
        std::string strVal = jsonVal.get<std::string>();
        // try to parse as a number for better performance
        if(Math::isNumber(strVal)){
            std::cout << "jk ts is a double now " << std::stod(strVal) << std::endl;
            double numVal = std::stod(strVal);
            if(std::floor(numVal) == numVal){
                std::cout << "jk again its an int now " << static_cast<int>(numVal) << std::endl;
                return Value(static_cast<int>(numVal));
            }
            return Value(numVal);
        }
        return Value(strVal);
    } else if(jsonVal.is_boolean()){
        std::cout << "is a boolean! " << jsonVal.get<bool>() << std::endl;
        return Value(jsonVal.get<bool>() ? 1 : 0);
    } else if(jsonVal.is_array()){
        std::cout << "is an array! Cannot convert directly to Value" << std::endl;
        // Handle array case - maybe extract first element or return default
        if(jsonVal.size() > 1) {
            std::cout << "Using second element of array: " << jsonVal[1].dump() << std::endl;
            return fromJson(jsonVal[1]); // Recursively process second element
        }
        return Value(0);
    }
    
    std::cout << "um... nothing... type: " << jsonVal.type_name() << std::endl;
    return Value(0);
}

    ValueType getType() const { return type; }


};