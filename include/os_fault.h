/**
 * @file os_fault.h
 * @brief fault
 *
 * @author  <binsonli666@163.com>
 * @version 1.0.0
 * @date 2019-07-12
 */

/* Copyright (C) 2009-2016, msrli Inc.
 * All right reserved
 *
 */

#ifndef __OS_FAULT_H__
#define __OS_FAULT_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef enum FAULT_ENUM
{
    NO_ERROR = 0, 
    PERM_DENIED,
    NOT_READY,
    PARAM_INVALID,
    INVALID_FORMAT,
    NO_MEMORY,
    SYS_ERROR,
    TIMEDOUT,
    NOT_FOUND,
    NOT_EXIST,
    ALREADY_INITED,
    ALREADY_EXISTS,
    NOT_INITED,
    NOT_REQUIRED,
    NOT_SUPPORTED,
    BAD_PROTOCAL,
    USER_ABORTED,
    JUMP_DONE,
    TEST_FAILED,
    UNKNOWN_ERROR,
}FAULT_E;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__OS_FAULT_H__
