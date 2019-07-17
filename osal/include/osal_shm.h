/**
 * @file  osal_shm.h
 * @brief  共享内存模块相关API
 * @author binsonli <itarge@itarge.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null  Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_SHM_H__
#define __OSAL_SHM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define OSAL_SHM_PROC_MEM_SIZE 8192
#define OSAL_SHM_MAX_SHARE_PROC  8

typedef enum OSAL_SHM_FLAG_TAG {
    OSAL_SHM_FLAG_CREAT = 0,
    OSAL_SHM_FLAG_CREAT_ERR = 1,
    OSAL_SHM_FLAG_CREAT_EXCL = 2,
}
OSAL_SHM_FLAG_T;

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Module_Version
 *         获取SHM模块的当前版本
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
OS_S32 OSAL_SHM_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Init
 *         共享内存初始化
 *
 * @param[in]       key     共享内存Key
 * @param[in]       size    共享内存大小,
 *                          大于0的整数：新建的共享内存大小，以字节为单位,
 *                          0:只获取共享内存时指定为0
 *                          如果不清楚请置0
 *
 * @param[in]       flag    共享内存标志
 *                          OSAL_SHM_FLAG_CREAT: 共享内存不存在创建，存在返回对应的id,
 *                          OSAL_SHM_FLAG_CREAT_ERR: 共享内存标识符，若不存在则返回错误,
 *                          OSAL_SHM_FLAG_CREAT_EXCL: 共享内存不存在创建，存在则返回错误
 *                          如果不清楚请置0
 * @param[in/out]   mid     共享内存id指针，需要外部传入有效指针，不能为NULL
 *
 * @return
 *          0 -> 初始化成功
 *          1 -> 参数错误
 *          2 -> 初始化失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Init(OS_S32 key, OS_U32 size, OS_U32 flag, OS_S32 *mid);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Addr
 *         把共享内存区对象映射到调用进程的地址空间
 *
 * @param[in]       mid     共享内存id, 参考OSAL_SHM_Init
 * @param[in/out]   addr    共享内存地址指针，需要外部传入有效指针，不能为NULL
 *
 * @return
 *          0 -> 获取共享内存地址成功
 *          1 -> 参数错误
 *          2 -> 获取共享内存地址失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Get_Addr(OS_S32 mid, OS_HANDLE *addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Detach_Addr
 *         取消共享内存地址映射
 *
 * @param[in]  addr     共享内存地址
 *
 * @return
 *          0 -> 取消共享内存地址映射成功
 *          1 -> 参数错误
 *          2 -> 取消共享内存地址映射失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Detach_Addr(OS_HANDLE addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Read
 *         共享内存读取操作
 *
 * @param[in]       mid     共享内存id, 参考OSAL_SHM_Init
 * @param[in]       offset  共享内存 offset
 * @param[in/out]   buf     存放共享数据的指针，需要外部传入，不能为NULL
 * @param[in]       len     需要读取的数据长度，(offset+len)不能超过共享内存大小
 *
 * @return
 *          0 -> 读取共享内存数据成功
 *          1 -> 参数错误
 *          2 -> 映射共享内存到当前进程错误
 *          3 -> 取消共享内存映射错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Read(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Write
 *         共享内存写入操作
 *
 * @param[in]   mid         共享内存id, 参考OSAL_SHM_Init
 * @param[in]   offset      共享内存 offset
 * @param[in]   buf         需要写入的数据内存指针
 * @param[in]   len         需要写入的数据长度
 *
 * @return
 *          0 -> 共享内存写入成功
 *          1 -> 参数错误
 *          2 -> 映射共享内存到当前进程错误
 *          3 -> 取消共享内存映射错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Write(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len);

#ifdef __cplusplus
}
#endif

#endif // __OSAL_SHM_H__
