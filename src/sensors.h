#ifndef CH560MONITOR_SENSORS_H
#define CH560MONITOR_SENSORS_H

#include <sensors/sensors.h>
#include <sensors/error.h>

#include "config.h"

int init_sensors(struct config* config, struct runtime_params* params);
void cleanup_sensors(struct runtime_params *params);

int cpu_temp(struct runtime_params *params);
int gpu_temp(struct runtime_params *params);

#endif //CH560MONITOR_SENSORS_H