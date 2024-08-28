/**
 * @file xf_task_config.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-19
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __XF_TASK_CONFIG_H__
#define __XF_TASK_CONFIG_H__

#define USE_GNU_UC  0

#if USE_GNU_UC
#include <ucontext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif



#define XF_TASK_CONTEXT_DISABLE 0

#define XF_TASK_HUNGER_ENABLE 0

#if USE_GNU_UC
#define XF_TASK_CONTEXT_TYPE ucontext_t
#else
#define XF_TASK_CONTEXT_TYPE void*
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __XF_TASK_CONFIG_H__
