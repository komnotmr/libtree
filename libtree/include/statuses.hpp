#pragma once

namespace tree
{
    enum status_t
    { // TODO если делать class, нужно перегрузить ostream
        ok,
        json_not_found,
        json_load_failed,
        parse_tree_failed,
        invalid_json,
        node_not_found,
        method_not_found,
        execution_method_failed,
        permission_for_privilege_denied,
    };
}
