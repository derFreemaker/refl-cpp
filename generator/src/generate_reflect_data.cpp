#include "generate_reflect_data.hpp"

#include <filesystem>
#include <fstream>
#include <map>

#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

namespace ReflCpp {

void GenerateReflectionCode(const std::string& output_dir, const std::vector<ReflectData>& data) {
    std::filesystem::create_directories(output_dir);
    
    std::map<std::string, std::vector<const ReflectData*>> file_to_types;
    for (const auto& type : data) {
        if (type.source_file.empty()) continue;
        
        std::string filename = llvm::sys::path::filename(type.source_file).str();
        const size_t dot_pos = filename.find_last_of('.');
        if (dot_pos != std::string::npos) {
            filename = filename.substr(0, dot_pos);
        }
        filename += ".reflect-generated.cpp";
        
        file_to_types[filename].push_back(&type);
    }
    
    // Generate a file for each source file
    for (const auto& [filename, types] : file_to_types) {
        std::string output_path = std::format("%s/%s", output_dir, filename);
        std::ofstream output_file(output_path);
        
        // Write header
        output_file << "#include \"reflect.h\"\n";
        
        // Include original header files (in a real implementation, this would be more sophisticated)
        for (const auto* type : types) {
            output_file << "#include \"" << llvm::sys::path::filename(type->source_file).str() << "\"\n";
        }

        output_file << "\n";

        output_file << "namespace ReflCpp {";
        
        // Generate TypeData for each reflected class in this file
        for (const auto* type : types) {
            // Create the specialized template function
            output_file << "TypeData ReflectData<" << type->qualified_name << ">::Create() {\n";
            output_file << "{\n";
            output_file << "    .name = \"" << type->name << "\",\n";
            output_file << "    ._namespace = \"" << type->_namespace << "\",\n";
            
            // Fields
            output_file << "    .fields = {\n";
            for (const auto& field : type->fields) {
                output_file << "        FieldData {\n";
                output_file << "            .ptr = &" << type->qualified_name << "::" << field.name << ",\n";
                output_file << "            .name = \"" << field.name << "\",\n";
                output_file << "        },\n";
            }
            output_file << "    },\n";
            
            // Methods
            // output_file << "    .methods = {\n";
            // for (const auto& method : type->methods) {
            //     output_file << "        MethodData {\n";
            //     output_file << "            .ptr = &" << type->qualified_name << "::" << method.name << ",\n";
            //     output_file << "            .name = \"" << method.name << "\",\n";
            //     
            //     // Method arguments
            //     if (!method.arg_names.empty()) {
            //         output_file << "                .arguments = {\n";
            //         for (const auto& arg : method.arg_names) {
            //             output_file << "                    \"" << arg << "\",\n";
            //         }
            //         output_file << "                },\n";
            //     }
            //     
            //     output_file << "            },\n";
            // }
            // output_file << "        },\n";
            
            output_file << "    };\n";
            output_file << "}\n";
        }
        
        output_file.close();
        llvm::outs() << "Generated: " << output_path << "\n";
    }
}

}
