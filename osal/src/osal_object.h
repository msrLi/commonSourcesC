/******************************************************************************
* OSAL_Object.h NULL C header file of osal object(inner)
*
* Copyright (C) Beijing Null Software Technologies Co.,Ltd.
*
* YYYY.mm.dd    Create
* YYYY.mm.dd    Brief modify description
******************************************************************************/
#ifndef __OSAL_OBJECT__
#define __OSAL_OBJECT__

#include <os_common.h>
#include "osal_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* define & enum
******************************************************************************/

/******************************************************************************
* types
******************************************************************************/

/******************************************************************************
* Global variable
******************************************************************************/

/******************************************************************************
* Global function
******************************************************************************/
OS_HANDLE osal_obj_request(OS_U32 size, OS_S8 * name, OS_U32 type);

void osal_obj_release(OS_HANDLE object);

#ifdef __cplusplus
}
#endif

#endif /* __OSAL_OBJECT__ */

/* End of file */

