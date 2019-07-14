/**
 * @file os_typedef.h
 * @brief 基本公用数据类型定义
 *
 * @author  <binsonli666@163.com>
 * @version 1.0.0
 * @date 2019-07-12
 */

/* Copyright (C) 2009-2016, msrli Inc.
 * All right reserved
 *
 */

#ifndef __OS_TYPEDEF_H__
#define __OS_TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* 数据类型定义 */
typedef char                        OS_S8;
typedef unsigned char               OS_U8;

typedef short                       OS_S16;
typedef unsigned short              OS_U16;

typedef int                         OS_S32;
typedef long                        OS_SLONG;
typedef unsigned int                OS_U32;
typedef unsigned long               OS_ULONG;
typedef float                       OS_F32;

typedef long long                   OS_S64;
typedef unsigned long long          OS_U64;
typedef double                      OS_D64;

typedef int                         OS_BOOL;
typedef void*                       OS_HANDLE;

typedef struct OS_RECT_TAG {
    OS_U32 left;
    OS_U32 right;
    OS_U32 top;
    OS_U32 bottom;
} OS_RECT_T;

typedef struct OS_RECT_U16_TAG {
    OS_U16 left;
    OS_U16 right;
    OS_U16 top;
    OS_U16 bottom;
} OS_RECT_U16_T;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__OS_TYPEDEF_H__
