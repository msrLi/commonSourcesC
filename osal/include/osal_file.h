/**
 * @file osal_file.h
 * @brief 基于C库二次封装的带参数检查的文件操作API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2018-03-28
 */
/* Copyright (C) 2008-2018, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_FILE_H__
#define __OSAL_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Get_Module_Version
 *         获取FILE模块的当前版本
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
OS_S32 OSAL_FILE_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Check_Exist
 *         检查文件是否存在
 *
 * @param[in]  filepath  文件绝对路径
 *
 * @return
 *          0 -> 文件存在
 *          1 -> 参数错误，无效文件路径NULL
 *          2 -> 文件不存在
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Check_Exist(OS_S8 *filepath);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Delete
 *         删除文件函数
 *
 * @param[in]  filepath  文件绝对路径
 *
 * @return
 *          0 -> 文件删除成功
 *          1 -> 参数无效, 无效文件路径NULL
 *          2 -> 文件不存在
 *          3 -> 文件删除失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Delete(OS_S8 *filepath);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Get_Size
 *         获取指定文件字节数
 *
 * @param[in]     filepath  文件绝对路径
 * @param[in/out] size      文件字节数返回指针，需要外部输入有效指针
 *
 * @return
 *          0 -> 文件大小获取成功
 *          1 -> 参数无效
 *          2 -> 文件不存在
 *          3 -> 获取文件信息失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Get_Size(OS_S8 *filepath, OS_ULONG *size);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Read_Arguments
 *         读取参数
 *
 * @param[in]       path         配置文件绝对路径
 * @param[in]       function     参数所属功能关键字
 * @param[in]       key          参数关键字
 * @param[in/out]   value        有效指针
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 *          2 -> 路径无效
 *          3 -> 配置文件打开失败
 *          4 -> 文件中未找到相关类别
 *          5 -> 文件中未找到相关参数
 *          6 -> 类别内未找到相关参数
 *          7 -> 类别未找到
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Read_Arguments(OS_S8 *path, OS_S8 *function, OS_S8 *key, OS_S8 *value);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_API_Test
 *         FILE相关API测试，已覆盖所有 FILE API函数
 *
 * @return
 *          0 -> 测试通过
 *          1 -> 参数个数为0
 *          2 -> 文件不存在
 *          3 -> 文件删除失败
 *          4 -> 获取文件大小失败
 *          5 -> 解析ini文件失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_API_Test(OS_S32 argc, OS_S8 *argv[]);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_FILE_H__

