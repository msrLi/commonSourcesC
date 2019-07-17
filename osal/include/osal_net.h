/**
 * @file osal_net.h
 * @brief  ����ģ��API
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
 *         ��ȡNETģ��ĵ�ǰ�汾
 *
 * @param[out]      major_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[out]      minor_version  ģ���Ӱ汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 * @param[out]      build_version  ģ�����汾�ŷ���ָ�룬�ⲿ������Чָ�룬����OS_U32
 *
 * @return
 *          0 -> �汾�Ż�ȡ�ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> �汾�Ÿ�ʽ����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_TCP_Server
 *         ����TCP�����
 *
 * @param[out]  link      ����˾��ָ�룬�ϲ���Ҫ����ά�����Ա��������ʶ������
 * @param[in]   addr      ��Ҫ��ص�IP��ַ�� NULL��ʾ��������IP��ַ
 * @param[in]   port      ��Ҫ��صĶ˿�
 * @param[in]   backlog   ֧��ͬʱ������
 *
 * @return
 *          0 -> �ɹ���������
 *          1 -> ��������
 *          2 -> socket����
 *          3 -> bindʧ��
 *          4 -> listenʧ��
 *          5 -> ������Դʧ��
 *          6 -> �����߳�ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_TCP_Server(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U16 backlog);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_TCP_Client
 *         ����TCP�ͻ���
 *
 * @param[out]  link      �ͻ��˾��ָ�룬�ϲ���Ҫ����ά�����Ա��������ʶ������
 * @param[in]   addr      ��Ҫ���ӵ�IP��ַ�� ����ΪNULL
 * @param[in]   port      ��Ҫ���ӵĶ˿�
 *
 * @return
 *          0 -> �ɹ���������
 *          1 -> ��������
 *          2 -> IP��ַ���Ϸ�
 *          3 -> socket����
 *          4 -> connect�����ʧ��
 *          5 -> ������Դʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_TCP_Client(OS_HANDLE *link, OS_S8 *addr, OS_U16 port);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Create_UDP
 *         ����UDP����
 *
 * @param[out]  link      ���Ӿ��ָ�룬�ϲ���Ҫ����ά�����Ա��������ʶ������
 * @param[in]   addr      ��Ҫ���ӵ�IP��ַ�� NULL��ʾ��������IP��ַ
 * @param[in]   port      ��Ҫ���ӵĶ˿�
 * @param[in]   flags     ������־
 *
 * @return
 *          0 -> �ɹ���������
 *          1 -> ��������
 *          2 -> socket����
 *          3 -> ���ù㲥������ʧ��
 *          4 -> bindʧ��
 *          5 -> ������Դʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Create_UDP(OS_HANDLE *link, OS_S8 *addr, OS_U16 port, OS_U32 flags);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Delete
 *         ɾ����Ӧ��socket����
 *
 * @param[in]  link   ���Ӿ��ָ�룬 OSAL_Socket_Create_xxx�ĵ�һ���������ؾ��
 *
 * @return
 *          0      -> ɾ���ɹ�
 *          ����ֵ -> ɾ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Delete(OS_HANDLE link);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Process
 *         ���socket��ִ�лص�����
 *
 * @param[in]  link   ���Ӿ��ָ�룬 OSAL_Socket_Create_xxx�ĵ�һ���������ؾ��
 * @param[in]  proc ��Ҫִ�еĻص�����ָ�룬
 * @param[in]  arg  ��Ҫ�ص������Ĳ���ָ��
 *
 * @return
 *          0      -> �ص��ɹ�
 *          1      -> ��֧�ֵĲ���
 *          ����ֵ -> �ص�ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Process(OS_HANDLE link, OSAL_Socket_Proc_Handler proc, OS_HANDLE arg);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Set_Peer
 *         �����޸�Զ�˵�ַ����ʱֻ֧��UDP���ӣ� ip��addrֻ��֧��һ��
 *
 * @param[in]  link   ���Ӿ��ָ�룬 OSAL_Socket_Create_xxx�ĵ�һ���������ؾ��
 * @param[in]  ip     Զ��IP��ַ, ����"192.168.1.1"�� NULL��ʾ��addr��ȡ
 * @param[in]  addr   Զ��IP��ַ, ֻ��ipΪNULLʱ���˲�������Ч
 * @param[in]  port   Զ�˶˿�
 *
 * @return
 *          0 -> ���óɹ�
 *          1 -> ����ʧ��
 *          2 -> ��֧�ֵĲ���
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Set_Peer(OS_HANDLE link, OS_S8 *ip, OS_U32 addr, OS_U16 port);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Send
 *         socket���ݷ���
 *
 * @param[in]  link   ���Ӿ��ָ�룬 OSAL_Socket_Create_xxx�ĵ�һ���������ؾ��
 * @param[in]  buf    ��Ҫ��������bufָ��
 * @param[in]  len    ��Ҫ���͵����ݳ���
 * @param[in]  tout   ��ʱ���ã� ��λms
 *
 * @return
 *          0     -> select��ʱ
 *          1     -> select����
 *          2     -> ����ʧ��
 *          3     -> ��֧�ֵ�ѡ��
 *          ����ֵ -> �ɹ������ֽ���
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Send(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Socket_Receive
 *         socket���ݽ���
 *
 * @param[in]  link     ���Ӿ��ָ�룬 OSAL_Socket_Create_xxx�ĵ�һ���������ؾ��
 * @param[in]  buf      �������ݵ�bufferָ��
 * @param[in]  len      �������ݳ���
 * @param[in]  tout     ��ʱ���ã� ��λms
 *
 * @return
 *          0     -> select��ʱ
 *          1     -> select����
 *          2     -> ����ʧ��
 *          3     -> ��֧�ֵ�ѡ��
 *          ����ֵ -> �ɹ������ֽ���
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Socket_Receive(OS_HANDLE link, OS_HANDLE buf, OS_U32 len, OS_U32 tout);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_IF_NAME
 *         ��ȡ�����豸
 *
 * @param[out] info  ������������Ϣ�Ľṹ��ָ�룬����ΪNULL
 *
 * @return
 *          0     -> �ɹ�
 *          1     -> �Ƿ�����
 *          2     -> ��/proc/net/devʧ��
 */
