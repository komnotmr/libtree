#pragma once

#include <map>
#include <stack>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include <json.hpp>

#include "node.hpp"
#include "method.hpp"
#include "result.hpp"
#include "context.hpp"
#include "iterator.hpp"
#include "statuses.hpp"
#include "privilege.hpp"

namespace tree
{

    template <typename TContext = context_t, typename TNode = node_t<privilege_t, iterator_t>>
    class Tree
    {
        struct stack_item_t
        {
            std::reference_wrapper<nlohmann::json const> json_node;
            std::shared_ptr<TNode> node;
            std::vector<std::string> path;

            stack_item_t(
                std::reference_wrapper<nlohmann::json const> json_node_,
                std::shared_ptr<TNode> node_,
                std::vector<std::string> path_) : json_node(json_node_), node(node_), path(std::move(path_))
            {
                path.push_back(node_->name);
            }

            ~stack_item_t() {}

            std::string getPath()
            {
                std::string res = path[0];
                for (int i = 1; i < path.size(); i++)
                {
                    res += "." + path[i];
                }

                return res;
            }
        };

    public:
        Tree() = default;

        ~Tree() = default;

        result_t init(const std::string path_to_json, methods_map_t const &methods_map)
        {
            std::ifstream file(path_to_json);
            if (!file.is_open())
            {
                return result_t{status_t::json_not_found, {{"path_to_json", path_to_json}}};
            }

            try
            {
                this->methods_ = methods_map; // нужно ли здесь копирование?
                auto tree = nlohmann::json::parse(file);
                auto res = constructJsonToTree_(tree);
                if (res.status != status_t::ok)
                    return res;
            }
            catch (const nlohmann::json::exception &e)
            {
                return result_t{status_t::invalid_json, {{"exception", e.what()}}};
            }
            catch (...)
            {
                return result_t{status_t::json_load_failed, {{"path_to_json", path_to_json}}};
            }

            return result_t{status_t::ok, nullptr};
        }

        result_t getNode(const std::string path_to_node)
        {
            return result_t{status_t::ok, {{"path", path_to_node}}};
        }

        result_t getNode(const std::vector<std::string> array_of_pathes)
        {
            return result_t{status_t::ok, {{"path", array_of_pathes}}};
        }

        std::shared_ptr<TNode> getRoot()
        {
            return this->root_;
        }

        result_t query(TContext &ctx)
        {
            return result_t{status_t::ok, nullptr};
        }

        result_t query(TContext &ctx, nlohmann::json const &payload)
        {
            return result_t{status_t::ok, nullptr};
        }

    private:
        std::shared_ptr<TNode> root_;
        methods_map_t methods_; // TODO приходит извне

        void merge_common_args(nlohmann::json const &source, nlohmann::json &target)
        {
            for (auto const &item : source.items())
            {
                if (target.contains(item.key()))
                {
                    // TODO warning, rewrite common_arg
                }
                target[item.key()] = item.value();
            }
        }
        bool check_privileges(TContext &ctx, privilege_array_t const &privs)
        {
            if (!privs.size())
                return true;

            // TODO
            return false;
        }

        result_t constructJsonToTree_(nlohmann::json const &json_tree)
        {
            this->root_ = std::make_shared<TNode>();
            this->root_->name = "root";

            std::stack<stack_item_t> stack;
            stack.push(stack_item_t{std::ref(json_tree), this->root_, {}});

            while (!stack.empty())
            {
                auto stack_elem = stack.top();
                auto const &json_node = stack_elem.json_node.get();
                auto tree_node = stack_elem.node;
                stack.pop();

                auto res = tree_node->parse_from_json(json_node, this->methods_);

                if (res.status != status_t::ok)
                {
                    res.payload["path"] = stack_elem.getPath();
                    return res;
                }

                if (!json_node.contains("children"))
                {
                    continue;
                }

                for (auto const &item : json_node["children"].items())
                {
                    stack.push(stack_item_t{std::ref(item.value()), tree_node->children[item.key()], stack_elem.path});
                }
            }

            return {status_t::ok, nullptr};
        }

        std::vector<std::string> splitPath_(const std::string path_to_node)
        {
            std::stringstream test(path_to_node);
            std::string segment;
            std::vector<std::string> seglist;

            seglist.push_back("root");

            while (std::getline(test, segment, '.'))
            {
                seglist.push_back(segment);
            }

            return seglist;
        }

        result_t traverseByStringPath_(TContext &ctx, const std::string method_name, const std::string path_to_node)
        {

            auto path = this->splitPath_(path_to_node);
            auto node = this->root_;

            std::vector<std::string> path_history = {};

            for (auto const &path_item : path)
            {

                path_history.push_back(path_item);
                /* node not found */
                if (node->name != path_item)
                {
                    return result_t{status_t::node_not_found, {{"path", path_history}}};
                }
                // проверяем привелегии по пути
                this->check_privileges(ctx, node->privileges);
                // подмешали common_args в свойства
                this->merge_common_args(node->common_args, ctx.args);
                // TODO выполняем действия итератора, если такой есть

                node->call(method_name, ctx.payload);
                

                // проверяем есть ли такой метод у ноды
                // если нет, то возвращаем ошибку
                // если есть, то выполняем метод
            }

            return {};
        }
    };

}
