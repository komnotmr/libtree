#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace tree
{

    enum privilege_level_t
    {
        p_0,
        p_1,
        p_2,
        p_3,
        p_4,
        p_5,
        p_6,
        p_7,
        p_8,
        p_9,
        p_10,
        p_11,
        p_12,
        p_13,
        p_14,
        p_15
    };

    struct privilege_t
    {
        privilege_level_t level;
        std::vector<std::string> methodNames;
        /* method for get privilege */
        // TODO make parse in node.hpp
        // there are two cases:
        // 1. "level": int - just level for node
        // 2. "level": string - function name for getting privs for that node
        std::string method_name; 
    };

    using privilege_array_t = std::vector<privilege_t>;

}
