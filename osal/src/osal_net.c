/**
 * @file osal_net.c
 * @brief  网络模块API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-06-05
 */

/* Copyright (C) 2009-2017, Null Inc.
 * All right reserved
 *
 */

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <fcntl.h>
#include <os_common.h>
#include <zlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <osal.h>
#include <osal_object.h>



/* NET 模块的版本号, 代码有修改时必须修改 */
#define OSAL_NET_MODULE_VERSION  ("1.2.0")

/******************************************************************************
* define & enum
******************************************************************************/
#define PROCNET_ROUTE_PATH      "/proc/net/route"
#define PATH_PROCNET_DEV        "/proc/net/dev"

#define ALL_ADVERTISED_MODES            \
    (ADVERTISED_10baseT_Half |          \
     ADVERTISED_10baseT_Full |          \
     ADVERTISED_100baseT_Half |         \
     ADVERTISED_100baseT_Full |         \
     ADVERTISED_1000baseT_Half |        \
     ADVERTISED_1000baseT_Full |        \
     ADVERTISED_2500baseX_Full |        \
     ADVERTISED_10000baseKX4_Full |     \
     ADVERTISED_10000baseKR_Full |      \
     ADVERTISED_10000baseR_FEC)

#define OSAL_NET_WAIT_NONE               (0)
#define OSAL_NET_WAIT_FOREVER            ((OS_U32) (~0))
#define OSAL_NET_LINK_DOWN               (0)
#define OSAL_NET_LINK_UP                 (1)

typedef enum OSAL_NET_TYPE_ENUM {
    NET_TYPE_TCPSVR = 0,
    NET_TYPE_TCPCLI,
    NET_TYPE_UDP,
} OSAL_NET_TYPE_E;

/******************************************************************************
* type
******************************************************************************/
typedef struct OSAL_LINK_TAG {
    struct sockaddr  local;
    struct sockaddr  remote;
    OS_S32           sock;
    OS_S32           type;
    OS_S32           lasterr;
    OS_U64           rxbytecnt;
    OS_U64           txbytecnt;
    OS_HANDLE        userarg;
    OSAL_Socket_Proc_Handler  userproc;

    //used for tcp server
    pthread_t        task;
    struct list_head link_head;
} OSAL_LINK_T;

typedef struct OSAL_NET_TAG {
    volatile OS_S32     done;
    OS_S32              sock;
    OS_S32              type;
    OS_S32              link_max;
    OS_S32              link_cnt;
    OSAL_LINK_T          link_info;

    //used for tcp server
    struct list_head link_head;
    pthread_mutex_t  link_lock;
    pthread_cond_t   link_cond;
    pthread_t        link_task[0];
} OSAL_NET_T;

/******************************************************************************
* Global
******************************************************************************/
static struct sockaddr_in pf_addr = {
sin_family:
    PF_INET,
    sin_port:   0
};

/******************************************************************************
* Local function
******************************************************************************/

/* TCP 服务解除线程锁 */
static void OSAL_tcp_server_unlock(OS_HANDLE arg)
{
    OSAL_NET_T *this_obj  = (OSAL_NET_T *)arg;

    pthread_mutex_unlock(&this_obj->link_lock);
}

/* TCP 服务关闭socket */
static void OSAL_tcp_server_close_socket(OS_HANDLE arg)
{
    OSAL_LINK_T *this_link = (OSAL_LINK_T *)arg;

    close(this_link->sock);
}

/* tcp 服务端任务线程处理函数 */
static OS_HANDLE OSAL_tcp_server_task(OS_HANDLE arg)
{
    OSAL_NET_T *this_obj = (OSAL_NET_T *)arg;
    OSAL_LINK_T link;

    while (1) {
        memset(&link, 0, sizeof(OSAL_LINK_T));

        /* wait for new connect */
        pthread_mutex_lock(&this_obj->link_lock);
        pthread_cleanup_push(OSAL_tcp_server_unlock, this_obj);
        while (this_obj->link_info.sock <= 0) {
            pthread_cond_wait(&this_obj->link_cond, &this_obj->link_lock);
        }
        pthread_cleanup_pop(0);
        this_obj->link_cnt++;
        link = this_obj->link_info;
        link.task = pthread_self();
        list_add_tail(&link.link_head, &this_obj->link_head);
        memset(&this_obj->link_info, 0, sizeof(OSAL_LINK_T));
        pthread_mutex_unlock(&this_obj->link_lock);

        pthread_cleanup_push(OSAL_tcp_server_close_socket, &link);

        /* call user proc */
        if (link.userproc) {
            link.userproc(&link, link.userarg);
        }

        //close connect
        pthread_mutex_lock(&this_obj->link_lock);
        list_del(&link.link_head);
        this_obj->link_cnt--;
        pthread_mutex_unlock(&this_obj->link_lock);

        pthread_cleanup_pop(1); //exec OSAL_tcp_server_close_socket
    }

    return NULL;
}

/* tcp 服务端task删除 */
static OS_S32 OSAL_tcp_server_delete(OSAL_NET_T *this_obj)
{
    OS_S32 task_id = 0;

    //TODO:
    //      We should wait for proc done!!!
    //      Change it! Other sync is better!
    if (this_obj->done == 0) {
        this_obj->done = 1;
        sleep(2);
    }

    for (task_id = 0; task_id < this_obj->link_max; task_id++) {
        pthread_cancel(this_obj->link_task[task_id]);
        pthread_join(this_obj->link_task[task_id], NULL);
    }

    if (this_obj->sock >= 0) {
        close(this_obj->sock);
    }

    osal_obj_release(this_obj);

    return 0;
}

/* TCP 客户端销毁 */
static OS_S32 OSAL_tcp_client_delete(OSAL_NET_T *this_obj)
{
    if (this_obj->sock >= 0) {
        close(this_obj->sock);
    }

    osal_obj_release(this_obj);

    return 0;
}

/* UDP 客户端销毁 */
static OS_S32 OSAL_udp_delete(OSAL_NET_T *this_obj)
{
    if (this_obj->sock >= 0) {
        close(this_obj->sock);
    }

    osal_obj_release(this_obj);

    return 0;
}

/* TCP 服务端处理函数 */
static OS_S32 OSAL_tcp_server_process(OSAL_NET_T *this_obj, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg)
{
    OS_S32 retval = 0;
    OS_S32 sock = -1;
    struct sockaddr remote = {0};
    socklen_t       addrsize = sizeof(struct sockaddr);
    fd_set          rfds;
    struct timeval  twait = {0};

    while (!this_obj->done) {
        FD_ZERO(&rfds);
        FD_SET(this_obj->sock, &rfds);
        twait.tv_sec  = 1;
        twait.tv_usec = 0;

        retval = select(this_obj->sock + 1, &rfds, NULL, NULL, &twait);
        if (retval == 0) {
            continue;
        }
        if (retval <  0) {
            break;
        }

        sock = accept(this_obj->sock, &remote, &addrsize);
        if (sock < 0) {
            break;
        }

        //Send link to procTask
        pthread_mutex_lock(&this_obj->link_lock);
        if (this_obj->link_cnt < this_obj->link_max
            && this_obj->link_info.sock == 0) {
            this_obj->link_info.sock     = sock;
            this_obj->link_info.remote   = remote;
            this_obj->link_info.userarg  = arg;
            this_obj->link_info.userproc = proc;
            sock = -1;
            pthread_cond_signal(&this_obj->link_cond);
        }
        pthread_mutex_unlock(&this_obj->link_lock);

        //We simply close the connect, if no task to service.
        if (sock >= 0) {
            close(sock);
        }
    }

    return 0;
}

/* TCP 客户端处理函数 */
static OS_S32 OSAL_tcp_client_process(OSAL_NET_T *this_obj, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg)
{
    OS_S32 retval = -1;

    memset(&this_obj->link_info, 0, sizeof(OSAL_LINK_T));

    this_obj->link_info.sock = this_obj->sock;
    this_obj->link_info.type = NET_TYPE_TCPCLI;
    this_obj->link_info.userarg  = arg;
    this_obj->link_info.userproc = proc;

    if (proc) {
        retval = proc(&this_obj->link_info, arg);
    }

    return retval;
}

