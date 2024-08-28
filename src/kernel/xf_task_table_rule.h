/**
 * @file xf_task_table_rule.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 内部任务注册规则。
 * @version 0.1
 * @date 2024-07-06
 * @note 此文件不需要防止重复包含。
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 * @details 用法：
 * 在包含本文件前定义 `XF_HAL_TABLE_TYPE` 或 `XF_HAL_TABLE_STR` 可以
 * 生成枚举值或生成字符串。
 */

/* ==================== [Includes] ========================================== */

#include "../xf_task_config_internal.h"

/* ==================== [Defines] =========================================== */

#ifdef XF_TASK_REG
#undef XF_TASK_REG
#endif

#ifdef XF_TASK_REG_ENUM
#define XF_TASK_REG(name)         XF_TASK_TYPE_##name,
#endif

#ifdef XF_TASK_REG_EXTERN
#define XF_TASK_REG(name)        extern void xf_##name##_vfunc_register(void);
#endif

#ifdef XF_TASK_REG_FUNCTION
#define XF_TASK_REG(name)        xf_##name##_vfunc_register();
#endif

#undef XF_TASK_REG_ENUM
#undef XF_TASK_REG_EXTERN
#undef XF_TASK_REG_FUNCTION

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */
