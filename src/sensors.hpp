#pragma once

int init_sensors(Configuration& config, DaemonRuntime& params);

void cleanup_sensors(DaemonRuntime& params);

int cpu_temp(DaemonRuntime& params);

int gpu_temp(DaemonRuntime& params);