/**
 * @file  osal_lock.c
 * @brief 线程互斥锁相关API
 * @author  <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal.h>
#include <osal_lock.h>

/* 线程互斥锁模块的版本号, 代码有修改时必须修改 */
#define OSAL_LOCK_MODULE_VERSION  ("1.2.0")

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Get_Module_Version
 *         获取LOCK模块的当前版本
 *
 * @param[in/out]   major_version  模块主版本号返回指针，需要外部输入有效指针
 * @param[in/out]   minor_version  模块子版本号返回指针，需要外部输入有效指针
 * @param[in/out]   build_version  模块编译版本号返回指针，需要外部输入有效指针
 *
 * @return
 *          0 -> 版本号获取成功
 *          1 -> 参数错误，输入指针非法
 *          2 -> 版本号格式错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* 版本打印，便于调试 */
    LOGI(MODULE_LOCK, "%s: %s\n", __func__, OSAL_LOCK_MODULE_VERSION);

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* 版本返回，便于上层应用通过版本号判断，实现差异化兼容处理 */
    ret = sscanf(OSAL_LOCK_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_LOCK, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Init
 *         初始化线程互斥锁
 *
 * @param[in/out]   handle  互斥锁创建成功后返回的句柄指针,
 *                          需要外部传入有效指针，不能为NULL
 *
 * @return
 *          0 -> 初始化成功
 *          1 -> 参数错误
 *          2 -> 内存不足
 *          3 -> pthread_mutex_init调用失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Init(OS_HANDLE *handle)
{
    OS_HANDLE mutex_handle = NULL;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* 分配互斥锁资源 */
    mutex_handle = malloc(sizeof(pthread_mutex_t));
    OSAL_CHECK_ERROR(MODULE_LOCK, (!mutex_handle), 2, return_err, "Malloc %d bytes fail! (Error: %p)\n",
                     sizeof(pthread_mutex_t), mutex_handle);

    /* 初始化线程互斥锁 */
    ret = pthread_mutex_init((pthread_mutex_t*) mutex_handle, NULL);
    OSAL_CHECK_ERROR(MODULE_LOCK, (ret != 0), 3, return_err, "Call pthread_mutex_init fail! (Error: %d)\n", ret);

    /* 输出有效互斥锁句柄 */
    *handle = mutex_handle;
    return 0;

return_err:
    /* 释放资源 */
    if (mutex_handle) {
        free(mutex_handle);
    }

    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Deinit
 *         反初始化线程互斥锁
 *
 * @param[in]  handle  需要反初始化的互斥锁句柄， 参考OSAL_LOCK_Init
 *
 * @return
 *          0 -> 反初始化成功
 *          1 -> 参数错误
 *          2 -> pthread_mutex_destroy调用失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Deinit(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    pthread_mutex_lock((pthread_mutex_t*)handle);
    pthread_mutex_unlock((pthread_mutex_t*)handle);

    /* 销毁互斥锁 */
    ret = pthread_mutex_destroy((pthread_mutex_t*)handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (ret != 0), 2, return_err, "Call pthread_mutex_destroy fail! (Error: %d)\n", ret);
    free(handle);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Up
 *         线程互斥锁加锁
 *
 * @param[in]  handle  需要加锁的互斥锁句柄， 参考OSAL_LOCK_Init
 *
 * @return
 *          0 -> 加锁成功
 *          1 -> 参数错误
 *          2 -> 加锁失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Up(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* 互斥锁加锁 */
    ret = pthread_mutex_lock((pthread_mutex_t*) handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (0 != ret), 2, return_err, "Call pthread_mutex_lock fail! (Error: %d)\n", ret);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_LOCK_Down
 *         线程互斥锁解锁
 *
 * @param[in]  handle  需要解锁的互斥锁句柄， 参考OSAL_LOCK_Init
 *
 * @return
 *          0 -> 解锁成功
 *          1 -> 参数错误
 *          2 -> 解锁失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_LOCK_Down(OS_HANDLE handle)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_LOCK, (!handle), 1, return_err, "Invalid argument! (%p)\n", handle);

    /* 互斥锁解锁 */
    ret = pthread_mutex_unlock((pthread_mutex_t*) handle);
    OSAL_CHECK_ERROR(MODULE_LOCK, (0 != ret), 2, return_err, "Call pthread_mutex_unlock fail! (Error: %d)\n", ret);

    return 0;

return_err:
    return (MODULE_LOCK | ret);
}

