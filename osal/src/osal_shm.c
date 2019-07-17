/**
 * @file  osal_shm.c
 * @brief  共享内存模块相关API
 * @author binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */


#include <os_common.h>
#include <osal.h>

/* 共享内存模块的版本号, 代码有修改时必须修改 */
#define OSAL_SHM_MODULE_VERSION  ("1.2.0")

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
OS_S32 OSAL_SHM_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* 版本打印，便于调试 */
    LOGI(MODULE_SHM, "%s: %s\n", __func__, OSAL_SHM_MODULE_VERSION);

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* 版本返回，便于上层应用通过版本号判断，实现差异化兼容处理 */
    ret = sscanf(OSAL_SHM_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_SHM, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_SHM | ret);
}

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
OS_S32 OSAL_SHM_Init(OS_S32 key, OS_U32 size, OS_U32 flag, OS_S32 *mid)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!mid), 1, return_err,
                     "Invalid argument! (0x%x, %d, %d, %p)\n", key, size, flag, mid);

    switch (flag) {
        case OSAL_SHM_FLAG_CREAT:
            ret = shmget(key, 0, 0);
            if (ret < 0) {
                ret = shmget(key, size, IPC_CREAT | 0660);
            }
            break;
        case OSAL_SHM_FLAG_CREAT_ERR:
            ret = shmget(key, 0, 0);
            break;
        case OSAL_SHM_FLAG_CREAT_EXCL:
            ret = shmget(key, size, IPC_CREAT | IPC_EXCL | 0660);
            break;
        default:
            OSAL_CHECK_ERROR(MODULE_SHM, 1, 1, return_err, "Invalid argument! flag = %d\n", flag);
            break;

    }
    /* 失败时返回-1和errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret == -1), 2, return_err, "Call shmget fail with %s!\n", strerror(errno));

    /* 返回有效mid值 */
    *mid = ret;

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

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
OS_S32 OSAL_SHM_Get_Addr(OS_S32 mid, OS_HANDLE *addr)
{
    OS_HANDLE shm_addr = NULL;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!addr), 1, return_err,
                     "Invalid argument! (0x%x, %p)\n", mid, addr);

    shm_addr = shmat(mid, 0, 0);
    /* 成功时返回正确的共享内存地址， 失败时返回 (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    /* 输出有效地址 */
    *addr = shm_addr;

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

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
OS_S32 OSAL_SHM_Detach_Addr(OS_HANDLE addr)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!addr), 1, return_err, "Invalid argument! (%p)\n", addr);

    ret = shmdt(addr);
    /* 成功时返回0， 失败时返回-1和errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 2, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

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
OS_S32 OSAL_SHM_Read(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len)
{
    OS_S8 *shm_addr = NULL;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!buf), 1, return_err,
                     "Invalid argument! (%d, %d, %p, %d)\n", mid, offset, buf, len);

    shm_addr = (OS_S8*) shmat(mid, 0, 0);
    /* 成功时返回正确的共享内存地址， 失败时返回 (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    memcpy(buf, shm_addr + offset, len);

    ret = shmdt(shm_addr);
    /* 成功时返回0， 失败时返回-1和errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 3, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

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
OS_S32 OSAL_SHM_Write(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len)
{
    OS_S8 *shm_addr = NULL;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_SHM, (!buf), 1, return_err,
                     "Invalid argument! (%d, %d, %p, %d)\n", mid, offset, buf, len);

    shm_addr = (OS_S8*) shmat(mid, 0, 0);
    /* 成功时返回正确的共享内存地址， 失败时返回 (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    /* 写入数据 */
    memcpy(shm_addr + offset, buf, len);

    ret = shmdt(shm_addr);
    /* 成功时返回0， 失败时返回-1和errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 3, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

