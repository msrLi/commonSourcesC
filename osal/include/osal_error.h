/**
 * @file osal_error.h
 * @brief  错误码基本定义及API
 * @author  <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-06-05
 */

/* Copyright (C) 2009-2016, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_ERROR_H__
#define __OSAL_ERROR_H__

#include <errno.h>

/******************************************************************************
* define & enum
******************************************************************************/
#define OSAL_ERR_BASE    (0x10000)

#define OSAL_ESYSTEM    (OSAL_ERR_BASE + 0)
#define OSAL_ENOMEM     (OSAL_ERR_BASE + 1)
#define OSAL_EINPUT     (OSAL_ERR_BASE + 2)
#define OSAL_ETIMEO     (OSAL_ERR_BASE + 3)
#define OSAL_ERR_END    (OSAL_ERR_BASE + 4)
/******************************************************************************
* type
******************************************************************************/

/******************************************************************************
* Global variable
******************************************************************************/

/******************************************************************************
* Global function
******************************************************************************/
static inline void OSAL_Error_Set(int code)
{
    errno = code;
}

static inline OS_S8* OSAL_Get_SysError(void)
{
    return strerror(errno);
}
#endif /* __OSAL_ERROR_H__ */
