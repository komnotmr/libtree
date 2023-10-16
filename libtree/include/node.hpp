#pragma once

#include <map>
#include <string>
#include <memory>

#include <json.hpp>

#include "privilege.hpp"
#include "method.hpp"
#include "iterator.hpp"
#include "result.hpp"

namespace tree
{

    constexpr auto error_message_should_be_array = "should be array";
    constexpr auto error_message_should_be_object = "should be object";
    constexpr auto error_message_should_be_string = "should be string";
    constexpr auto error_message_should_be_number_integer = "should be integer";
    constexpr auto error_message_methods_not_found = "method not found";
    constexpr auto error_message_duplicate_node = "node already exists";

    template <typename TPrivilege=privilege_t, typename TIterator=iterator_t>
    struct node_t
    {
        /* имя ноды */
        std::string name;
        /* список привилегий для узла, можно регулировать распространяется ли на детей */
        TPrivilege privileges;
        /* агрументы, которые подмешиваем ко всем методам, нужно регулировать, распространяется ли на детей */
        nlohmann::json common_args;
        /* методы из таблицы методов: get, set, remove, add, ...*/
        std::map<std::string, method_t> methods;
        /* для векторных полей: */
        TIterator iterator;
        /* дочерние узлы */
        std::map<std::string, std::shared_ptr<node_t>> children;

        /* методы */
        bool parse_privileges(nlohmann::json const &json_node, nlohmann::json &errors)
        {
            constexpr auto const n_privileges = "privileges";

            if (!json_node.contains(n_privileges))
                return true;

            auto const &privs = json_node[n_privileges];
            if (!privs.is_array())
            {
                errors[n_privileges] = error_message_should_be_array;
                return false;
            }

            for (int i = 0; i < privs.size(); i++)
            {
                auto const &item = privs[i];

                if (!item.is_object())
                {
                    errors[n_privileges + ("." + std::to_string(i))] = error_message_should_be_object;
                    continue;
                }

                constexpr auto const n_method = "method";
                if (!item.contains(n_method))
                {
                    errors[n_privileges + ("." + std::to_string(i) + ".") + n_method] = error_message_should_be_object;
                    continue;
                }
                else if (!item[n_method].is_string())
                {
                    errors[n_privileges + ("." + std::to_string(i) + ".") + n_method] = error_message_should_be_string;
                    continue;
                }
            // TODO
            //     constexpr auto const n_level = "level";
            //     if (!item.contains(n_level))
            //     {
            //         errors[n_privileges + ("." + std::to_string(i) + ".") + n_level] = error_message_should_be_object;
            //         continue;
            //     }


            //     if (!item[n_level].is_number_integer())
            //     {
            //         if (!item[n_level].is_string()) {
            //             errors[n_privileges + ("." + std::to_string(i) + ".") + n_level] = error_message_should_be_number_integer;
            //             continue;
            //         } else {
            //             this->privileges.method_name = 
            //         }
            //     } else {
            //             this->privileges.method_name = 
            //     }
            }

            return true;
        }

        bool parse_common_args(nlohmann::json const &json_node, nlohmann::json &errors)
        {
            constexpr auto const n_common_args = "common_args";

            if (!json_node.contains(n_common_args))
                return true;

            auto const &common_args = json_node[n_common_args];

            if (!common_args.is_object())
            {
                errors[n_common_args] = error_message_should_be_object;
                return false;
            }

            this->common_args = common_args;
            return true;
        }

        bool parse_methods(nlohmann::json const &json_node, methods_map_t const &methods_map, nlohmann::json &errors)
        {
            constexpr auto const n_methods = "methods";

            if (!json_node.contains(n_methods))
                return true;

            auto const &methods = json_node[n_methods];

            if (!methods.is_object())
            {
                errors[n_methods] = error_message_should_be_object;
                return false;
            }

            bool was_error_in_methods = false;

            for (auto &item : methods.items())
            {
                auto const &key = item.key();

                if (!item.value().is_string())
                {
                    errors[n_methods + ("." + key)] = error_message_should_be_string;
                    was_error_in_methods = true;
                    continue;
                }

                auto const method_it = methods_map.find(item.value());
                if (method_it == methods_map.end())
                {
                    errors[n_methods + ("." + key)] = error_message_methods_not_found + (": " + static_cast<std::string>(item.value()));
                    was_error_in_methods = true;
                    continue;
                }

                this->methods[key] = method_it->second;
            }

            return !was_error_in_methods;
        }