/* UDP 处理函数 */
static OS_S32 OSAL_udp_process(OSAL_NET_T *this_obj, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg)
{
    OS_S32 retval = -1;

    memset(&this_obj->link_info, 0, sizeof(OSAL_LINK_T));

    this_obj->link_info.sock = this_obj->sock;
    this_obj->link_info.type = NET_TYPE_UDP;
    this_obj->link_info.userarg  = arg;
    this_obj->link_info.userproc = proc;

    if (proc) {
        retval = proc(&this_obj->link_info, arg);
    }

    return retval;
}

/* TCP 接收函数 */
static OS_S32 OSAL_tcp_receive(OSAL_LINK_T *this_link, OS_HANDLE buf, OS_U32 size, OS_U32 tout)
{
    fd_set rfds;
    struct timeval twait = {0};
    OS_S32 retval = 0;

    if (tout != (OS_U32) OSAL_NET_WAIT_FOREVER) {
        FD_ZERO(&rfds);
        FD_SET(this_link->sock, &rfds);

        twait.tv_sec = tout / 1000;
        twait.tv_usec = (tout % 1000) * 1000;

        retval = select(this_link->sock + 1, &rfds, NULL, NULL, &twait);
        if (retval == 0) {
            return 0;
        }
        if (retval <  0) {
            this_link->lasterr = errno;
            return -1;
        }
    }

    retval = recv(this_link->sock, buf, size, 0);
    if (retval < 0) {
        this_link->lasterr = errno;
        return -2;
    } else {
        this_link->rxbytecnt += retval;
    }

    return retval;
}

/* TCP 发送函数 */
static OS_S32 OSAL_tcp_send(OSAL_LINK_T *this_link, OS_HANDLE buf, OS_U32 size, OS_U32 tout)
{
    struct timeval twait = {0};
    fd_set wfds;
    OS_S32 retval = 0;

    if (tout != OSAL_NET_WAIT_FOREVER) {

        FD_ZERO(&wfds);
        FD_SET(this_link->sock, &wfds);

        twait.tv_sec = tout / 1000;
        twait.tv_usec = (tout % 1000) * 1000;

        retval = select(this_link->sock + 1, NULL, &wfds, NULL, &twait);
        if (retval == 0) {
            return 0;
        }
        if (retval <  0) {
            this_link->lasterr = errno;
            return -1;
        }
    }

    retval = send(this_link->sock, buf, size, 0);
    if (retval < 0) {
        this_link->lasterr = errno;
        return -2;
    } else {
        this_link->txbytecnt += retval;
    }

    return retval;
}

/* UDP 接收函数 */
static OS_S32 OSAL_udp_receive(OSAL_LINK_T *this_link, OS_HANDLE buf, OS_U32 size, OS_U32 tout)
{
    struct timeval twait = {0};
    fd_set rfds;
    OS_S32 retval = 0;
    socklen_t addrlen = 0;

    if (tout != (OS_U32) OSAL_NET_WAIT_FOREVER) {

        FD_ZERO(&rfds);
        FD_SET(this_link->sock, &rfds);

        twait.tv_sec = tout / 1000;
        twait.tv_usec = (tout % 1000) * 1000;

        retval = select(this_link->sock + 1, &rfds, NULL, NULL, &twait);
        if (retval == 0) {
            return 0;
        }
        if (retval <  0) {
            this_link->lasterr = errno;
            return -1;
        }
    }

    addrlen = sizeof(struct sockaddr);
    retval = recvfrom(this_link->sock, buf, size, 0, &this_link->remote, &addrlen);
    if (retval < 0) {
        this_link->lasterr = errno;
        return -2;
    } else {
        this_link->rxbytecnt += retval;
    }

    return retval;
}

/* UDP 发送函数 */
static OS_S32 OSAL_udp_send(OSAL_LINK_T *this_link, OS_HANDLE buf, OS_U32 size, OS_U32 tout)
{
    struct timeval  twait = {0};
    fd_set wfds;
    OS_S32 retval = 0;

    if (tout != OSAL_NET_WAIT_FOREVER) {

        FD_ZERO(&wfds);
        FD_SET(this_link->sock, &wfds);

        twait.tv_sec = tout / 1000;
        twait.tv_usec = (tout % 1000) * 1000;

        retval = select(this_link->sock + 1, NULL, &wfds, NULL, &twait);
        if (retval == 0) {
            return 0;
        }
        if (retval <  0) {
            this_link->lasterr = errno;
            return -1;
        }
    }

    retval = sendto(this_link->sock, buf, size, 0, &this_link->remote, sizeof(struct sockaddr));
    if (retval < 0) {
        this_link->lasterr = errno;
        return -2;
    } else {
        this_link->txbytecnt += retval;
    }

    return retval;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Module_Version
 *         获取NET模块的当前版本
 *
 * @param[out]      major_version  模块主版本号返回指针，外部输入有效指针，长度OS_U32
 * @param[out]      minor_version  模块子版本号返回指针，外部输入有效指针，长度OS_U32
 * @param[out]      build_version  模块编译版本号返回指针，外部输入有效指针，长度OS_U32
 *
 * @return
 *          0 -> 版本号获取成功
 *          1 -> 参数错误，输入指针非法
 *          2 -> 版本号格式错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* 版本打印，便于调试 */
    LOGI(MODULE_NET, "%s: %s\n", __func__, OSAL_NET_MODULE_VERSION);

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!major_version) || (!minor_version) || (!build_version)), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* 版本返回，便于上层应用通过版本号判断，实现差异化兼容处理 */
    ret = sscanf(OSAL_NET_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_NET, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_TCP_Server
 *         创建TCP服务端
 *
 * @param[out]  link      服务端句柄指针，上层需要保存维护，以便后续调用识别区分
 * @param[in]   addr      需要监控的IP地址， NULL表示本机所有IP地址
 * @param[in]   port      需要监控的端口
 * @param[in]   backlog   支持同时连接数
 *
 * @return
 *          0 -> 成功创建服务
 *          1 -> 参数错误
 *          2 -> socket错误
 *          3 -> bind失败
 *          4 -> listen失败
 *          5 -> 分配资源失败
 *          6 -> 创建线程失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_TCP_Server(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U16 backlog)
{
    OSAL_NET_T *this_obj = NULL;
    struct sockaddr_in inaddr = {0};
    OS_S32 sock   = -1;
    OS_S32 onoff  = 0;
    OS_S32 task_id = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (link == NULL || backlog < 1), 1, return_err,
                     "Invalid argument! (%p, %p, %d, %d)\n", link, addr, port, backlog);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (sock < 0), 2, return_err, "Socket create failed! (Error: %d)\n", sock);

    inaddr.sin_family = AF_INET;
    if (NULL == addr || 0 == inet_aton(addr, &inaddr.sin_addr)) {
        inaddr.sin_addr.s_addr = INADDR_ANY;
    }
    inaddr.sin_port = htons(port);

    onoff = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (OS_HANDLE) &onoff, sizeof(onoff));

    ret = bind(sock, (struct sockaddr *)&inaddr, sizeof(inaddr));
    OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 3, DO_ERROR, "Bind failed! (Error: %d)\n", ret);

    ret = listen(sock, backlog);
    OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 4, DO_ERROR, "Listen failed! (Error: %d)\n", ret);

    this_obj = (OSAL_NET_T *)osal_obj_request(sizeof(OSAL_NET_T) + sizeof(pthread_t) * backlog,
               "net", 0);
    OSAL_CHECK_ERROR(MODULE_NET, (!this_obj), 5, DO_ERROR, "Obj request failed! (Error: %p)\n", this_obj);
    memset(this_obj, 0, sizeof(OSAL_NET_T));

    this_obj->sock = sock;
    this_obj->type = NET_TYPE_TCPSVR;
    this_obj->link_max = backlog;
    this_obj->link_cnt = 0;

    INIT_LIST_HEAD(&this_obj->link_head);
    pthread_mutex_init(&this_obj->link_lock, NULL);
    pthread_cond_init(&this_obj->link_cond, NULL);

    for (task_id = 0; task_id < backlog; task_id++) {
        ret = pthread_create(&this_obj->link_task[task_id], NULL, OSAL_tcp_server_task, this_obj);
        OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 6, DO_ERROR, "Thread create failed! (Error: %d)\n", ret);
    }
    *link = (OS_HANDLE)this_obj;

    return 0;

