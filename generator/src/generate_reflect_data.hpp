#pragma once

#include <string>
#include <vector>

#include "reflect_data.hpp"

namespace ReflCpp {

void GenerateReflectionCode(const std::string& output_dir, const std::vector<ReflectData>& data);

}
