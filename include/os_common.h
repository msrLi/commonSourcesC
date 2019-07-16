/**
 * @file os_common.h
 * @brief  包含常用头文件
 * @author <binsonli666@163.com>
 * @version 1.0.0
 * @date 2019-01-12
 */

/* Copyright (C) 2009-2016, binsonLi Inc.
 * All right reserved
 *
 */

#ifndef __OS_COMMON_H__
#define __OS_COMMON_H__

/* 常用标准C库头文件 */
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/param.h>   /* NOFILE */
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>    /* sysinfo函数, 获取系统运行时间 */
#include <sys/ipc.h>        /* 核间通讯 */
#include <sys/msg.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include <linux/sockios.h>
#include <pthread.h>
#include <errno.h>

#include "os_typedef.h"
#include "os_fault.h"
#include "os_module.h"

#define SUCCEED(rc)                 ((rc) == NO_ERROR)
#define ISNULL(ptr)                 ((ptr) == NULL)
#define FAILED(rc)                  (!SUCCEED(rc))
#define NOTNULL(ptr)                (!ISNULL(ptr))
#define POSITIVE_NUM(num)           ((num) > 0)
#define NEGITIVE_NUM(num)           ((num) < 0)
#define ISZERO(num)                 ((num) == 0)
#define ARRAYSIZE(array)            (sizeof(array) / sizeof(array[0]))
#define INVALIDNUM                  (-1)
#define EQUALPTR(a, b)              ((a) == (b))
// #define MAX(a, b)                   ((a) > (b) ? (a) : (b))
// #define MIN(a, b)                   ((a) < (b) ? (a) : (b))
#define RESETRESULT(rc)             ((rc) = NO_ERROR)
#define RETURNIGNORE(rc, ignore)    ((rc) & (~(ignore)))
#define SUCCEEDIGNORE(rc, ignore)   (SUCCEED(rc) || ((rc) == (ignore)))
#define EPSINON                     (1e-7)
#define EQUALFLOAT(a, b)            (fabsf((a) - (b)) < EPSINON)

#define SECURE_FREE(ptr)                         \
    do {                                         \
        if (!ISNULL(ptr)) {                      \
            Free(ptr);                           \
            (ptr) = NULL;                        \
        }                                        \
    } while(0)

#define SECURE_DELETE(obj)                       \
    do {                                         \
        if (!ISNULL(obj)) {                      \
            delete(obj);                         \
            (obj) = NULL;                        \
        }                                        \
    } while(0)

#define COMPARE_SAME_STRING(LHS, RHS)            \
    ({                                           \
        bool _result = true;                     \
        if (NOTNULL(LHS) && NOTNULL(RHS)) {      \
            _result &= !strcmp(LHS, RHS);        \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else {                                 \
            _result = false;                     \
        }                                        \
        _result;                                 \
    })

#define COMPARE_SAME_LEN_STRING(LHS, RHS, len)   \
    ({                                           \
        bool _result = true;                     \
        if (NOTNULL(LHS) && NOTNULL(RHS)) {      \
            _result &= !strncmp(LHS, RHS, len);  \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else {                                 \
            _result = false;                     \
        }                                        \
        _result;                                 \
    })

#define COMPARE_SAME_DATA(LHS, RHS, SIZE)        \
    ({                                           \
        bool _result = true;                     \
        if (NOTNULL(LHS) && NOTNULL(RHS)) {      \
            _result &= !memcmp(LHS, RHS, SIZE);  \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else {                                 \
            _result = false;                     \
        }                                        \
        _result;                                 \
    })
#if 0
#ifndef offsetof
#define offsetof(TYPE, MEMBER)                   \
    ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({                 \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
#endif

#define ALIGN_LEN_TO_SIZE(len,size) ({                     \
        (((len) + (size) - 1) & ~((size) - 1)); });


#endif //__OS_COMMON_H__