DO_ERROR:
    for (; --task_id > 0;) {
        pthread_cancel(this_obj->link_task[task_id]);
        pthread_join(this_obj->link_task[task_id], NULL);
    }
    if (sock >= 0) {
        close(sock);
    }

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_TCP_Client
 *         创建TCP客户端
 *
 * @param[out]  link      客户端句柄指针，上层需要保存维护，以便后续调用识别区分
 * @param[in]   addr      需要连接的IP地址， 不能为NULL
 * @param[in]   port      需要连接的端口
 *
 * @return
 *          0 -> 成功创建连接
 *          1 -> 参数错误
 *          2 -> IP地址不合法
 *          3 -> socket错误
 *          4 -> connect服务端失败
 *          5 -> 分配资源失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_TCP_Client(OS_HANDLE *link, OS_S8 *addr, OS_U16 port)
{
    OSAL_NET_T *this_obj = NULL;
    struct sockaddr_in inaddr = {0};
    OS_S32 sock = -1;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!link) || (!addr)), 1, return_err,
                     "Invalid argument! (%p, %p, %d)\n", link, addr, port);

    /* IP地址转换 */
    inaddr.sin_family = AF_INET;
    inaddr.sin_port = htons(port);
    /* IP地址不合法，转换失败 */
    OSAL_CHECK_ERROR(MODULE_NET, (0 == inet_aton(addr, &inaddr.sin_addr)), 2, return_err, "Invalid IP address\n");

    sock = socket(AF_INET, SOCK_STREAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (sock < 0), 3, return_err, "Socket create failed! (Error: %d)\n", sock);

    ret = connect(sock, (struct sockaddr*)&inaddr, sizeof(inaddr));
    OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 4, DO_ERROR, "Connect failed! (Error: %d)\n", ret);

    this_obj = (OSAL_NET_T *)osal_obj_request(sizeof(OSAL_NET_T), "net", 0);
    OSAL_CHECK_ERROR(MODULE_NET, (!this_obj), 5, DO_ERROR, "Obj request failed! (Error: %p)\n", this_obj);
    memset(this_obj, 0, sizeof(OSAL_NET_T));

    this_obj->sock = sock;
    this_obj->type = NET_TYPE_TCPCLI;
    this_obj->link_max = 1;
    this_obj->link_cnt = 0;
    *link = (OS_HANDLE)this_obj;

    return 0;

DO_ERROR:
    if (sock >= 0) {
        close(sock);
    }

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_UDP
 *         创建UDP连接
 *
 * @param[out]  link      连接句柄指针，上层需要保存维护，以便后续调用识别区分
 * @param[in]   addr      需要连接的IP地址， NULL表示本机所有IP地址
 * @param[in]   port      需要连接的端口
 * @param[in]   flags     创建标志
 *
 * @return
 *          0 -> 成功创建连接
 *          1 -> 参数错误
 *          2 -> socket错误
 *          3 -> 设置广播包发送失败
 *          4 -> bind失败
 *          5 -> 分配资源失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_UDP(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U32 flags)
{
    OSAL_NET_T *this_obj = NULL;
    struct sockaddr_in inaddr = {0};
    OS_S32 sock = -1;
    OS_S32 on = 1;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!link), 1, return_err,
                     "Invalid argument! (%p, %p, %d, %d)\n", link, addr, port, flags);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (sock < 0), 2, return_err, "Socket create failed! (Error: %d)\n", sock);

    inaddr.sin_family = AF_INET;
    if (NULL == addr || 0 == inet_aton(addr, &inaddr.sin_addr)) {
        inaddr.sin_addr.s_addr = INADDR_ANY;
    }
    inaddr.sin_port = htons(port);

    if (flags & OSAL_NET_FL_BROADCAST) {
        ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
        OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 3, DO_ERROR, "Setsockopt failed! (Error: %d)\n", ret);
    }

    ret = bind(sock, (struct sockaddr *)&inaddr, sizeof(struct sockaddr));
    OSAL_CHECK_ERROR(MODULE_NET, (ret != 0), 4, DO_ERROR, "Bind failed! (Error: %d)\n", ret);

    this_obj = (OSAL_NET_T *)osal_obj_request(sizeof(OSAL_NET_T), "net", 0);
    OSAL_CHECK_ERROR(MODULE_NET, (!this_obj), 5, DO_ERROR, "Obj request failed! (Error: %p)\n", this_obj);
    memset(this_obj, 0, sizeof(OSAL_NET_T));

    this_obj->sock = sock;
    this_obj->type = NET_TYPE_UDP;
    this_obj->link_max = 1;
    this_obj->link_cnt = 0;
    *link = (OS_HANDLE)this_obj;

    return 0;

