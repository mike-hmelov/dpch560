#ifndef CH560MONITOR_CONFIG_H
#define CH560MONITOR_CONFIG_H

#include "runtime_params.h"

struct config {
    int delay;
    char cpu_sensor_name[100];
    char gpu_sensor_name[100];
};

int read_conf_file(int reload, struct runtime_params* runtime, struct config* target);
int test_conf_file(char *_conf_file_name);
void clean_config(struct config *config);

#endif //CH560MONITOR_CONFIG_H
