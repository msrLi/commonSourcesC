/**
 * @file osal_lock.h
 * @brief 线程互斥锁相关API
 * @author binsonli <binsonli666@iccloud.com>
 * @version 1.0.0
 * @date 2017-03-10
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_LOCK_H__
#define __OSAL_LOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

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
OS_S32 OSAL_LOCK_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

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
OS_S32 OSAL_LOCK_Init(OS_HANDLE *handle);

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
OS_S32 OSAL_LOCK_Deinit(OS_HANDLE handle);

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
OS_S32 OSAL_LOCK_Up(OS_HANDLE handle);

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
OS_S32 OSAL_LOCK_Down(OS_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_LOCK_H__
