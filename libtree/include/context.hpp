#pragma once

#include <string>

#include <json.hpp>

#include "privilege.hpp"

namespace tree
{
    struct context_t
    {

        privilege_level_t privilege;

        std::string pathToNode;

        std::string method;

        nlohmann::json payload;

        nlohmann::json args;

        context_t(privilege_level_t p_, std::string pn_, std::string m_, nlohmann::json pl_)
            : privilege(p_),
            pathToNode(pn_),
            method(m_),
            payload(std::move(pl_))
        {}

        context_t(privilege_level_t p_, std::string m_, nlohmann::json pl_) : context_t(p_, m_, "", pl_) {}

        context_t(privilege_level_t p_, std::string m_) : context_t(p_, m_, "", {}) {}


        bool isPathHasSet() {
            return this->pathToNode.size();
        }

        ~context_t() {}
    };
}
