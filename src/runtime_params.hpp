#pragma once

class DaemonRuntime {
public:
    DaemonRuntime();
    ~DaemonRuntime();

public:
    char *conf_file_name;
    char *log_file_name;
    char *pid_file_name;
    int pid_fd;
    char *app_name;
    FILE *log_stream;
    int start_daemonized;

    sensors_chip_name cpu_root_chip;
    const sensors_chip_name* cpu_chip;
    const sensors_feature *cpu_feature;
    const sensors_subfeature *cpu_sub;

    sensors_chip_name gpu_root_chip;
    const sensors_chip_name* gpu_chip;
    const sensors_feature *gpu_feature;
    const sensors_subfeature *gpu_sub;
};

