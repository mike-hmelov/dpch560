#ifndef CH560MONITOR_RUNTIME_PARAMS_H
#define CH560MONITOR_RUNTIME_PARAMS_H

#include <stdio.h>
#include <hidapi/hidapi.h>

struct runtime_params {
    char *conf_file_name;
    char *log_file_name;
    char *pid_file_name;
    int pid_fd;
    char *app_name;
    FILE *log_stream;
    int start_daemonized;
    hid_device* hid_handle;
};

#endif //CH560MONITOR_RUNTIME_PARAMS_H
