/**
 * @file xf_task_reg.inc
 * @author cangyu (sky.kirto@qq.com)
 * @brief xf_task 的任务注册表。
 * @version 1.0
 * @date 2024-08-08
 * @note 此文件不需要防止重复包含。
 *
 * Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#include "../kernel/xf_task_table_rule.h"

XF_TASK_REG(ntask)

#if XF_TASK_CONTEXT_IS_ENABLE 
XF_TASK_REG(ctask)
#endif // XF_TASK_CONTEXT_IS_ENABLE
