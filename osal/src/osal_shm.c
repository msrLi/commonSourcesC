/**
 * @file  osal_shm.c
 * @brief  �����ڴ�ģ�����API
 * @author binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2017-03-31
 */
/* Copyright (C) 2008-2016, Null Inc.
 * All right reserved
 *
 */


#include <os_common.h>
#include <osal.h>

/* �����ڴ�ģ��İ汾��, �������޸�ʱ�����޸� */
#define OSAL_SHM_MODULE_VERSION  ("1.2.0")

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Module_Version
 *         ��ȡSHMģ��ĵ�ǰ�汾
 *
 * @param[in/out]   major_version  ģ�����汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 * @param[in/out]   minor_version  ģ���Ӱ汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 * @param[in/out]   build_version  ģ�����汾�ŷ���ָ�룬��Ҫ�ⲿ������Чָ��
 *
 * @return
 *          0 -> �汾�Ż�ȡ�ɹ�
 *          1 -> ������������ָ��Ƿ�
 *          2 -> �汾�Ÿ�ʽ����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* �汾��ӡ�����ڵ��� */
    LOGI(MODULE_SHM, "%s: %s\n", __func__, OSAL_SHM_MODULE_VERSION);

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(OSAL_SHM_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_SHM, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_SHM | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Init
 *         �����ڴ��ʼ��
 *
 * @param[in]       key     �����ڴ�Key
 * @param[in]       size    �����ڴ��С,
 *                          ����0���������½��Ĺ����ڴ��С�����ֽ�Ϊ��λ,
 *                          0:ֻ��ȡ�����ڴ�ʱָ��Ϊ0
 *                          ������������0
 *
 * @param[in]       flag    �����ڴ��־
 *                          OSAL_SHM_FLAG_CREAT: �����ڴ治���ڴ��������ڷ��ض�Ӧ��id,
 *                          OSAL_SHM_FLAG_CREAT_ERR: �����ڴ��ʶ�������������򷵻ش���,
 *                          OSAL_SHM_FLAG_CREAT_EXCL: �����ڴ治���ڴ����������򷵻ش���
 *                          ������������0
 * @param[in/out]   mid     �����ڴ�idָ�룬��Ҫ�ⲿ������Чָ�룬����ΪNULL
 *
 * @return
 *          0 -> ��ʼ���ɹ�
 *          1 -> ��������
 *          2 -> ��ʼ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Init(OS_S32 key, OS_U32 size, OS_U32 flag, OS_S32 *mid)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!mid), 1, return_err,
                     "Invalid argument! (0x%x, %d, %d, %p)\n", key, size, flag, mid);

    switch (flag) {
        case OSAL_SHM_FLAG_CREAT:
            ret = shmget(key, 0, 0);
            if (ret < 0) {
                ret = shmget(key, size, IPC_CREAT | 0660);
            }
            break;
        case OSAL_SHM_FLAG_CREAT_ERR:
            ret = shmget(key, 0, 0);
            break;
        case OSAL_SHM_FLAG_CREAT_EXCL:
            ret = shmget(key, size, IPC_CREAT | IPC_EXCL | 0660);
            break;
        default:
            OSAL_CHECK_ERROR(MODULE_SHM, 1, 1, return_err, "Invalid argument! flag = %d\n", flag);
            break;

    }
    /* ʧ��ʱ����-1��errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret == -1), 2, return_err, "Call shmget fail with %s!\n", strerror(errno));

    /* ������Чmidֵ */
    *mid = ret;

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Get_Addr
 *         �ѹ����ڴ�������ӳ�䵽���ý��̵ĵ�ַ�ռ�
 *
 * @param[in]       mid     �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in/out]   addr    �����ڴ��ַָ�룬��Ҫ�ⲿ������Чָ�룬����ΪNULL
 *
 * @return
 *          0 -> ��ȡ�����ڴ��ַ�ɹ�
 *          1 -> ��������
 *          2 -> ��ȡ�����ڴ��ַʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Get_Addr(OS_S32 mid, OS_HANDLE *addr)
{
    OS_HANDLE shm_addr = NULL;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!addr), 1, return_err,
                     "Invalid argument! (0x%x, %p)\n", mid, addr);

    shm_addr = shmat(mid, 0, 0);
    /* �ɹ�ʱ������ȷ�Ĺ����ڴ��ַ�� ʧ��ʱ���� (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    /* �����Ч��ַ */
    *addr = shm_addr;

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Detach_Addr
 *         ȡ�������ڴ��ַӳ��
 *
 * @param[in]  addr     �����ڴ��ַ
 *
 * @return
 *          0 -> ȡ�������ڴ��ַӳ��ɹ�
 *          1 -> ��������
 *          2 -> ȡ�������ڴ��ַӳ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Detach_Addr(OS_HANDLE addr)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!addr), 1, return_err, "Invalid argument! (%p)\n", addr);

    ret = shmdt(addr);
    /* �ɹ�ʱ����0�� ʧ��ʱ����-1��errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 2, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Read
 *         �����ڴ��ȡ����
 *
 * @param[in]       mid     �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in]       offset  �����ڴ� offset
 * @param[in/out]   buf     ��Ź������ݵ�ָ�룬��Ҫ�ⲿ���룬����ΪNULL
 * @param[in]       len     ��Ҫ��ȡ�����ݳ��ȣ�(offset+len)���ܳ��������ڴ��С
 *
 * @return
 *          0 -> ��ȡ�����ڴ����ݳɹ�
 *          1 -> ��������
 *          2 -> ӳ�乲���ڴ浽��ǰ���̴���
 *          3 -> ȡ�������ڴ�ӳ�����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Read(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len)
{
    OS_S8 *shm_addr = NULL;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!buf), 1, return_err,
                     "Invalid argument! (%d, %d, %p, %d)\n", mid, offset, buf, len);

    shm_addr = (OS_S8*) shmat(mid, 0, 0);
    /* �ɹ�ʱ������ȷ�Ĺ����ڴ��ַ�� ʧ��ʱ���� (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    memcpy(buf, shm_addr + offset, len);

    ret = shmdt(shm_addr);
    /* �ɹ�ʱ����0�� ʧ��ʱ����-1��errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 3, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_SHM_Write
 *         �����ڴ�д�����
 *
 * @param[in]   mid         �����ڴ�id, �ο�OSAL_SHM_Init
 * @param[in]   offset      �����ڴ� offset
 * @param[in]   buf         ��Ҫд��������ڴ�ָ��
 * @param[in]   len         ��Ҫд������ݳ���
 *
 * @return
 *          0 -> �����ڴ�д��ɹ�
 *          1 -> ��������
 *          2 -> ӳ�乲���ڴ浽��ǰ���̴���
 *          3 -> ȡ�������ڴ�ӳ�����
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_SHM_Write(OS_S32 mid, OS_S32 offset, OS_HANDLE buf, OS_U32 len)
{
    OS_S8 *shm_addr = NULL;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_SHM, (!buf), 1, return_err,
                     "Invalid argument! (%d, %d, %p, %d)\n", mid, offset, buf, len);

    shm_addr = (OS_S8*) shmat(mid, 0, 0);
    /* �ɹ�ʱ������ȷ�Ĺ����ڴ��ַ�� ʧ��ʱ���� (void *) -1 */
    OSAL_CHECK_ERROR(MODULE_SHM, (shm_addr == ((OS_HANDLE) - 1)), 2, return_err, "Call shmat fail with %s!\n",
                     strerror(errno));

    /* д������ */
    memcpy(shm_addr + offset, buf, len);

    ret = shmdt(shm_addr);
    /* �ɹ�ʱ����0�� ʧ��ʱ����-1��errno */
    OSAL_CHECK_ERROR(MODULE_SHM, (ret), 3, return_err, "Call shmat fail with %s!\n", strerror(errno));

    return 0;

return_err:
    return (MODULE_SHM | ret);
}

