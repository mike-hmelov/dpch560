#pragma once

int init_sensors(Configuration& config, runtime_params *params);

void cleanup_sensors(runtime_params *params);

int cpu_temp(runtime_params *params);

int gpu_temp(runtime_params *params);