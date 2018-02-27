#ifndef __DEBUG_LOG_H_
#define __DEBUG_LOG_H_

#include "cpu.h"

/**
 * @brief Hexdump via log_info
 * @param data
 * @param size
 */
void log_info_hexdump(const void *data, int size);

void printf_buf(u8 *buf, u32 len);

#define PRINTF(format, ...)         printf(format, ## __VA_ARGS__)

#ifdef LOG_INFO_ENABLE
#define log_info(format, ...)       PRINTF("[info] :" LOG_TAG format "\n", ## __VA_ARGS__)
#else
#define log_info(...)
#endif

#ifdef LOG_ERROR_ENABLE
#define log_error(format, ...)      PRINTF("<error> :" LOG_TAG format "\n", ## __VA_ARGS__)
#define log_error_hexdump(x, y)     printf_buf(x, y)
#else
#define log_error(...)
#define log_error_hexdump(...)
#endif

#ifdef LOG_DUMP_ENABLE
#define log_info_hexdump(x,y)       printf_buf(x,y)
#else
#define log_info_hexdump(...)
#endif

#endif//__DEBUG_LOG_H_
