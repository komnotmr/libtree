#pragma once

#include <utility>

#include <json.hpp>

#include "statuses.hpp"

namespace tree
{
    struct result_t {
        status_t status;
        nlohmann::json payload;

        result_t() = default;

        result_t(status_t st_, nlohmann::json p_) : status(st_), payload(std::move(p_)) {}

        ~result_t() = default;

    };

    // using result_t = std::tuple<status_t, nlohmann::json>;

}
