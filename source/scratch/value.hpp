#pragma once
#include <string>
#include <cmath>

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
    Value() : type(ValueType::INTEGER), intValue(0) {}

    explicit Value(int val) : type(ValueType::INTEGER), intValue(val) {}

    explicit Value(double val) : type(ValueType::DOUBLE), doubleValue(val) {}

    explicit Value(std::string& val) : type(ValueType::STRING) {
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
        }
    }
};