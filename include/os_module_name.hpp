/* 不要添加到其他头文件中 */
#ifdef __MODULE_NAME_H__

#include "os_common.h"

#define MODULE_DEFAULT   "[NONAME]"

MODULE_SHORT_NAME_T g_module_names[] = {
    {MODULE_LOG, "[LOGS]"},
    {MODULE_RTC, "[LOGS]"},
    {MODULE_I2C, "[LOGS]"},
    {MODULE_SPI, "[NULL]"},
    {MODULE_UART, "[NULL]"},
    {MODULE_FPGA, "[NULL]"},
    {MODULE_EEPROM, "[NULL]"},
    {MODULE_VIN,   "[NULL]"},
    {MODULE_GPIO,  "[NULL]"},
    {MODULE_TEMPERATURE, "[NULL]"},
    {MODULE_LOCAL_BUS, "[NULL]"},
    {MODULE_JPEG_SW,   "[NULL]"},
    {MODULE_MESSAGE,   "[NULL]"},
    {MODULE_SHM,       "[NULL]"},
    {MODULE_MSG,       "[NULL]"},
    {MODULE_SEM,       "[NULL]"},
    {MODULE_LOCK,      "[NULL]"},
    {MODULE_TASK,      "[NULL]"},
    {MODULE_SWOSD,     "[NULL]"},
    {MODULE_ISP,       "[NULL]"},
    {MODULE_ENCODE,    "[NULL]"},
    {MODULE_WATCHDOG,  "[NULL]"},
    {MODULE_SEMAPHORE, "[NULL]"},
    {MODULE_SHARE_BUFFER, "[NULL]"},
    {MODULE_SECRET,      "[NULL]"},
    {MODULE_PID,         "[NULL]"},
    {MODULE_SYSINFO,     "[NULL]"},
};

const OS_S8* OS_Get_Module_Name(OS_U32 module)
{
    for (OS_U32 i = 0; i < (OS_U32) (sizeof(g_module_names) / sizeof(MODULE_SHORT_NAME_T)); i++) {
        if (EQUALPTR(g_module_names[i].module, module)) {
            return g_module_names[i].name;
        }
    }

    return MODULE_DEFAULT;
}

#endif // __MODULE_NAME_H__

