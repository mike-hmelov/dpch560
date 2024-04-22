#ifndef CH560MONITOR_RUNTIME_PARAMS_H
#define CH560MONITOR_RUNTIME_PARAMS_H

#include <stdio.h>
#include <hidapi/hidapi.h>
#include <sensors/sensors.h>
#include <malloc.h>

struct runtime_params {
    char *conf_file_name;
    char *log_file_name;
    char *pid_file_name;
    int pid_fd;
    char *app_name;
    FILE *log_stream;
    int start_daemonized;
    hid_device* hid_handle;

    sensors_chip_name cpu_root_chip;
    const sensors_chip_name* cpu_chip;
    const sensors_feature *cpu_feature;
    const sensors_subfeature *cpu_sub;

    sensors_chip_name gpu_root_chip;
    const sensors_chip_name* gpu_chip;
    const sensors_feature *gpu_feature;
    const sensors_subfeature *gpu_sub;
};

void clean_runtime(struct runtime_params* params);

#endif //CH560MONITOR_RUNTIME_PARAMS_H
