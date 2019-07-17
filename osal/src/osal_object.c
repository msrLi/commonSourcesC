/**
 * @file  osal_object.c
 * @brief   message queue
 * @author  <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal.h>
#include "osal_object.h"

/******************************************************************************
* type
******************************************************************************/
typedef struct OSAL_OBJ_TAG {
    struct list_head link;
    OS_U32           size;      /* 长度 */
    OS_U32           type;      /* 类型 */
    OS_S8            name[16];  /* 名称 */
    OS_U8            priv[0];   /* 数据指针 */
} OSAL_OBJ_T;

OS_HANDLE osal_obj_request(OS_U32 size, const OS_S8 * name, OS_U32 type)
{
    OSAL_OBJ_T *obj = NULL;
    OS_HANDLE ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_IPC, (0 == size), NULL, return_null, "Invalid argument! (%d, %p, %d)\n", size, name, type);

    /* 内存分配 */
    obj = (OSAL_OBJ_T*) malloc(size + sizeof(OSAL_OBJ_T));
    OSAL_CHECK_ERROR(MODULE_IPC, (NULL == obj), NULL, return_null, "Malloc %d bytes failed! (Error: %p)\n",
                     (size + sizeof(OSAL_OBJ_T)), obj);

    INIT_LIST_HEAD(&obj->link);

    /* 填充结构体 */
    obj->type = type;
    obj->size = size;
    if (NULL == name) {
        strncpy(obj->name, "noname", 16);
    } else {
        strncpy(obj->name, name, 16);
    }

    /* 初始化数据 */
    memset(obj->priv, 0x55, size);

    return obj->priv;

return_null:
    return ret;
}

void osal_obj_release(OS_HANDLE object)
{
    OSAL_OBJ_T *obj = NULL;

    if (NULL == object) {
        LOGE(MODULE_IPC, "Parameter error!\n");
        return;
    }

    obj = container_of(object, OSAL_OBJ_T, priv);

    free(obj);
}