DO_ERROR:
    if (sock >= 0) {
        close(sock);
    }

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Delete
 *         删除相应的socket连接
 *
 * @param[in]  link   连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 *
 * @return
 *          0      -> 删除成功
 *          1      -> 不支持的操作
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Delete(OS_HANDLE link)
{
    OSAL_NET_T *this_obj = (OSAL_NET_T *)link;

    switch (this_obj->type) {
        case NET_TYPE_TCPSVR:
            return OSAL_tcp_server_delete(this_obj);
        case NET_TYPE_TCPCLI:
            return OSAL_tcp_client_delete(this_obj);
        case NET_TYPE_UDP:
            return OSAL_udp_delete(this_obj);
        default:
            LOGE(MODULE_NET, "Unsupport protocol!\n");
            return (MODULE_NET | 1);
    }

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Process
 *         监控socket并执行回调函数
 *
 * @param[in]  link   连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 * @param[in]  proc 需要执行的回调函数指针，
 * @param[in]  arg  需要回调函数的参数指针
 *
 * @return
 *          0      -> 回调成功
 *          1      -> 不支持的操作
 *          其他值 -> 回调失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Process(OS_HANDLE link, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg)
{
    OSAL_NET_T *this_obj = (OSAL_NET_T *)link;

    switch (this_obj->type) {
        case NET_TYPE_TCPSVR:
            return OSAL_tcp_server_process(this_obj, proc, arg);
        case NET_TYPE_TCPCLI:
            return OSAL_tcp_client_process(this_obj, proc, arg);
        case NET_TYPE_UDP:
            return OSAL_udp_process(this_obj, proc, arg);
        default:
            LOGE(MODULE_NET, "Unsupport protocol!\n");
            return MODULE_NET | 1;
    }

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Set_Peer
 *         设置修改远端地址，暂时只支持UDP连接， ip和addr只能支持一个
 *
 * @param[in]  link   连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 * @param[in]  ip     远端IP地址, 例如"192.168.1.1"， NULL表示从addr获取
 * @param[in]  addr   远端IP地址, 只有ip为NULL时，此参数才有效
 * @param[in]  port   远端端口
 *
 * @return
 *          0 -> 设置成功
 *          1 -> 设置失败
 *          2 -> 不支持的操作
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Set_Peer(OS_HANDLE link, OS_S8 *ip, OS_U32 addr, OS_U16 port)
{
    OSAL_LINK_T *this_link = (OSAL_LINK_T *)link;
    struct sockaddr_in *inaddr = NULL;
    OS_S32 ret = 0;

    switch (this_link->type) {
        case NET_TYPE_UDP:
            inaddr = (struct sockaddr_in *)&this_link->remote;

            if (inaddr->sin_family != AF_INET) {
                inaddr->sin_family = AF_INET;
            }

            /* 支持字符串方式 和 struct in_addr 传IP地址 */
            if (NULL != ip) {
                ret = inet_aton(ip, &inaddr->sin_addr);
                /* 字符串转换为IP地址 */
                OSAL_CHECK_ERROR(MODULE_NET, (0 == ret), 1, return_err, "Setup failed! (Error: %d)\n", ret);
            } else {
                inaddr->sin_addr.s_addr = addr;
            }

            if (port != 0) {
                inaddr->sin_port = htons(port);

            }
            return 0;
        default:
            OSAL_CHECK_ERROR(MODULE_NET, 1, 2, return_err, "Unsupport protocol!\n");
    }

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Send
 *         socket数据发送
 *
 * @param[in]  link   连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 * @param[in]  buf    需要发送数据buf指针
 * @param[in]  len    需要发送的数据长度
 * @param[in]  tout   超时设置， 单位ms
 *
 * @return
 *          0    -> select超时
 *          -1    -> select错误
 *          -2    -> 发送失败
 *          -3    -> 不支持的选项
 *          >0    -> 成功发送字节数
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Send(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout)
{
    OSAL_LINK_T *this_link = (OSAL_LINK_T *)link;
    OS_S32 ret = 0;

    switch (this_link->type) {
        case NET_TYPE_TCPSVR:
        case NET_TYPE_TCPCLI:
            ret = OSAL_tcp_send(this_link, buf, len, tout);
            break;
        case NET_TYPE_UDP:
            ret = OSAL_udp_send(this_link, buf, len, tout);
            break;
        default:
            LOGE(MODULE_NET, "Unsupport protocol!\n");
            ret = -3;
            break;
    }

    return ret;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Receive
 *         socket数据接收
 *
 * @param[in]  link     连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 * @param[in]  buf      接收数据的buffer指针
 * @param[in]  len      接收数据长度
 * @param[in]  tout     超时设置， 单位ms
 *
 * @return
 *          0    -> select超时
 *          -1    -> select错误
 *          -2    -> 接收失败
 *          -3    -> 不支持的选项
 *          >0    -> 成功接收字节数
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Receive(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout)
{
    OSAL_LINK_T *this_link = (OSAL_LINK_T *)link;
    OS_S32 ret = 0;

    switch (this_link->type) {
        case NET_TYPE_TCPSVR:
        case NET_TYPE_TCPCLI:
            ret = OSAL_tcp_receive(this_link, buf, len, tout);
            break;
        case NET_TYPE_UDP:
            ret = OSAL_udp_receive(this_link, buf, len, tout);
            break;
        default:
            LOGE(MODULE_NET, "Unsupport protocol!\n");
            ret = -3;
            break;
    }

    return ret;
}

static OS_S8 *get_name(OS_S8 *name, OS_S8 *p)
{
    while (isspace(*p)) {
        p++;
    }
    while (*p) {
        if (isspace(*p)) {
            break;
        }
        if (*p == ':') {    /* could be an alias */
            OS_S8 *dot = p, *dotname = name;
            *name++ = *p++;
            while (isdigit(*p)) {
                *name++ = *p++;
            }
            if (*p != ':') {    /* it wasn't, backup */
                p = dot;
                name = dotname;
            }
            if (*p == '\0') {
                return NULL;
            }
            p++;
            break;
        }
        *name++ = *p++;
    }
    *name++ = '\0';
    return p;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_IF_NAME
 *         获取网络设备
 *
 * @param[out] info  网络界面相关信息的结构体指针，不能为NULL
 *
 * @return
 *          0     -> 成功
 *          1     -> 非法参数
 *          2     -> 打开/proc/net/dev失败
 */
/* --------------------------------------------------------------------------*/
OS_U32 OSAL_NET_Get_IF_NAME(OSAL_NET_IF_INFO_T *info)
{
    struct interface *ife;
    FILE *fp;
    OS_S8 buf[512];
    OS_S8 name[IFNAMSIZ];
    OS_U32 netdev_cnt = 0;
    OS_S8 *retStr = NULL;
    OS_U32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!info), 1, return_err, "Invalid argument! (%p)\n", info);

    /* 打开/proc/net/dev文件 */
    fp = fopen(PATH_PROCNET_DEV, "r");
    /* 打开文件错误检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!fp), 2, return_err, "Open %s failed! (Error: %p)", fp);

    retStr = fgets(buf, sizeof(buf), fp);
    retStr = fgets(buf, sizeof(buf), fp);

    while (fgets(buf, sizeof(buf), fp)) {
        get_name(name, buf);
        /* 储存读取的数据到info结构体 */
        strcpy(info->if_name[netdev_cnt].name, name);
        netdev_cnt++;
    }
    /* 网络设备数量 */
    info->if_num = netdev_cnt;

    fclose(fp);
    return 0;

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_net_search_same_network_device
 *         搜索同一网段的网络设备
 *
 * @param[in] gateway   网关地址
 *
 * @return
 *          0     -> 成功
 *          1     -> socket创建失败
 *          2     -> SIOCGIFCONF失败
 *          3     -> 获取IP地址失败
 *          4     -> 获取子网掩码失败
 *          5     -> 未找到设备
 */
/* --------------------------------------------------------------------------*/
static OS_U32 OSAL_net_search_same_network_device(OS_S8 *gateway)
{
    struct ifreq buf[IFNAMSIZ];
    struct ifconf ifc;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;
    OS_S32 if_num = 0;
    in_addr_t ip_addr = 0;
    in_addr_t netmask = 0;
    OS_S8 *ip_addr_str = NULL, *ip_addr_str_next = NULL;
    OS_S8 *netmask_str = NULL, *netmask_next = NULL;
    OS_S32 ip_addr_int[4] = {};
    OS_S32 netmask_int[4] = {};
    OS_U32 i = 0;
    OS_S8 gw_addr_str[16] = {};
    OS_S32 gw_addr_int[4] = {};
    OS_S8 *gw_addr_str_next = NULL;
    OS_S8 gw_save[16] = {};
    OS_U32 link_status = 0;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 1, return_err, "Socket create failed! (Error: %d)\n", skfd);

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;

    /* 获取存在IP的网络设备 */
    ret = ioctl(skfd, SIOCGIFCONF, (char *)&ifc);
    OSAL_CHECK_ERROR(MODULE_NET, ret, 2, return_err, "Call SIOCGIFCONF failed! (Error: %d)\n", ret);

    if_num = ifc.ifc_len / sizeof(struct ifreq);
    strcpy(gw_save, gateway);
    while (if_num-- > 0) {
        /* 获取IP地址 */
        ret = OSAL_NET_Get_IP_Addr(buf[if_num].ifr_name, &ip_addr);
        OSAL_CHECK_ERROR(MODULE_NET, ret, 3, return_err, "%s get ip address failed! (Error: %08x)\n", buf[if_num].ifr_name, ret);
        strcpy(gw_addr_str, gw_save);

        /* 获取子网掩码 */
        ret = OSAL_NET_Get_Subnet_Mask(buf[if_num].ifr_name, &netmask);
        OSAL_CHECK_ERROR(MODULE_NET, ret, 4, return_err, "%s get subnet mask failed! (Error: %08x)\n", buf[if_num].ifr_name, ret);

        /* 字符串地址转int */
        ip_addr_str = inet_ntoa(*((struct in_addr*)&ip_addr));
        ip_addr_int[0] = atoi(strtok(ip_addr_str, "."));
        i = 0;
        while ((ip_addr_str_next = strtok(NULL, "."))) {
            ip_addr_int[++i] = atoi(ip_addr_str_next);
        }
        netmask_str = inet_ntoa(*((struct in_addr*)&netmask));
        netmask_int[0] = atoi(strtok(netmask_str, "."));
        i = 0;
        while ((netmask_next = strtok(NULL, "."))) {
            netmask_int[++i] = atoi(netmask_next);
        }
        gw_addr_int[0] = atoi(strtok(gw_addr_str, "."));
        i = 0;
        while ((gw_addr_str_next = strtok(NULL, "."))) {
            gw_addr_int[++i] = atoi(gw_addr_str_next);
        }

        /* 对比网段地址 */
        OS_U32 flag = 0;
        for (i = 0; i < 4; i++) {
            if ((ip_addr_int[i] & netmask_int[i]) == (gw_addr_int[i] & netmask_int[i])) {
                flag |= 0;
            } else {
                flag |= 1;
            }
        }
        if (flag) {
            continue;
        } else {
            /* 检查设备连接状态 */
            ret = OSAL_NET_Get_Link_Status(buf[if_num].ifr_name, &link_status);
            if (ret) {
                LOGE(MODULE_NET, "Failed to get link status! (Error: 0x%08x)\n", ret);
            }

            if (!link_status) {
                ret = OSAL_NET_Set_Link_Status(buf[if_num].ifr_name, OSAL_NET_LINK_UP);
                if (ret) {
                    LOGE(MODULE_NET, "Failed to set link status! (Error: 0x%08x)\n", ret);
                }
            }

            close(skfd);
            return 0;
        }
    }

    /* 未找到设备 */
    ret = 5;
    OSAL_CHECK_ERROR(MODULE_NET, ret, 5, return_err, "Device not found! (Error: %d)\n", ret);

    /* 函数执行异常值返回 */
return_err:
    if (ret > 2) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_IP_Addr
 *         获取网络IP地址
 *
 * @param[in]  ifname 网络界面名称, 例如"eth0", 不能为NULL
 * @param[out] ipaddr 返回IP地址的指针, 外部分配空间, 不能为NULL
 *
 * @return
 *          0 -> 获取IP地址成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCGIFADDR失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_IP_Addr(OS_S8 *ifname, in_addr_t *ipaddr)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;
    struct sockaddr_in *saddr = NULL;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!ipaddr)), 1, return_err, "Invalid argument! (%p, %p)\n", ifname, ipaddr);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ret = ioctl(skfd, SIOCGIFADDR, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCGIFADDR failed! (Error: %d)\n", ret);
    close(skfd);

    saddr = (struct sockaddr_in *) &ifr.ifr_addr;
    *ipaddr = saddr->sin_addr.s_addr;

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_IP_Addr
 *         设置网络IP地址
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  addr   需要设置的IP地址
 *
 * @return
 *          0 -> 设置IP地址成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCSIFADDR失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_IP_Addr(OS_S8 *ifname, in_addr_t addr)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!ifname), 1, return_err, "Invalid argument! (%p, %x)\n", ifname, addr);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    pf_addr.sin_addr.s_addr = addr;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    memcpy((OS_S8 *) &ifr.ifr_addr, (OS_S8 *) &pf_addr, sizeof(struct sockaddr));
    ret = ioctl(skfd, SIOCSIFADDR, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCSIFADDR failed! (Error: %d)\n", ret);
    close(skfd);

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_MAC_Addr
 *         获取网络MAC地址
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[out] mac    存放MAC地址的内存指针，需要调用者分配内存，不能为NULL
 *
 * @return
 *          0 -> 获取MAC地址成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCGIFHWADDR失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_MAC_Addr(OS_S8 *ifname, OS_U8 *mac)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!mac)), 1, return_err, "Invalid argument! (%p, %p)\n", ifname, mac);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ret = ioctl(skfd, SIOCGIFHWADDR, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCGIFHWADDR failed! (Error: %d)\n", ret);
    close(skfd);

    memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_MAC_Addr
 *         修改MAC地址, 设置MAC可能会导致默认网关失效
 *
 * @param[in]  ifname 网络界面名称, 例如"eth0", 不能为NULL
 * @param[in]  mac    需要修改的MAC地址指针
 *
 * @return
 *          0 -> 设置MAC地址成功
 *          1 -> 非法参数
 *          2 -> 获取网络设备状态失败
 *          3 -> 网络设备关闭失败
 *          4 -> socket打开失败
 *          5 -> SIOCGIFHWADDR失败
 *          6 -> SIOCSIFHWADDR失败
 *          7 -> 设置网络设备状态失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_MAC_Addr(OS_S8 *ifname, OS_U8 *mac)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;
    OS_U32 link_status = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!mac)), 1, return_err, "Invalid argument! (%p, %p)\n", ifname, mac);

