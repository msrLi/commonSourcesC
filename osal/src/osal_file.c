/**
 * @file osal_file.c
 * @brief ����C����η�װ�Ĵ����������ļ�����API
 * @author  binsonli <binsonli666@icloud.com>
 * @version 1.0.0
 * @date 2018-03-28
 */
/* Copyright (C) 2008-2018, Null Inc.
 * All right reserved
 *
 */

#include <os_common.h>
#include <osal_log.h>
#include <osal_file.h>

/* �����ļ��г��� */
#define OSAL_FILE_LINE_SIZE      (128)

/* FILE ģ��İ汾��, �������޸�ʱ�����޸� */
#define OSAL_FILE_MODULE_VERSION  ("1.2.0")
#if 0
typedef struct OSAL_FILE_INI_INFO_TAG {
    OS_S8 func[16];
    OS_S8 key[16];
    OS_S8 val[128];
} OSAL_FILE_INI_INFO_T;
#endif
static OS_S32 usage_print(OS_S8 *name)
{
    LOGI(MODULE_FILE,
         "\r\n           Example: %s -c filepath\r\n"
         "           Options:\r\n"
         "           -h | --help          Print this message\r\n"
         "           -c | --check-exist   Check file exist\r\n"
         "           -d | --delete        Delete file\r\n"
         "           -g | --get-size      Get file size\r\n"
         "           -p | --parse         Parse ini file\r\n"
         "\n", name);
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Get_Module_Version
 *         ��ȡFILEģ��ĵ�ǰ�汾
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
OS_S32 OSAL_FILE_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* �汾��ӡ�����ڵ��� */
    LOGI(MODULE_FILE, "%s: %s\n", __func__, OSAL_FILE_MODULE_VERSION);

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_FILE, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* �汾���أ������ϲ�Ӧ��ͨ���汾���жϣ�ʵ�ֲ��컯���ݴ��� */
    ret = sscanf(OSAL_FILE_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_FILE, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_FILE | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Check_Exist
 *         ����ļ��Ƿ����
 *
 * @param[in]  filepath  �ļ�����·��
 *
 * @return
 *          0 -> �ļ�����
 *          1 -> ���������ļ�·��ΪNULL
 *          2 -> �ļ�������
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Check_Exist(OS_S8 *filepath)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_FILE, (!filepath), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* �ļ����ڼ�� */
    ret = access(filepath, 0);
    /* ������ */
    if (ret) {
        ret = 2;
        goto return_err;
    }

    return 0;

return_err:
    return (MODULE_FILE | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Delete
 *         ɾ���ļ�����
 *
 * @param[in]  filepath  �ļ�����·��
 *
 * @return
 *          0 -> �ļ�ɾ���ɹ�
 *          1 -> ������Ч, ��Ч�ļ�·��NULL
 *          2 -> �ļ�������
 *          3 -> �ļ�ɾ��ʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Delete(OS_S8 *filepath)
{
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_FILE, (!filepath), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* �ļ����ڼ�� */
    ret = OSAL_FILE_Check_Exist(filepath);
    OSAL_CHECK_ERROR(MODULE_FILE, ret, 2, return_err, "File %s is not exist! (Error: 0x%08x)\n", filepath, ret);
    /* ɾ���ļ� */
    ret = unlink(filepath);
    /* ������ */
    OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 3, return_err, "Failed to delete file %s! (Error: %d)\n", filepath, ret);

    return 0;

return_err:
    return (MODULE_FILE | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Get_Size
 *         ��ȡָ���ļ��ֽ���
 *
 * @param[in]     filepath  �ļ�����·��
 * @param[in/out] size      �ļ��ֽ�������ָ�룬��Ҫ�ⲿ������Чָ��
 *
 * @return
 *          0 -> �ļ���С��ȡ�ɹ�
 *          1 -> ������Ч
 *          2 -> �ļ�������
 *          3 -> ��ȡ�ļ���Ϣʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Get_Size(OS_S8 *filepath, OS_ULONG *size)
{
    struct stat file_info;
    struct timeval timestamp;
    OS_S8 target_filename[64];
    OS_S8 *cmd_buf = NULL;
    OS_U32 cmd_len = 0;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_FILE, ((!filepath) || (!size)), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* �ļ����ڼ�� */
    ret = OSAL_FILE_Check_Exist(filepath);
    OSAL_CHECK_ERROR(MODULE_FILE, ret, 2, return_err, "File %s is not exist! (Error: 0x%08x)\n", filepath, ret);
    /* ��ȡ�ļ���Ϣ */
    ret = stat(filepath, &file_info);
    /* ������ */
    OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 3, return_err, "Failed to get file information! (Error: %d)\n", ret);

    /* ������������ */
    if (file_info.st_size == 0) {
        /* ����ļ�·���Ƿ���proc����sysĿ¼�������ļ�, ��������⴦�� */
        if ((strncmp(filepath, "/proc/", 6) == 0) || (strncmp(filepath, "/sys/", 5) == 0)) {
            /* ���������ڴ� */
            cmd_len = strlen(filepath) * 2;
            cmd_buf = (OS_S8*) malloc(cmd_len);
            OSAL_CHECK_ERROR(MODULE_FILE, (!cmd_buf), 4, return_err, "Malloc %d bytes fail\n", cmd_len);
            /* ����Ŀ���ļ��� */
            gettimeofday(&timestamp, NULL);
            memset(target_filename, 0, 64);
            sprintf(target_filename, "/tmp/filesize%ld%ld", timestamp.tv_sec, timestamp.tv_usec);
            memset(cmd_buf, 0, cmd_len);
            sprintf(cmd_buf, "cp -af %s %s", filepath, target_filename);
            ret = system((const OS_S8*) cmd_buf);
            /* ��ȡ�ļ���Ϣ */
            ret = stat(target_filename, &file_info);
            /* ������ */
            OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 5, return_err, "Failed to get file information! (Error: %d)\n", ret);
            /* ɾ���ļ� */
            memset(cmd_buf, 0, cmd_len);
            sprintf(cmd_buf, "rm -rf %s", target_filename);
            ret = system((const OS_S8*) cmd_buf);
            free(cmd_buf);
        }
    }

    *size = file_info.st_size;

    return 0;

return_err:
    /* ����ڴ�δ�ͷţ��ͷ��ڴ� */
    if (cmd_buf) {
        free(cmd_buf);
    }
    return (MODULE_FILE | ret);
}
#if 0
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Read_Arguments
 *         ��ȡ����
 *
 * @param[in]       path         �����ļ�����·��
 * @param[in]       function     �����������ܹؼ���
 * @param[in]       key          �����ؼ���
 * @param[in/out]   value        ��Чָ��
 *
 * @return
 *          0 -> �ɹ�
 *          1 -> ��������
 *          2 -> ·����Ч
 *          3 -> �����ļ���ʧ��
 *          4 -> �ļ���δ�ҵ�������
 *          5 -> �ļ���δ�ҵ���ز���
 *          6 -> �����δ�ҵ���ز���
 *          7 -> ���δ�ҵ�
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Read_Arguments(OS_S8 *path, OS_S8 *function, OS_S8 *key, OS_S8 *value)
{
    FILE *fp = NULL;
    OS_S8 tmpbuf[OSAL_FILE_LINE_SIZE] = {0};
    OS_U32 i = 0;
    OS_S32 ret = 0;

    /* ������� */
    OSAL_CHECK_ERROR(MODULE_FILE, (((!path) || (!function)) || ((key) && (!value))), 1, return_err,
                     "Invalid argument! (%p, %p, %p, %p)\n", path, function, key, value);

    ret = strcmp(path, "");
    OSAL_CHECK_ERROR(MODULE_FILE, (ret == 0), 2, return_err, "File path is invalid! (Error: %d)\n", ret);

    fp = fopen(path, "rt");
    OSAL_CHECK_ERROR(MODULE_FILE, (!fp), 3, return_err, "Failed to open %s! (Error: %p)\n", path, fp);

    while (!feof(fp)) {
        if (fgets(tmpbuf, OSAL_FILE_LINE_SIZE, fp) <= 0) {
            /* �ļ���δ�ҵ������� */
            ret = 4;
            goto return_err;
        }
        if (tmpbuf[0] == ' ' || tmpbuf[0] == '#' || tmpbuf[0] == '/') {
            continue;
        }
        if ((strstr(tmpbuf, function) != NULL && tmpbuf[0] == '[') && tmpbuf[strlen(function) + 1] == ']') {
            if (!key) {
                return 0;
            }

            memset(tmpbuf, 0, sizeof(tmpbuf));
            while (!feof(fp)) {
                if (fgets(tmpbuf, OSAL_FILE_LINE_SIZE, fp) <= 0) {
                    /* �ļ���δ�ҵ���ز��� */
                    ret = 5;
                    goto return_err;
                }
                if (tmpbuf[0] == '[') {
                    for (i = 0; i < OSAL_FILE_LINE_SIZE; i++) {
                        if (tmpbuf[i] == ']') {
                            /* �����δ�ҵ���ز��� */
                            ret = 6;
                            goto return_err;
                        }
                    }
                }
                if (strstr(tmpbuf, key) == tmpbuf && (tmpbuf[strlen(key)] == '=') &&
                    (tmpbuf[strlen(key) + 1] != ' ') && (tmpbuf[strlen(key) + 1] != '\n')) {
                    strncpy(value, tmpbuf + strlen(key) + 1, strlen(tmpbuf) - (strlen(key) + 2));
                    fclose(fp);
                    return 0;
                }
            }
        }
    }

    fclose(fp);
    /* ���δ�ҵ� */
    ret = 7;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_FILE | ret);
}
#endif
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_API_Test
 *         FILE���API���ԣ��Ѹ������� FILE API����
 *
 * @return
 *          0 -> ����ͨ��
 *          1 -> ��������Ϊ0
 *          2 -> �ļ�������
 *          3 -> �ļ�ɾ��ʧ��
 *          4 -> ��ȡ�ļ���Сʧ��
 *          5 -> ����iniʧ��
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_API_Test(OS_S32 argc, OS_S8 *argv[])
{
    /* ���ڴ洢�汾�� */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    /* ��ʱ���� */
    OS_S8 *file_path = NULL;
    OS_U32 i = 0;
    OS_U32 ret = 0;
    OS_ULONG file_size = 0;
#if 0
    OSAL_FILE_INI_INFO_T ini_info[4] = {
        {
            .func = {"SERVER\0"},
            .key = {"ip\0"},
            .val = {0},
        },
        {
            .func = {"SERVER\0"},
            .key = {"path\0}"},
            .val = {0},
        },
        {
            .func = {"CLIENT"},
            .key = {"count"},
            .val = {0},
        },
        {
            .func = {"CLIENT"},
            .key = {"path"},
            .val = {0},
        }
    };
#endif
    /* �������� */
    OS_S32 c = 0;
    const struct option long_options [] = {
        { "help",           no_argument,            NULL,           'h' },
        { "check-exist",    required_argument,      NULL,           'c' },
        { "delete",         required_argument,      NULL,           'd' },
        { "get-size",       required_argument,      NULL,           'g' },
        { "parse",          required_argument,      NULL,           'p' },
        {}
    };
    const OS_S8 short_options [] = "c:d:g:p:h";

    /* ��ӡ FILE �汾�� */
    ret = OSAL_FILE_Get_Module_Version(&major_version, &minor_version, &build_version);

    /* ������ӡ */
    if (argc == 1) {
        usage_print(argv[0]);
        return MODULE_FILE | 1;
    }

    /* �������� */
    while (1) {
        c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch (c) {
            case 'c':
                file_path = optarg;
                ret = OSAL_FILE_Check_Exist(file_path);
                OSAL_CHECK_ERROR(MODULE_FILE, ret, 2, return_err, "File %s is not exist! (Error: 0x%08x)\n\n", file_path, ret);
                LOGI(MODULE_FILE, "File %s is exist!\n", file_path);
                break;
            case 'd':
                file_path = optarg;
                ret = OSAL_FILE_Delete(file_path);
                OSAL_CHECK_ERROR(MODULE_FILE, ret, 3, return_err, "Failed to delete file %s! (Error: 0x%08x)\n", file_path, ret);
                LOGI(MODULE_FILE, "Delete file %s success!\n", file_path);
                break;
            case 'g':
                file_path = optarg;
                ret = OSAL_FILE_Get_Size(file_path, &file_size);
                OSAL_CHECK_ERROR(MODULE_FILE, ret, 4, return_err, "Failed to get file size! (Error: 0x%08x)\n", ret);
                LOGI(MODULE_FILE, "File %s size is %lu bytes!\n", file_path, file_size);
                break;
            case 'p':
                file_path = optarg;
#if 0
                for (i = 0; i < 4; i++) {
                    ret |= OSAL_FILE_Read_Arguments(file_path, ini_info[i].func, ini_info[i].key, ini_info[i].val);
                }
                OSAL_CHECK_ERROR(MODULE_FILE, ret, 5, return_err, "Failed to get ini info! (Error: 0x%08x)\n", ret);
                for (i = 0; i < 4; i++) {
                    LOGI(MODULE_FILE, "func=%s key=%s val=%s\n", ini_info[i].func, ini_info[i].key, ini_info[i].val);
                }
#endif
                LOGE(MODULE_FILE, "Not support\n");
                break;
            default:
                usage_print(argv[0]);
                break;
        }
    }

    return 0;

    /* ����ִ���쳣ֵ���� */
return_err:
    return (MODULE_FILE | ret);
}