        bool parse_children(nlohmann::json const &json_node, nlohmann::json &errors)
        {
            constexpr auto const n_children = "children";

            if (!json_node.contains(n_children))
                return true;

            auto const &children = json_node[n_children];

            if (!children.is_object())
            {
                errors[n_children] = error_message_should_be_object;
                return false;
            }

            for (auto const &item : children.items())
            {
                auto node = std::make_shared<node_t>();
                node->name = item.key();
                if (this->children.find(node->name) != this->children.end())
                {
                    errors[n_children] = error_message_duplicate_node + (": " + node->name);
                    return false;
                }
                this->children[node->name] = node;
            }

            return true;
        }

        bool parse_iterator(nlohmann::json const &json_node, nlohmann::json &errors)
        {
            constexpr auto const n_iterator = "iterator";

            if (!json_node.contains(n_iterator))
                return true;

            auto const &iterator = json_node[n_iterator];

            if (!iterator.is_object())
            {
                errors[n_iterator] = error_message_should_be_object;
                return false;
            }

            constexpr auto const n_from = "from";
            if (!iterator.contains(n_from) || !iterator[n_from].is_number_integer())
            {
                errors[n_iterator + static_cast<std::string>(".") + n_from] = error_message_should_be_number_integer;
                return false;
            }

            constexpr auto const n_to = "to";
            if (!iterator.contains(n_to) || !iterator[n_to].is_number_integer())
            {
                errors[n_iterator + static_cast<std::string>(".") + n_to] = error_message_should_be_number_integer;
                return false;
            }

            this->iterator.range.to = iterator[n_to];
            this->iterator.range.from = iterator[n_from];
            this->iterator.mode = iteration_mode_t::iterate_before_first_error;

            constexpr auto const n_mode = "mode";
            const auto error_message = error_message_should_be_string + static_cast<std::string>(": iterate_all | iterate_before_first_error | iterate_all_ignore_errors");
            if (iterator.contains(n_mode))
            {
                if (!iterator[n_mode].is_string())
                {
                    errors[n_iterator + static_cast<std::string>(".") + n_mode] = error_message;
                    return false;
                }
                std::string mode = iterator[n_mode];
                if (mode == "iterate_all")
                {
                    this->iterator.mode = iteration_mode_t::iterate_all;
                }
                else if (mode == "iterate_before_first_error")
                {
                    this->iterator.mode = iteration_mode_t::iterate_before_first_error;
                }
                else if (mode == "iterate_all_ignore_errors")
                {
                    this->iterator.mode = iteration_mode_t::iterate_all_ignore_errors;
                }
                else
                {
                    errors[n_iterator + static_cast<std::string>(".") + n_mode] = error_message;
                    return false;
                }
            }

            return true;
        }

        result_t parse_from_json(nlohmann::json const &json_node, methods_map_t const &methods_map)
        {
            nlohmann::json errors = {};

            auto res_privileges = this->parse_privileges(json_node, errors);
            auto res_common_args = this->parse_common_args(json_node, errors);
            auto res_methods = this->parse_methods(json_node, methods_map, errors);
            auto res_iterator = this->parse_iterator(json_node, errors);
            auto res_children = this->parse_children(json_node, errors);

            auto is_only_children_node = !(
                res_privileges &&
                res_common_args &&
                res_methods &&
                res_iterator);

            auto is_valid_node = (is_only_children_node && res_children) || !is_only_children_node;

            if (!is_valid_node)
            {
                if (!this->children.size())
                {
                    return result_t{status_t::parse_tree_failed, {
                                                                     {this->name, "empty node"},
                                                                     {"priveleges", res_privileges},
                                                                     {"common_args", res_common_args},
                                                                     {"methods", res_methods},
                                                                     {"iterator", res_iterator},
                                                                     {"children", res_children},
                                                                 }};
                }
            }

            return result_t{status_t::ok, errors};
        }

        result_t call(const std::string method_name, nlohmann::json const &args)
        {
            auto method_it = this->methods.find(method_name);
            if (method_it == this->methods.end())
            {
                return result_t{status_t::method_not_found, nlohmann::json{{"method_name", method_name}}};
            }

            return method_it->second(args);
        }
    };

}