#if (!defined CONFIG_TARGET_CPU_TK1) && (!defined CONFIG_TARGET_CPU_TX1) && (!defined CONFIG_TARGET_CPU_TX2) //Hi3519_link_backup && down
    /* TX1不需要down设备即可设置MAC，HI3519需要down设备才能设置MAC */
    /* 备份网络设备状态 */
    ret = OSAL_NET_Get_Link_Status(ifname, &link_status);
    OSAL_CHECK_ERROR(MODULE_NET, ret, 2, return_err, "%s get link status failed! (Error: %08x)\n", ifname, ret);

    /* 关闭网络设备以设置MAC */
    ret = OSAL_NET_Set_Link_Status(ifname, OSAL_NET_LINK_DOWN);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "%s link down failed! (Error: %08x)\n", ifname, ret);
#endif //Hi3519_link_backup && down

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 4, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    ret = ioctl(skfd, SIOCGIFHWADDR, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 5, return_err, "Call SIOCGIFHWADDR failed! (Error: %d)\n", ret);

    memcpy(ifr.ifr_ifru.ifru_hwaddr.sa_data, mac, IFHWADDRLEN);
    ret = ioctl(skfd, SIOCSIFHWADDR, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 6, return_err, "Call SIOCSIFHWADDR failed! (Error: %d)\n", ret);
    close(skfd);

#if (!defined CONFIG_TARGET_CPU_TK1) && (!defined CONFIG_TARGET_CPU_TX1) && (!defined CONFIG_TARGET_CPU_TX2) //Hi3519_link_restore
    /* 恢复网络设备状态 */
    ret = OSAL_NET_Set_Link_Status(ifname, link_status);
    OSAL_CHECK_ERROR(MODULE_NET, ret, 7, return_err, "%s restore link status %s failed! (Error: %08x)\n", ifname, ret,
                     link_status ? "Up" : "Down");
#endif //Hi3519_link_restore

    return 0;

