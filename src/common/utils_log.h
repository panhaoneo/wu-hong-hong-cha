#pragma once

#include <stdio.h>
// 暂时用print代替日志，打印到前台，不支持多线程打印

#define log_debug(err_code, fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__); fprintf(stdout, "\n")
#define log_info(err_code, fmt, ...)  fprintf(stdout, fmt, ##__VA_ARGS__); fprintf(stdout, "\n")
#define log_warn(err_code, fmt, ...)  fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n")
#define log_err(err_code, fmt, ...)   fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr, "\n")

//////////////////// error code

enum QDSErrCode {
    kEok = 0,
    kEInit, // 初始化错误
    kEConfig,
    kEParam, // 参数错误
};