/* --------------------------------------------------------------------------*/
OS_U32 OSAL_NET_Get_IF_NAME(OSAL_NET_IF_INFO_T *info);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_IP_Addr
 *         ��ȡ����IP��ַ
 *
 * @param[in]  ifname �����������, ����"eth0", ����ΪNULL
 * @param[out] ipaddr ����IP��ַ��ָ��, �ⲿ����ռ�, ����ΪNULL
 * @return
 *          0 -> ��ȡIP��ַ�ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> SIOCGIFADDRʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_IP_Addr(OS_S8 *ifname, in_addr_t *ipaddr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_IP_Addr
 *         ��������IP��ַ
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[in]  addr   ��Ҫ���õ�IP��ַ
 *
 * @return
 *          0 -> ����IP��ַ�ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> SIOCSIFADDRʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_IP_Addr(OS_S8 *ifname, in_addr_t addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_MAC_Addr
 *         ��ȡ����MAC��ַ
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[out] mac    ���MAC��ַ���ڴ�ָ�룬��Ҫ�����߷����ڴ棬����ΪNULL
 *
 * @return
 *          0 -> ��ȡMAC��ַ�ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> SIOCGIFHWADDRʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_MAC_Addr(OS_S8 *ifname, OS_U8 *mac);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_MAC_Addr
 *         �޸�MAC��ַ, ����MAC���ܻᵼ��Ĭ����������
 *
 * @param[in]  ifname �����������, ����"eth0", ����ΪNULL
 * @param[in]  mac    ��Ҫ�޸ĵ�MAC��ַָ��
 *
 * @return
 *          0 -> ����MAC��ַ�ɹ�
 *          1 -> �Ƿ�����
 *          2 -> ��ȡ�����豸״̬ʧ��
 *          3 -> �����豸�ر�ʧ��
 *          4 -> socket��ʧ��
 *          5 -> SIOCGIFHWADDRʧ��
 *          6 -> SIOCSIFHWADDRʧ��
 *          7 -> ���������豸״̬ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_MAC_Addr(OS_S8 *ifname, OS_U8 *mac);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Subnet_Mask
 *         ��ȡ��������
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[out] netmask �����������ָ�룬��Ҫ�����߷���ռ䣬����ΪNULL
 *
 * @return
 *          0 -> ��ȡ��������ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> SIOCGIFNETMASKʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Subnet_Mask(OS_S8 *ifname, in_addr_t *netmask);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Subnet_Mask
 *         ������������
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[in]  addr   ��Ҫ���õ���������
 * @return
 *          0 -> ������������ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> SIOCSIFNETMASKʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Subnet_Mask(OS_S8 *ifname, in_addr_t addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Gateway
 *         ��ȡ����
 *
 * @return
 *          0 -> ��ȡIP��ַ�ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket��ʧ��
 *          3 -> ��������ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Gateway(in_addr_t *gateway);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Add_Gateway
 *         �������, ����Ч��ͬ���ε��豸ȫ��down֮ǰ
 *
 * @param[in]  addr ��Ҫ���ӵ�����
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> �����豸���ɴ�
 *          2 -> socket��ʧ��
 *          3 -> SIOCADDRTʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Add_Gateway(in_addr_t addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Del_Gateway
 *         ɾ������
 *
 * @param[in]  addr gateway ��Ҫɾ��������
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> socket��ʧ��
 *          2 -> SIOCDELRTʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Del_Gateway(in_addr_t addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Gateway
 *         ��������
 *
 * @param[in]  addr ��Ҫ���õ�����
 *
 * @return
 *          0      -> �ɹ�
 *          ����ֵ -> ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Gateway(in_addr_t addr);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Link_Speed
 *         ��ȡ��ǰ���������ٶ�
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[out] speed  ���ص�ǰ���������ٶ�
 *
 * @return
 *          0      -> �ɹ�
 *          1      -> �Ƿ�����
 *          2      -> socket����ʧ��
 *          3      -> ETHTOOL_GSETʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Link_Speed(OS_S8 *ifname, OS_U32 *speed);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Get_Link_Status
 *         ��ȡ����ӿ��Ƿ�����״̬
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[out] up_down ���ص�ǰ����״̬  1->Link up : 0->Link down
 *
 * @return
 *          0      -> �ɹ�
 *          ����ֵ -> ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Get_Link_Status(OS_S8 *ifname, OS_U32 *up_down);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Set_Link_Status
 *         ��������ӿ�����״̬
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[in]  status ���õ�ǰ����״̬  1->Link up : 0->Link down
 *
 * @return
 *          0      -> �ɹ�
 *          1      -> �Ƿ�����
 *          2      -> socket����ʧ��
 *          3      -> δ�ҵ��������
 *          4      -> SIOCSIFFLAGSʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Set_Link_Status(OS_S8 *ifname, OS_S16 status);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Force_Link_Speed
 *         �������������ٶ�
 *
 * @param[in]  ifname ����������ƣ�eth0��
 * @param[in]  speed  ��Ҫ���õ������ٶ� 0(��Э��)/10/100/1000
 * @return
 *          0 -> �ɹ�
 *          1 -> �������Ϸ�
 *          2 -> socket��ʧ��
 *          3 -> ETHTOOL_GSETʧ��
 *          4 -> ETHTOOL_SSETʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Force_Link_Speed(OS_S8 *ifname, OS_U32 speed);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_Ping
 *         ping����
 *
 * @param[in]  ips      IP��ַָ��
 * @param[in]  timeout  ��ʱʱ�䣨��λ�����룩
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> �Ƿ�����
 *          2 -> socket����ʧ��
 *          3 -> setsockoptʧ��
 *          4 -> ����ʧ��
 *          5 -> select��ʱ
 *          6 -> Ŀ���������ɴ�
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Ping(OS_S8 *ips, OS_U32 timeout);

/* --------------------------------------------------------------------------*/
/**
 * @brief OSAL_NET_Generate_Mac_Address
 *          ���������buffer���� ����6�ֽڵ�MAC��ַ
 * @param[in/out] mac       Ҫ���ɵ�mac��ַָ��
 * @param[in]     mac0      ��һλmac��ֵַ, �ǳ���
 * @param[in]     buffer    ���������ָ��
 * @param[in]     len       ��������鳤��
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> �Ƿ�����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_Generate_Mac_Address(OS_U8 *mac, OS_U8 mac0, OS_U8 *buffer, OS_U32 len);

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_NET_API_Test
 *         ����API���Ժ���
 *
 * @param[in]  argc
 * @param[in]  argv
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ip��ַ����
 *          2 -> �����������
 *          3 -> ���ص�ַ����
 *          4 -> �ָ�����������Ϣʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_NET_API_Test(OS_S32 argc, OS_S8 **argv);

#ifdef __cplusplus
}
#endif

#endif //__OSAL_NET_H__