return_err:
    if (3 == ret || 4 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Subnet_Mask
 *         获取子网掩码
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[out] netmask 子网掩码输出指针，需要调用者分配空间，不能为NULL
 *
 * @return
 *          0 -> 获取子网掩码成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCGIFNETMASK失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Subnet_Mask(OS_S8 *ifname, in_addr_t *netmask)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;
    struct sockaddr_in *saddr = NULL;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!netmask)), 1, return_err,
                     "Invalid argument! (%p, %p)\n", ifname, netmask);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ret = ioctl(skfd, SIOCGIFNETMASK, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCGIFNETMASK failed! (Error: %d)\n", ret);
    close(skfd);

    saddr = (struct sockaddr_in *)&ifr.ifr_addr;
    *netmask = saddr->sin_addr.s_addr;

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Subnet_Mask
 *         设置子网掩码
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  addr   需要设置的子网掩码
 *
 * @return
 *          0 -> 设置子网掩码成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCSIFNETMASK失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Subnet_Mask(OS_S8 *ifname, in_addr_t addr)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!ifname), 1, return_err, "Invalid argument! (%p, %x)\n", ifname, addr);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    pf_addr.sin_addr.s_addr = addr;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    memcpy((OS_S8 *) &ifr.ifr_addr, (OS_S8 *) &pf_addr, sizeof(struct sockaddr));
    ret = ioctl(skfd, SIOCSIFNETMASK, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCSIFNETMASK failed! (Error: %d)\n", ret);
    close(skfd);

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_net_search_gateway
 *         搜索网关
 *
 * @param[in]   buf       网关文件
 * @param[out]  gate_addr 网关地址存放指针
 *
 * @return
 *          0 -> 成功
 *          1 -> 非法参数
 *          2 -> 失败
 */
/* --------------------------------------------------------------------------*/
static OS_S32 OSAL_net_search_gateway(OS_S8 *buf, in_addr_t *gate_addr)
{
    OS_S8 iface[16];
    OS_ULONG dest = 0;
    OS_ULONG gate = 0;
    OS_S32 iflags = 0;
    OS_U32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, ((!buf) || (!gate_addr)), 1, return_err, "Invalid argument! (%p, %p)\n", buf, gate_addr);

    sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
    if ((iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
        *gate_addr = gate;
        return 0;
    }
    ret = 2;

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Gateway
 *         获取网关
 *
 * @return
 *          0 -> 获取IP地址成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> 查找网关失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Gateway(in_addr_t *gateway)
{
    in_addr_t gate_addr;
    OS_U32 ret = 0;
    OS_S8 buff[132];
    OS_S8 *retStr = NULL;
    FILE *fp = NULL;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!gateway), 1, return_err, "Invalid argument! (%p)\n", gateway);

    fp = fopen(PROCNET_ROUTE_PATH, "r");
    OSAL_CHECK_ERROR(MODULE_NET, (!fp), 2, return_err, "Open %s failed! (Error: %p)\n"
                     "INET (IPv4) not configured in this system. \n",
                     PROCNET_ROUTE_PATH, fp);

    retStr = fgets(buff, 130, fp);
    while (fgets(buff, 130, fp) != NULL) {
        if (OSAL_net_search_gateway(buff, &gate_addr) == 0) {
            fclose(fp);
            *gateway = gate_addr;
            return 0;
        }
    }
    ret = 3;
    fclose(fp);

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Add_Gateway
 *         添加网关, 仅有效至同网段的设备全部down之前
 *
 * @param[in]  addr 需要增加的网关
 *
 * @return
 *          0 -> 成功
 *          1 -> 网络设备不可达
 *          2 -> socket打开失败
 *          3 -> SIOCADDRT失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Add_Gateway(in_addr_t addr)
{
    struct rtentry rt = {0};
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* 搜索当前同一网段的网络设备 */
    ret = OSAL_net_search_same_network_device(inet_ntoa(*((struct in_addr*)&addr)));
    OSAL_CHECK_ERROR(MODULE_NET, ret, 1, return_err, "Network is unreachable! (Error: %08x)\n", ret);

    /* Clean out the RTREQ structure. */
    memset((OS_S8 *) &rt, 0, sizeof(struct rtentry));

    /* Fill in the other fields. */
    rt.rt_flags = (RTF_UP | RTF_GATEWAY);

    rt.rt_dst.sa_family = PF_INET;
    rt.rt_genmask.sa_family = PF_INET;

    pf_addr.sin_addr.s_addr = addr;
    memcpy((OS_S8 *) &rt.rt_gateway, (OS_S8 *) &pf_addr, sizeof(struct sockaddr));

    /* Create a socket to the INET kernel. */
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    /* Tell the kernel to accept this route. */
    ret = ioctl(skfd, SIOCADDRT, &rt);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call SIOCADDRT failed! (Error: %d)\n", ret);

    /* Close the socket. */
    close(skfd);

    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Del_Gateway
 *         删除网关
 *
 * @param[in]  addr gateway 需要删除的网关
 *
 * @return
 *          0 -> 成功
 *          1 -> socket打开失败
 *          2 -> SIOCDELRT失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Del_Gateway(in_addr_t addr)
{
    struct rtentry rt = {0};
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* Clean out the RTREQ structure. */
    memset((OS_S8 *) &rt, 0, sizeof(struct rtentry));

    /* Fill in the other fields. */
    rt.rt_flags = (RTF_UP | RTF_GATEWAY);

    rt.rt_dst.sa_family = PF_INET;
    rt.rt_genmask.sa_family = PF_INET;

    pf_addr.sin_addr.s_addr = addr;
    memcpy((OS_S8 *) &rt.rt_gateway, (OS_S8 *) &pf_addr, sizeof(struct sockaddr));

    /* Create a socket to the INET kernel. */
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 1, return_err, "Socket create failed! (Error: %d)\n", skfd);

    /* Tell the kernel to accept this route. */
    ret = ioctl(skfd, SIOCDELRT, &rt);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 2, return_err, "Call SIOCDELRT failed! (Error: %d)\n", ret);

    /* Close the socket. */
    close(skfd);

    return 0;

return_err:
    if (2 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Gateway
 *         设置网关
 *
 * @param[in]  addr 需要设置的网关
 *
 * @return
 *          0      -> 成功
 *          其他值 -> 失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Gateway(in_addr_t addr)
{
    in_addr_t gate_addr;
    OS_S8 buff[132];
    OS_S8 *retStr = NULL;
    OS_U32 ret = 0;

    FILE *fp = fopen(PROCNET_ROUTE_PATH, "r");
    OSAL_CHECK_ERROR(MODULE_NET, (!fp), 1, return_err, "Open %s failed! (Error: %p)\n"
                     "INET (IPv4) not configured in this system. \n",
                     PROCNET_ROUTE_PATH, fp);
    retStr = fgets(buff, 130, fp);
    while (fgets(buff, 130, fp) != NULL) {
        if (OSAL_net_search_gateway(buff, &gate_addr) == 0) {
            OSAL_NET_Del_Gateway(gate_addr);
        }
    }
    fclose(fp);
    return OSAL_NET_Add_Gateway(addr);

return_err:
    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Link_Speed
 *         获取当前网络连接速度
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[out] speed  返回当前网络连接速度
 *
 * @return
 *          0      -> 成功
 *          1      -> 非法参数
 *          2      -> socket创建失败
 *          3      -> ETHTOOL_GSET失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Link_Speed(OS_S8 *ifname, OS_U32 *speed)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;
    struct ethtool_cmd ecmd = {0};

    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!speed)), 1, return_err, "Invalid argument! (%p, %p)\n", ifname, speed);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ecmd.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (OS_S8*) &ecmd;
    ret = ioctl(skfd, SIOCETHTOOL, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call ETHTOOL_GSET failed! (Error: %d)\n", ret);

    *speed = ethtool_cmd_speed(&ecmd);

    close(skfd);
    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Link_Status
 *         获取网络接口是否连接状态
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[out] up_down 返回当前连接状态  1->Link up : 0->Link down
 *
 * @return
 *          0      -> 成功
 *          1      -> 非法参数
 *          2      -> socket创建失败
 *          3      -> ETHTOOL_GSET失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Link_Status(OS_S8 *ifname, OS_U32 *up_down)
{
    struct ifreq ifr;
    struct ethtool_value edata = {0};
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    OSAL_CHECK_ERROR(MODULE_NET, ((!ifname) || (!up_down)), 1, return_err, "Invalid argument! (%p, %p)\n", ifname, up_down);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (OS_S8*) &edata;

    ret = ioctl(skfd, SIOCETHTOOL, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Call ETHTOOL_GSET failed! (Error: %d)\n", ret);

    *up_down = edata.data ? 1 : 0;

    close(skfd);
    return 0;

return_err:
    if (3 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Link_Status
 *         设置网络接口连接状态
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  status 设置当前连接状态  1->Link up : 0->Link down
 *
 * @return
 *          0      -> 成功
 *          1      -> 非法参数
 *          2      -> socket创建失败
 *          3      -> 未找到网络界面
 *          4      -> SIOCSIFFLAGS失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Link_Status(OS_S8 *ifname, OS_S16 status)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!ifname), 1, return_err, "Invalid argument! (%p, %d)\n", ifname, status);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ret = ioctl(skfd, SIOCGIFFLAGS, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "%s: unknown interface: %s (Error: %d)\n",
                     ifname, strerror(errno), ret);
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    /* 设置状态判断 */
    if (status) {
        ifr.ifr_flags |= status;    //Link up
    } else {
        ifr.ifr_flags &= status;    //Link down
    }
    ret = ioctl(skfd, SIOCSIFFLAGS, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 4, return_err, "Call SIOCSIFFLAGS failed! (Error: %d)\n", ret);

    close(skfd);
    return 0;

return_err:
    if (3 == ret || 4 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Force_Link_Speed
 *         限制网络连接速度
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  speed  需要设置的网络速度 0(自协商)/10/100/1000
 *
 * @return
 *          0 -> 成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> ETHTOOL_GSET失败
 *          4 -> ETHTOOL_SSET失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Force_Link_Speed(OS_S8 *ifname, OS_U32 speed)
{
    struct ifreq ifr;
    OS_S32 skfd = 0;
    struct ethtool_cmd ecmd = {0};
    OS_U32 advertising_wanted = 0;
    OS_S32 ret = 0;

    OSAL_CHECK_ERROR(MODULE_NET, ((ifname == NULL) || ((speed != 0) && (speed != 10) && (speed != 100) && (speed != 1000))),
                     1, return_err, "Invalid argument\n");

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Socket create failed! (Error: %d)\n", skfd);

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ecmd.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (OS_S8*) &ecmd;
    ret = ioctl(skfd, SIOCETHTOOL, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 3, return_err, "Socket create failed! (Error: %d)\n", ret);

    ecmd.duplex  = DUPLEX_FULL;
    if (speed) {
        /* Changed speed */
        ethtool_cmd_speed_set(&ecmd, speed);
        if (speed == SPEED_10) {
            advertising_wanted = ADVERTISED_10baseT_Full;
        } else if (speed == SPEED_100) {
            advertising_wanted = ADVERTISED_100baseT_Full;
        } else if (speed == SPEED_1000) {
            advertising_wanted = ADVERTISED_1000baseT_Full;
        }
        /* Enable all requested modes */
        ecmd.advertising = (ecmd.advertising & ~ALL_ADVERTISED_MODES)
                           | advertising_wanted;
    } else {
        ecmd.autoneg = AUTONEG_ENABLE;
        ecmd.advertising = (ecmd.advertising & ~ALL_ADVERTISED_MODES)
                           | (ALL_ADVERTISED_MODES & ecmd.supported);
    }
    ecmd.cmd = ETHTOOL_SSET;
    ifr.ifr_data = (OS_S8*) &ecmd;

    /* Set the speed */
    ret = ioctl(skfd, SIOCETHTOOL, &ifr);
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 0), 4, return_err, "ETHTOOL_SSET fail\n");
    close(skfd);

    return 0;

return_err:
    if (3 == ret || 4 == ret) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* ping工具效验算法 */
static OS_U16 cal_chksum(OS_U16 *addr, OS_S32 len)
{
    OS_S32 nleft = len;
    OS_S32 sum = 0;
    OS_U16 *w = addr;
    OS_U16 answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(OS_U8 *)(&answer) = *(OS_U8 *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Ping
 *         ping工具
 *
 * @param[in]  ips      IP地址指针
 * @param[in]  timeout  超时时间（单位：毫秒）
 *
 * @return
 *          0 -> 成功
 *          1 -> 非法参数
 *          2 -> socket创建失败
 *          3 -> setsockopt失败
 *          4 -> 发送失败
 *          5 -> select超时
 *          6 -> 目标主机不可达
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Ping(OS_S8 *ips, OS_U32 timeout)
{
    struct timeval timeo;
    struct sockaddr_in addr;
    struct sockaddr_in from;
    struct timeval *tval = NULL;
    struct ip *iph = NULL;
    struct icmp *icmp = NULL;

    pid_t pid;
    socklen_t fromlen = 0;
    fd_set readfds;
    OS_S32 skfd = 0;
    OS_S8 sendpacket[4096] = {0};
    OS_S8 recvpacket[4096] = {0};

    OS_U32 i = 0;
    OS_U32 packsize = 0;
    OS_S32 ret = 0;
    OS_S32 maxfds = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, (!ips), 1, return_err, "Invalid argument! (%p, %d)\n", ips, timeout);

    /* 设置ip信息 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ips);

    skfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    OSAL_CHECK_ERROR(MODULE_NET, (skfd < 0), 2, return_err, "Failed to create socket! (Error: %d)\n", skfd);

    /* 设置超时时间 */
    timeo.tv_sec = timeout / 1000;
    timeo.tv_usec = (timeout % 1000) * 1000;

    ret = setsockopt(skfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
    OSAL_CHECK_ERROR(MODULE_NET, (-1 == ret), 3, return_err, "Failed to set sockopt! (Error: %d)\n", ret);

    /* 清空ping包 */
    memset(sendpacket, 0, sizeof(sendpacket));

    /* 获取pid, 作为ping的sequence id */
    pid = getpid();

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = 0;
    icmp->icmp_id = pid;
    packsize = 8 + 56;
    tval = (struct timeval *)icmp->icmp_data;
    gettimeofday(tval, NULL);
    icmp->icmp_cksum = cal_chksum((OS_U16 *)icmp, packsize);

    /* 发送数据包 */
    ret = sendto(skfd, (OS_S8 *)&sendpacket, packsize, 0, (struct sockaddr *)&addr, sizeof(addr));
    OSAL_CHECK_ERROR(MODULE_NET, (ret < 1), 4, return_err, "Failed to send package! (Error: %d)\n", ret);

    /* 接收 */
    /* 由于可能接受到其他Ping的应答消息，所以这里要用循环 */
    while (1) {
        /* 设置超时时间，这次才是真正起作用的 */
        FD_ZERO(&readfds);
        FD_SET(skfd, &readfds);
        maxfds = skfd + 1;
        ret = select(maxfds, &readfds, NULL, NULL, &timeo);
        OSAL_CHECK_ERROR(MODULE_NET, (ret <= 0), 5, return_err, "select timeout! (Error: %d)\n", ret);

        /* 接收 */
        memset(recvpacket, 0, sizeof(recvpacket));
        fromlen = sizeof(from);
        ret = recvfrom(skfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen);
        if (ret < 1) {
            break;
        }

        /* 判断是否是自己ping的回复 */
        OS_S8 *from_ip = (OS_S8 *)inet_ntoa(from.sin_addr);
        if (strcmp(from_ip, ips) != 0) {
            ret = 6;
            OSAL_CHECK_ERROR(MODULE_NET, ret, 6, return_err, "Destination Host Unreachable! (Error: %d)\n", ret);
        }

        iph = (struct ip *)recvpacket;
        icmp = (struct icmp *)(recvpacket + (iph->ip_hl << 2));

        OS_U32 len = iph->ip_hl << 2;

        /* 判断ping回复包的状态 */
        if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid) {
            /* 正常就退出循环 */
            LOGI(MODULE_NET, "%d byte from %s: icmp_seq=%u ttl=%d \n",
                 len, from_ip, icmp->icmp_seq, iph->ip_ttl);
            break;
        } else {
            /* 否则继续等待 */
            continue;
        }
    }

    /* 关闭socket */
    close(skfd);
    return 0;

return_err:
    if (ret > 2) {
        close(skfd);
    }

    return (MODULE_NET | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief OSAL_NET_Generate_Mac_Address
 *          根据输入的buffer数组 生成6字节的MAC地址
 * @param[in/out] mac       要生成的mac地址指针
 * @param[in]     mac0      第一位mac地址值, 是常量
 * @param[in]     buffer    输入的数组指针
 * @param[in]     len       输入的数组长度
 *
 * @return
 *          0 -> 成功
 *          1 -> 非法参数
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Generate_Mac_Address(OS_U8 *mac, OS_U8 mac0, OS_U8 *buffer, OS_U32 len)
{
    OS_S32 ret = 0;
    OS_U32 i = 0;
    OS_U8  checksum = 0;
    OS_U32 crc_value = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_NET, !buffer, 1, return_err, "Invalid argument! (%p)\n", buffer);

    crc_value = crc32(0, buffer, len);
    mac[0] = mac0;
    for (i = 1; i < 5; i++) {
        mac[i] = ((crc_value >> ((i - 1) * 8)) & 0xFF);
    }

    for (i = 0; i < len; i++) {
        checksum += buffer[i];
    }
    mac[5] = checksum ? checksum : (~checksum - 1);
    mac[5] &= 0xFE;

    return 0;
return_err:
    return (MODULE_NET | ret);
}

static OS_S32 app_usage(OS_S8 *app_name)
{
    LOGI(MODULE_NET,
         "\r\n           Example: %s -i eth0 -n 192.168.3.121 -m 00044b664b44 -s 255.255.255.0 -g 192.168.3.254 -l up\r\n"
         "           Options:\r\n"
         "           -h | --help          Print this message\r\n"
         "           -i | --interface     Netname\r\n"
         "           -n | --ip            IP Address\r\n"
         "           -m | --mac           MAC Address\r\n"
         "           -s | --subnet        Subnet Mask\r\n"
         "           -g | --gateway       Gateway\r\n"
         "           -l | --link          Net status(up or down)\r\n"
         "\n", app_name);
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_API_Test
 *         网络API测试函数
 *
 * @param[in]  argc
 * @param[in]  argv
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 *          2 -> 系统中未找到网络设备
 *          3 -> 输入的网络设备未找到
 *          4 -> ip地址错误
 *          5 -> 子网掩码错误
 *          6 -> 网关地址错误
 *          7 -> 恢复网络配置信息失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_API_Test(OS_S32 argc, OS_S8 **argv)
{
    struct in_addr ip_addr;
    struct in_addr subnet_mask;
    struct in_addr gateway;
    OSAL_NET_IF_INFO_T if_info;

    /* 用于存储版本号 */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    /* 临时变量 */
    OS_U32 flag = 0;
    OS_U32 speed = 0;
    OS_U32 link_speed = 0;
    OS_U32 link_status = 0;
    OS_S8 *netif = NULL;
    in_addr_t ipaddr = 0;
    in_addr_t netmask = 0;
    in_addr_t gw_addr = 0;
    OS_U8 mac[6];
    OS_U32 i = 0;
    OS_S32 ret = 0;
    OS_S8 *ip = NULL;
    OS_U8 mac_addr[6];
    OS_S8 mac_addr_str[16];
    OS_U64 mac_addr_long = 0;
    OS_S8 *mask = NULL;
    OS_S8 *gw = NULL;
    OS_S8 *status_str = NULL;
    OS_S16 status = 0;
    OS_U16 link_set_err_flag = 0;

    /* 参数解析 */
    OS_S32 c = 0;
    OS_S32 index = 0;
    const struct option long_options [] = {
        { "help",           no_argument,            NULL,           'h' },
        { "interface",      required_argument,      NULL,           'i' },
        { "ip",             required_argument,      NULL,           'n' },
        { "mac",            required_argument,      NULL,           'm' },
        { "subnet",         required_argument,      NULL,           's' },
        { "gatewat",        required_argument,      NULL,           'g' },
        { "link",           required_argument,      NULL,           'l' },
        {0, 0, 0, 0}
    };
    const char short_options [] = "n:i:m:s:g:l:h";

    /* 打印NET版本号 */
    ret = OSAL_NET_Get_Module_Version(&major_version, &minor_version, &build_version);

    if (argc == 1) {
        app_usage(argv[0]);
        return (MODULE_NET | 1);
    }

    /* 参数解析 */
    while (1) {
        c = getopt_long(argc, argv, short_options, long_options, &index);
        if (-1 == c) {
            break;
        }

        switch (c) {
            case 0: /* getopt_long() flag */
                break;
            case 'i':
                netif = optarg;
                break;
            case 'n':
                ip = optarg;
                break;
            case 'm':
                strcpy(mac_addr_str, optarg);
                break;
            case 's':
                mask = optarg;
                break;
            case 'g':
                gw = optarg;
                break;
            case 'l':
                status_str = optarg;
                if (!strncmp(optarg, "up", 2)) {
                    status = 1;
                    link_set_err_flag = 0;
                } else if (!strncmp(optarg, "down", 4)) {
                    status = 0;
                    link_set_err_flag = 0;
                } else {
                    link_set_err_flag = 1;
                }
                break;
            case 'h':
            default:
                app_usage(argv[0]);
                return 0;
        }
    }

    /* 获取网络接口名称 */
    ret = OSAL_NET_Get_IF_NAME(&if_info);
    if (!ret) {
        if (if_info.if_num > 1) {
            LOGI(MODULE_NET, "There are total %d network interfaces on system.\n", if_info.if_num);
        } else {
            LOGI(MODULE_NET, "There is total %d network interface on system.\n", if_info.if_num);
        }
        /* 列出网络接口名称 */
        LOGI(MODULE_NET, "Interface name: ");
        for (i = 0; i < if_info.if_num; i++) {
            if ((i == (if_info.if_num - 1))) {
                printf("%s \n", if_info.if_name[i].name);
            } else {
                printf("%s | ", if_info.if_name[i].name);
            }
            if (!(strcmp(if_info.if_name[i].name, netif))) {
                flag |= 1;
            } else {
                flag |= 0;
            }
        }

        /* 判断netif设备是否存在 */
        if (!flag) {
            OSAL_CHECK_ERROR(MODULE_NET, true, 3, return_err, "Device %s not found!\n", netif);
        }
        /* 获取物理连接状态 */
        if (!OSAL_NET_Get_Link_Status(netif, &link_status)) {
            LOGI(MODULE_NET, "OSAL_NET_Get_Link_Status %s is %s\n", netif, link_status ? "Up" : "Down");
            /* 获取IP地址 */
            ret = OSAL_NET_Get_IP_Addr(netif, &ipaddr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Get_IP_Addr fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Get_IP_Addr success, ip: %s\n", inet_ntoa(*((struct in_addr*)&ipaddr)));
            }
            /* 获取MAC地址 */
            ret = OSAL_NET_Get_MAC_Addr(netif, mac);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Get_MAC_Addr fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Get_MAC_Addr success, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            /* 获取子网掩码 */
            ret = OSAL_NET_Get_Subnet_Mask(netif, &netmask);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Get_Subnet_Mask fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Get_Subnet_Mask success, netmask: %s\n",
                     inet_ntoa(*((struct in_addr*)&netmask)));
            }
            /* 获取网关 */
            ret = OSAL_NET_Get_Gateway(&gw_addr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Get_Gateway fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Get_Gateway success, gateway: %s\n",
                     inet_ntoa(*((struct in_addr*)&gw_addr)));
            }

            /* 字符串地址转换 */
            ret = inet_aton(ip, &ip_addr);
            OSAL_CHECK_ERROR(MODULE_NET, (0 == ret), 4, return_err, "IP address error! (Error: %d)\n", ret);
            ret = inet_aton(mask, &subnet_mask);
            OSAL_CHECK_ERROR(MODULE_NET, (0 == ret), 5, return_err, "Subnet mask error! (Error: %d)\n", ret);
            ret = inet_aton(gw, &gateway);
            OSAL_CHECK_ERROR(MODULE_NET, (0 == ret), 6, return_err, "Gateway error! (Error: %d)\n", ret);
            /* MAC地址转换 */
            mac_addr_long = strtoll(mac_addr_str, NULL, 16);
            for (i = 0; i < 6; i++) {
                mac_addr[i] = (mac_addr_long >> ((2 * (5 - i)) * 4)) & 0xFF;
            }

            /* 设置IP地址 */
            ret = OSAL_NET_Set_IP_Addr(netif, ip_addr.s_addr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Set_IP_Addr fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Set_IP_Addr success, ip: %s\n", ip);
            }
            /* must run set_mac before set_gateway */
            /* 设置MAC地址 */
            ret = OSAL_NET_Set_MAC_Addr(netif, mac_addr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Set_MAC_Addr fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Set_MAC_Addr success, mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            }
            /* 设置子网掩码 */
            ret = OSAL_NET_Set_Subnet_Mask(netif, subnet_mask.s_addr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Set_Subnet_Mask fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Set_Subnet_Mask success, netmask: %s\n", mask);
            }
            /* 设置网关 */
            ret = OSAL_NET_Set_Gateway(gateway.s_addr);
            if (ret) {
                LOGI(MODULE_NET, "OSAL_NET_Set_Gateway fail ret 0x%08x\n", ret);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Set_Gateway success, gateway: %s\n", gw);
            }
            /* 设置网络连接状态 */
            ret = OSAL_NET_Set_Link_Status(netif, status);
            if (!link_set_err_flag) {
                if (ret) {
                    LOGI(MODULE_NET, "OSAL_NET_Set_Link_Status fail ret 0x%08x\n", ret);
                } else {
                    LOGI(MODULE_NET, "OSAL_NET_Set_Link_Status %s is %s\n", netif, status ? "Up" : "Down");
                }
            } else {
                LOGE(MODULE_NET, "\033\[1m\033[40;31mInvalid argument of link status: %s \033[0m\n", status_str);
            }

            /* 等待用户确认恢复网络配置状态 */
            LOGI(MODULE_NET,
                 "\033[1m\033[40;32mPlease enter any key to restore the network configuration! \033[0m");
            getchar();

            /* 恢复到用户设置前的网络设备配置状态 */
            ret = OSAL_NET_Set_IP_Addr(netif, ipaddr);
            ret |= OSAL_NET_Set_MAC_Addr(netif, mac);
            ret |= OSAL_NET_Set_Subnet_Mask(netif, netmask);
            ret |= OSAL_NET_Set_Gateway(gw_addr);
            ret |= OSAL_NET_Set_Link_Status(netif, link_status);
            OSAL_CHECK_ERROR(MODULE_NET, ret, 7, return_err, "Restore network configuration failed! (Error: %08x)\n", ret);
        } else {
            LOGI(MODULE_NET, "OSAL_NET_Get_Link_Status fail!\n");
        }

        /* 依次测试网络在自协商和强制速度是否可用 */
        for (i = 0; i < 4; i++) {
            switch (i) {
                case 0:
                    speed = 0;
                    LOGI(MODULE_NET, "Set %s speed to auto-negotiation\n", netif);
                    break;
                case 1:
                    speed = 10;
                    LOGI(MODULE_NET, "Set %s speed to %dMbps\n", netif, speed);
                    break;
                case 2:
                    speed = 100;
                    LOGI(MODULE_NET, "Set %s speed to %dMbps\n", netif, speed);
                    break;
                case 3:
                default:
                    speed = 1000;
                    LOGI(MODULE_NET, "Set %s speed to %dMbps\n", netif, speed);
                    break;
            }
            /* 设置网络连接速度 */
            if (OSAL_NET_Force_Link_Speed(netif, speed)) {
                LOGI(MODULE_NET, "Failed\n");
            } else {
                LOGI(MODULE_NET, "Success\n");
                ret |= 1;
            }
            sleep(6);
            /* 回读网络连接速度 */
            if (!OSAL_NET_Get_Link_Speed(netif, &link_speed)) {
                LOGI(MODULE_NET, "OSAL_NET_Get_Link_Speed Current speed is %dMbps\n", link_speed);
            } else {
                LOGI(MODULE_NET, "OSAL_NET_Get_Link_Speed Failed\n");
            }
            sleep(16);
        }
    } else {
        OSAL_CHECK_ERROR(MODULE_NET, true, 2, return_err, "Network interface not found! (Error: %08x)\n", ret);
    }

    return 0;

return_err:
    return MODULE_NET | ret;
}
