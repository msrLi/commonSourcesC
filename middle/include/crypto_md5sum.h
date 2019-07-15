/**
 * @file crypto.h
 * @brief 加密相关API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.2.0
 * @date 2019-02-19
 */

/* Copyright(C) 2009-2016, null Inc.
 * All right reserved
 *
 */

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Get_Module_Version
 *         获取CRYPTO模块的当前版本
 *
 * @param[in/out]      major_version  模块主版本号返回指针，外部输入有效指针，长度OS_U32
 * @param[in/out]      minor_version  模块子版本号返回指针，外部输入有效指针，长度OS_U32
 * @param[in/out]      build_version  模块编译版本号返回指针，外部输入有效指针，长度OS_U32
 *
 * @return
 *          0 -> 版本号获取成功
 *          1 -> 参数错误，输入指针非法
 *          2 -> 版本号格式错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Calulate_Buffer_MD5
 *         计算数据的md5值
 *
 * @param[in/out]       buffer      数据指针
 * @param[in/out]       len         数据长度
 * @param[in/out]       md5_str     有效指针
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Calulate_Buffer_MD5(OS_U8 *buffer, OS_U32 len, OS_S8 *md5_str);


/* --------------------------------------------------------------------------*/
/**
 * @brief  CRYPTO_Calulate_File_MD5
 *         计算文件的md5值
 *
 * @param[in/out]       file_path   文件绝对路径
 * @param[in/out]       md5_str     有效指针
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 *          2 -> 文件打开失败
 *          3 -> 读取数据失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_Calulate_File_MD5(OS_S8 *file_path, OS_S8 *md5_str);

/* --------------------------------------------------------------------------*/
/**
 * @brief  main
 *         加密相关API测试程序，已覆盖所有 CRYPTO API函数
 *
 * @return
 *          0 -> 测试通过
 *          1 -> 参数错误
 *          2 -> 通过文件计算MD5失败
 *          3 -> 打开文件失败
 *          4 -> 获取文件大小失败
 *          5 -> 申请内存失败
 *          6 -> 读取数据失败
 *          7 -> 通过内存计算MD5失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 CRYPTO_API_Test(OS_S32 argc, OS_S8 *argv[]);

#ifdef __cplusplus
}       // extern "C" {
#endif  // __cplusplus

#endif  // __CRYPTO_H__
