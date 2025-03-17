#pragma once

#include <string>
#include <vector>

namespace ReflCpp {

struct ReflectMethodData {
    std::string qualified_name;

    std::string name;
    std::vector<std::string> arg_names;
};

struct ReflectFieldData {
    std::string qualified_name;

    std::string name;
};

struct ReflectData {
    std::string qualified_name;
    std::string source_file;
    
    std::string name;
    std::string _namespace;
    
    std::vector<std::string> bases;
    std::vector<std::string> inners;

    std::vector<ReflectFieldData> fields;
    std::vector<ReflectMethodData> methods;
};

}