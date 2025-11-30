#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

namespace nlohmann {

class json {
private:
    enum class value_t { null, object, array, string, number, boolean };
    value_t type;
    
    std::map<std::string, json>* object_value;
    std::vector<json>* array_value;
    std::string* string_value;
    int* number_value;
    bool* bool_value;
    
    void clear() {
        if (object_value) delete object_value;
        if (array_value) delete array_value;
        if (string_value) delete string_value;
        if (number_value) delete number_value;
        if (bool_value) delete bool_value;
        object_value = nullptr;
        array_value = nullptr;
        string_value = nullptr;
        number_value = nullptr;
        bool_value = nullptr;
    }

public:
    json() : type(value_t::null), object_value(nullptr), array_value(nullptr), 
             string_value(nullptr), number_value(nullptr), bool_value(nullptr) {}
    
    json(const json& other) : type(other.type), object_value(nullptr), array_value(nullptr),
                              string_value(nullptr), number_value(nullptr), bool_value(nullptr) {
        if (other.object_value) object_value = new std::map<std::string, json>(*other.object_value);
        if (other.array_value) array_value = new std::vector<json>(*other.array_value);
        if (other.string_value) string_value = new std::string(*other.string_value);
        if (other.number_value) number_value = new int(*other.number_value);
        if (other.bool_value) bool_value = new bool(*other.bool_value);
    }
    
    json& operator=(const json& other) {
        if (this != &other) {
            clear();
            type = other.type;
            if (other.object_value) object_value = new std::map<std::string, json>(*other.object_value);
            if (other.array_value) array_value = new std::vector<json>(*other.array_value);
            if (other.string_value) string_value = new std::string(*other.string_value);
            if (other.number_value) number_value = new int(*other.number_value);
            if (other.bool_value) bool_value = new bool(*other.bool_value);
        }
        return *this;
    }
    
    ~json() { clear(); }
    
    static json array() {
        json j;
        j.type = value_t::array;
        j.array_value = new std::vector<json>();
        return j;
    }
    
    static json object() {
        json j;
        j.type = value_t::object;
        j.object_value = new std::map<std::string, json>();
        return j;
    }
    
    json& operator[](const std::string& key) {
        if (type == value_t::null) {
            type = value_t::object;
            object_value = new std::map<std::string, json>();
        }
        return (*object_value)[key];
    }
    
    json& operator=(const std::string& value) {
        clear();
        type = value_t::string;
        string_value = new std::string(value);
        return *this;
    }
    
    json& operator=(int value) {
        clear();
        type = value_t::number;
        number_value = new int(value);
        return *this;
    }
    
    // Conversión a string
    operator std::string() const {
        if (type == value_t::string && string_value) {
            return *string_value;
        }
        return "";
    }
    
    // Método para obtener string
    std::string get_string() const {
        if (type == value_t::string && string_value) {
            return *string_value;
        }
        return "";
    }
    
    void push_back(const json& value) {
        if (type == value_t::null) {
            type = value_t::array;
            array_value = new std::vector<json>();
        }
        array_value->push_back(value);
    }
    
    bool contains(const std::string& key) const {
        if (type != value_t::object || !object_value) return false;
        return object_value->find(key) != object_value->end();
    }
    
    size_t size() const {
        if (type == value_t::array && array_value) return array_value->size();
        if (type == value_t::object && object_value) return object_value->size();
        return 0;
    }
    
    std::vector<json>::iterator begin() { 
        if (array_value) return array_value->begin();
        static std::vector<json> empty;
        return empty.begin(); 
    }
    
    std::vector<json>::iterator end() { 
        if (array_value) return array_value->end();
        static std::vector<json> empty;
        return empty.end(); 
    }
    
    std::vector<json>::const_iterator begin() const { 
        if (array_value) return array_value->begin();
        static std::vector<json> empty;
        return empty.begin(); 
    }
    
    std::vector<json>::const_iterator end() const { 
        if (array_value) return array_value->end();
        static std::vector<json> empty;
        return empty.end(); 
    }
    
    std::string dump(int indent = -1) const {
        std::ostringstream oss;
        dump_internal(oss, indent, 0);
        return oss.str();
    }

private:
    void dump_internal(std::ostringstream& oss, int indent, int level) const {
        std::string ind = (indent > 0) ? std::string(level * indent, ' ') : "";
        std::string nl = (indent > 0) ? "\n" : "";
        
        switch (type) {
            case value_t::null:
                oss << "null";
                break;
            case value_t::string:
                oss << "\"" << *string_value << "\"";
                break;
            case value_t::number:
                oss << *number_value;
                break;
            case value_t::boolean:
                oss << (*bool_value ? "true" : "false");
                break;
            case value_t::array:
                oss << "[" << nl;
                for (size_t i = 0; i < array_value->size(); ++i) {
                    if (indent > 0) oss << std::string((level + 1) * indent, ' ');
                    (*array_value)[i].dump_internal(oss, indent, level + 1);
                    if (i < array_value->size() - 1) oss << ",";
                    oss << nl;
                }
                oss << ind << "]";
                break;
            case value_t::object:
                oss << "{" << nl;
                size_t count = 0;
                for (const auto& [key, value] : *object_value) {
                    if (indent > 0) oss << std::string((level + 1) * indent, ' ');
                    oss << "\"" << key << "\": ";
                    value.dump_internal(oss, indent, level + 1);
                    if (++count < object_value->size()) oss << ",";
                    oss << nl;
                }
                oss << ind << "}";
                break;
        }
    }
};

} // namespace nlohmann

#endif // NLOHMANN_JSON_HPP
