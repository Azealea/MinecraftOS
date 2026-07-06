#pragma once
#include <stdio.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4

#ifndef LOG_LEVEL
#    define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#define _LOG_COL_TRACE "\033[90m"
#define _LOG_COL_DEBUG "\033[36m"
#define _LOG_COL_INFO "\033[32m"
#define _LOG_COL_WARN "\033[33m"
#define _LOG_COL_ERROR "\033[31m"
#define _LOG_COL_RESET "\033[0m"

#define _LOG(color, label, fmt, ...)                                                     \
    fprintf(stderr, color "[" label "] %s:%d %s: " fmt _LOG_COL_RESET "\n",              \
            __FILE_NAME__, __LINE__, __func__, ##__VA_ARGS__)

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#    define TRACE(fmt, ...) _LOG(_LOG_COL_TRACE, "TRACE", fmt, ##__VA_ARGS__)
#else
#    define TRACE(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#    define DEBUG(fmt, ...) _LOG(_LOG_COL_DEBUG, "DEBUG", fmt, ##__VA_ARGS__)
#else
#    define DEBUG(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#    define INFO(fmt, ...) _LOG(_LOG_COL_INFO, "INFO", fmt, ##__VA_ARGS__)
#else
#    define INFO(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#    define WARN(fmt, ...) _LOG(_LOG_COL_WARN, "WARN", fmt, ##__VA_ARGS__)
#else
#    define WARN(fmt, ...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#    define ERROR(fmt, ...) _LOG(_LOG_COL_ERROR, "ERROR", fmt, ##__VA_ARGS__)
#else
#    define ERROR(fmt, ...) ((void)0)
#endif
