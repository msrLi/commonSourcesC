/**
 * @file osal_net.h
 * @brief  网络模块API
 * @author binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-06-05
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */

#ifndef __OSAL_NET_H__
#define __OSAL_NET_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* define & enum
******************************************************************************/
#define OSAL_INADDR_ANY         (0)
#define OSAL_INADDR_NONE        (0xffffffff)
#define OSAL_INADDR_BROADCAST   (0xffffffff)

#define MAX_INTERFACE_NUM       (16)
#define INTERFACE_NAME_SIZE     (16)

enum OSAL_NET_FLAG {
    OSAL_NET_FL_BROADCAST = 1,
};

/******************************************************************************
* type
******************************************************************************/
typedef OS_U32 OSAL_INADDR_T;
typedef OS_S32(*OSAL_Socket_Proc_Handler)(OS_HANDLE link, OS_HANDLE arg);

typedef struct OSAL_NET_IF_NAME_TAG {
    OS_S8 name[INTERFACE_NAME_SIZE];
} OSAL_NET_IF_NAME_T;

typedef struct OSAL_NET_IF_INFO_TAG {
    OS_U32 if_num;
    OSAL_NET_IF_NAME_T if_name[MAX_INTERFACE_NUM];
} OSAL_NET_IF_INFO_T;

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
OS_S32 OSAL_NET_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

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
OS_S32 OSAL_NET_Create_TCP_Server(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U16 backlog);

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
OS_S32 OSAL_NET_Create_TCP_Client(OS_HANDLE *link, OS_S8 *addr, OS_U16 port);

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
OS_S32 OSAL_NET_Create_UDP(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U32 flags);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Delete
 *         删除相应的socket连接
 *
 * @param[in]  link   连接句柄指针， OSAL_Socket_Create_xxx的第一个参数返回句柄
 *
 * @return
 *          0      -> 删除成功
 *          其他值 -> 删除失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Delete(OS_HANDLE link);

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
OS_S32 OSAL_NET_Socket_Process(OS_HANDLE link, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg);

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
OS_S32 OSAL_NET_Socket_Set_Peer(OS_HANDLE link, OS_S8 *ip, OS_U32 addr, OS_U16 port);

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
 *          0     -> select超时
 *          1     -> select错误
 *          2     -> 发送失败
 *          3     -> 不支持的选项
 *          其他值 -> 成功发送字节数
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Send(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout);

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
 *          0     -> select超时
 *          1     -> select错误
 *          2     -> 接收失败
 *          3     -> 不支持的选项
 *          其他值 -> 成功接收字节数
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Receive(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout);

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
OS_U32 OSAL_NET_Get_IF_NAME(OSAL_NET_IF_INFO_T *info);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_IP_Addr
 *         获取网络IP地址
 *
 * @param[in]  ifname 网络界面名称, 例如"eth0", 不能为NULL
 * @param[out] ipaddr 返回IP地址的指针, 外部分配空间, 不能为NULL
 * @return
 *          0 -> 获取IP地址成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCGIFADDR失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_IP_Addr(OS_S8 *ifname, in_addr_t *ipaddr);

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
OS_S32 OSAL_NET_Set_IP_Addr(OS_S8 *ifname, in_addr_t addr);

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
OS_S32 OSAL_NET_Get_MAC_Addr(OS_S8 *ifname, OS_U8 *mac);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_MAC_Addr
 *         修改MAC地址, 设置MAC可能会导致默认网关重置
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
OS_S32 OSAL_NET_Set_MAC_Addr(OS_S8 *ifname, OS_U8 *mac);

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
OS_S32 OSAL_NET_Get_Subnet_Mask(OS_S8 *ifname, in_addr_t *netmask);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Subnet_Mask
 *         设置子网掩码
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  addr   需要设置的子网掩码
 * @return
 *          0 -> 设置子网掩码成功
 *          1 -> 非法参数
 *          2 -> socket打开失败
 *          3 -> SIOCSIFNETMASK失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Subnet_Mask(OS_S8 *ifname, in_addr_t addr);

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
OS_S32 OSAL_NET_Get_Gateway(in_addr_t *gateway);

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
OS_S32 OSAL_NET_Add_Gateway(in_addr_t addr);

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
OS_S32 OSAL_NET_Del_Gateway(in_addr_t addr);

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
OS_S32 OSAL_NET_Set_Gateway(in_addr_t addr);

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
OS_S32 OSAL_NET_Get_Link_Speed(OS_S8 *ifname, OS_U32 *speed);

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
 *          其他值 -> 失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Link_Status(OS_S8 *ifname, OS_U32 *up_down);

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
OS_S32 OSAL_NET_Set_Link_Status(OS_S8 *ifname, OS_S16 status);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Force_Link_Speed
 *         限制网络连接速度
 *
 * @param[in]  ifname 网络界面名称（eth0）
 * @param[in]  speed  需要设置的网络速度 0(自协商)/10/100/1000
 * @return
 *          0 -> 成功
 *          1 -> 参数不合法
 *          2 -> socket打开失败
 *          3 -> ETHTOOL_GSET失败
 *          4 -> ETHTOOL_SSET失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Force_Link_Speed(OS_S8 *ifname, OS_U32 speed);

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
OS_S32 OSAL_NET_Ping(OS_S8 *ips, OS_U32 timeout);

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
OS_S32 OSAL_NET_Generate_Mac_Address(OS_U8 *mac, OS_U8 mac0, OS_U8 *buffer, OS_U32 len);

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
 *          1 -> ip地址错误
 *          2 -> 子网掩码错误
 *          3 -> 网关地址错误
 *          4 -> 恢复网络配置信息失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_API_Test(OS_S32 argc, OS_S8 **argv);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_NET_H__
