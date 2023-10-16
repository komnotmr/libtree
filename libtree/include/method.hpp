#pragma once

#include <map>
#include <string>
#include <functional>

#include <json.hpp>

#include "result.hpp"

namespace tree
{

    using method_t = std::function<result_t(nlohmann::json)>;

    using methods_map_t = std::map<std::string, method_t>;

}
