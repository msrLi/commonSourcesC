/**
 * @file osal_dir.h
 * @brief 目录操作API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2019-03-27
 */
/* Copyright (C) 2008-2019, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_DIR_H__
#define __OSAL_DIR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_Get_Module_Version
 *         获取DIR模块的当前版本
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
OS_S32 OSAL_DIR_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_Create_Multi_Dir
 *         创建多级目录
 *
 * @param[in]  path       目录
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 *          2 -> 失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_DIR_Create_Multi_Dir(OS_S8 *path);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_DIR_API_Test
 *         DIR相关API测试，已覆盖所有 DIR API函数
 *
 * @return
 *          0 -> 测试通过
 *          1 -> 参数错误
 *          2 -> 创建目录失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_DIR_API_Test(OS_S32 argc, OS_S8 *argv[]);


#ifdef __cplusplus
}
#endif

#endif //__OSAL_DIR_H__

