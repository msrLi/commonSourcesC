/**
 * @file osal_file.c
 * @brief 基于C库二次封装的带参数检查的文件操作API
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

/* 配置文件行长度 */
#define OSAL_FILE_LINE_SIZE      (128)

/* FILE 模块的版本号, 代码有修改时必须修改 */
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
 *         获取FILE模块的当前版本
 *
 * @param[in/out]   major_version  模块主版本号返回指针，需要外部输入有效指针
 * @param[in/out]   minor_version  模块子版本号返回指针，需要外部输入有效指针
 * @param[in/out]   build_version  模块编译版本号返回指针，需要外部输入有效指针
 *
 * @return
 *          0 -> 版本号获取成功
 *          1 -> 参数错误，输入指针非法
 *          2 -> 版本号格式错误
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Get_Module_Version(OS_U32 *major_version, OS_U32 *minor_version, OS_U32 *build_version)
{
    OS_S32 ret = 0;

    /* 版本打印，便于调试 */
    LOGI(MODULE_FILE, "%s: %s\n", __func__, OSAL_FILE_MODULE_VERSION);

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (!major_version) || (!minor_version) || (!build_version), 1, return_err,
                     "Invalid argument! (%p, %p, %p)\n", major_version, minor_version, build_version);

    /* 版本返回，便于上层应用通过版本号判断，实现差异化兼容处理 */
    ret = sscanf(OSAL_FILE_MODULE_VERSION, "%d.%d.%d", major_version, minor_version, build_version);
    OSAL_CHECK_ERROR(MODULE_FILE, (3 != ret), 2, return_err, "Version format invalid! (Error: %d)\n", ret);

    return 0;

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_FILE | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Check_Exist
 *         检查文件是否存在
 *
 * @param[in]  filepath  文件绝对路径
 *
 * @return
 *          0 -> 文件存在
 *          1 -> 参数错误，文件路径为NULL
 *          2 -> 文件不存在
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Check_Exist(OS_S8 *filepath)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (!filepath), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* 文件存在检查 */
    ret = access(filepath, 0);
    /* 结果检查 */
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
 *         删除文件函数
 *
 * @param[in]  filepath  文件绝对路径
 *
 * @return
 *          0 -> 文件删除成功
 *          1 -> 参数无效, 无效文件路径NULL
 *          2 -> 文件不存在
 *          3 -> 文件删除失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Delete(OS_S8 *filepath)
{
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (!filepath), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* 文件存在检查 */
    ret = OSAL_FILE_Check_Exist(filepath);
    OSAL_CHECK_ERROR(MODULE_FILE, ret, 2, return_err, "File %s is not exist! (Error: 0x%08x)\n", filepath, ret);
    /* 删除文件 */
    ret = unlink(filepath);
    /* 结果检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 3, return_err, "Failed to delete file %s! (Error: %d)\n", filepath, ret);

    return 0;

return_err:
    return (MODULE_FILE | ret);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Get_Size
 *         获取指定文件字节数
 *
 * @param[in]     filepath  文件绝对路径
 * @param[in/out] size      文件字节数返回指针，需要外部输入有效指针
 *
 * @return
 *          0 -> 文件大小获取成功
 *          1 -> 参数无效
 *          2 -> 文件不存在
 *          3 -> 获取文件信息失败
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

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, ((!filepath) || (!size)), 1, return_err, "Invalid argument! (%p)\n", filepath);

    /* 文件存在检查 */
    ret = OSAL_FILE_Check_Exist(filepath);
    OSAL_CHECK_ERROR(MODULE_FILE, ret, 2, return_err, "File %s is not exist! (Error: 0x%08x)\n", filepath, ret);
    /* 获取文件信息 */
    ret = stat(filepath, &file_info);
    /* 结果检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 3, return_err, "Failed to get file information! (Error: %d)\n", ret);

    /* 测试资料如下 */
    if (file_info.st_size == 0) {
        /* 检查文件路径是否是proc或者sys目录下特殊文件, 如果是特殊处理 */
        if ((strncmp(filepath, "/proc/", 6) == 0) || (strncmp(filepath, "/sys/", 5) == 0)) {
            /* 分配命令内存 */
            cmd_len = strlen(filepath) * 2;
            cmd_buf = (OS_S8*) malloc(cmd_len);
            OSAL_CHECK_ERROR(MODULE_FILE, (!cmd_buf), 4, return_err, "Malloc %d bytes fail\n", cmd_len);
            /* 生成目标文件名 */
            gettimeofday(&timestamp, NULL);
            memset(target_filename, 0, 64);
            sprintf(target_filename, "/tmp/filesize%ld%ld", timestamp.tv_sec, timestamp.tv_usec);
            memset(cmd_buf, 0, cmd_len);
            sprintf(cmd_buf, "cp -af %s %s", filepath, target_filename);
            ret = system((const OS_S8*) cmd_buf);
            /* 获取文件信息 */
            ret = stat(target_filename, &file_info);
            /* 结果检查 */
            OSAL_CHECK_ERROR(MODULE_FILE, (-1 == ret), 5, return_err, "Failed to get file information! (Error: %d)\n", ret);
            /* 删除文件 */
            memset(cmd_buf, 0, cmd_len);
            sprintf(cmd_buf, "rm -rf %s", target_filename);
            ret = system((const OS_S8*) cmd_buf);
            free(cmd_buf);
        }
    }

    *size = file_info.st_size;

    return 0;

return_err:
    /* 如果内存未释放，释放内存 */
    if (cmd_buf) {
        free(cmd_buf);
    }
    return (MODULE_FILE | ret);
}
#if 0
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_Read_Arguments
 *         读取参数
 *
 * @param[in]       path         配置文件绝对路径
 * @param[in]       function     参数所属功能关键字
 * @param[in]       key          参数关键字
 * @param[in/out]   value        有效指针
 *
 * @return
 *          0 -> 成功
 *          1 -> 参数错误
 *          2 -> 路径无效
 *          3 -> 配置文件打开失败
 *          4 -> 文件中未找到相关类别
 *          5 -> 文件中未找到相关参数
 *          6 -> 类别内未找到相关参数
 *          7 -> 类别未找到
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_Read_Arguments(OS_S8 *path, OS_S8 *function, OS_S8 *key, OS_S8 *value)
{
    FILE *fp = NULL;
    OS_S8 tmpbuf[OSAL_FILE_LINE_SIZE] = {0};
    OS_U32 i = 0;
    OS_S32 ret = 0;

    /* 参数检查 */
    OSAL_CHECK_ERROR(MODULE_FILE, (((!path) || (!function)) || ((key) && (!value))), 1, return_err,
                     "Invalid argument! (%p, %p, %p, %p)\n", path, function, key, value);

    ret = strcmp(path, "");
    OSAL_CHECK_ERROR(MODULE_FILE, (ret == 0), 2, return_err, "File path is invalid! (Error: %d)\n", ret);

    fp = fopen(path, "rt");
    OSAL_CHECK_ERROR(MODULE_FILE, (!fp), 3, return_err, "Failed to open %s! (Error: %p)\n", path, fp);

    while (!feof(fp)) {
        if (fgets(tmpbuf, OSAL_FILE_LINE_SIZE, fp) <= 0) {
            /* 文件中未找到相关类别 */
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
                    /* 文件中未找到相关参数 */
                    ret = 5;
                    goto return_err;
                }
                if (tmpbuf[0] == '[') {
                    for (i = 0; i < OSAL_FILE_LINE_SIZE; i++) {
                        if (tmpbuf[i] == ']') {
                            /* 类别内未找到相关参数 */
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
    /* 类别未找到 */
    ret = 7;

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_FILE | ret);
}
#endif
/* --------------------------------------------------------------------------*/
/**
 * @brief  OSAL_FILE_API_Test
 *         FILE相关API测试，已覆盖所有 FILE API函数
 *
 * @return
 *          0 -> 测试通过
 *          1 -> 参数个数为0
 *          2 -> 文件不存在
 *          3 -> 文件删除失败
 *          4 -> 获取文件大小失败
 *          5 -> 解析ini失败
 */
/* --------------------------------------------------------------------------*/
OS_S32 OSAL_FILE_API_Test(OS_S32 argc, OS_S8 *argv[])
{
    /* 用于存储版本号 */
    OS_U32 major_version = 0;
    OS_U32 minor_version = 0;
    OS_U32 build_version = 0;

    /* 临时变量 */
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
    /* 参数解析 */
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

    /* 打印 FILE 版本号 */
    ret = OSAL_FILE_Get_Module_Version(&major_version, &minor_version, &build_version);

    /* 帮助打印 */
    if (argc == 1) {
        usage_print(argv[0]);
        return MODULE_FILE | 1;
    }

    /* 参数解析 */
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

    /* 函数执行异常值返回 */
return_err:
    return (MODULE_FILE | ret);
}

