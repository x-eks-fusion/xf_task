/**
 * @file xf_task_default.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-06-21
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_task_default.h"
#include "xf_ctask.h"
#include "xf_ntask.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static xf_task_manager_t default_manager = NULL;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_task_manager_default_init(xf_task_on_idle_t on_idle)
{
    default_manager = xf_task_manager_create(on_idle);
    if (default_manager == NULL) {
        return XF_FAIL;
    }
    return XF_OK;
}

xf_err_t xf_task_manager_set_default_idle(xf_task_on_idle_t on_idle)
{
    return xf_task_manager_set_idle(default_manager, on_idle); 
}

xf_task_manager_t xf_task_get_default_manager(void)
{
    return default_manager;
}

void xf_task_manager_run_default(void)
{
    xf_task_manager_run(default_manager);
}

xf_err_t xf_task_set_urgent_task(xf_task_t task, bool force)
{
    return xf_task_set_urgent_task_with_manager(default_manager, task, force);
}

xf_task_t xf_task_create(xf_task_type_t type, xf_task_func_t func, void *func_arg, uint16_t priority, void *config)
{
    return xf_task_create_with_manager(default_manager, type, func, func_arg, priority, config);
}

/* ==================== [Static Functions] ================================== */
