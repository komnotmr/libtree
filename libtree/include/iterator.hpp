#pragma once

#include <cstdint>

namespace tree
{

    enum class iteration_mode_t
    {
        /* пройтись по всем элементам (статические массивы) */
        iterate_all,
        /* пройтись по всем доступным до первой ошибки (динамические списки) */
        iterate_before_first_error,
        /* пройтись по ВСЕМ доступным, игнорируя ошибки (массивы с дырками)*/
        iterate_all_ignore_errors
    };

    struct iterate_range_t
    {
        uint32_t from;
        uint32_t to;
    };

    struct iterator_t
    {
        /* задаёт рендж для перебора, подмешивает индекс в каждую ноду */
        iterate_range_t range;
        /* режим прохода по элементам */
        iteration_mode_t mode;
    };
}
