#pragma once

class DaemonRuntime {
public:
    DaemonRuntime();
    ~DaemonRuntime();

public:
    char *conf_file_name{};
    char *log_file_name{};
    char *pid_file_name{};
    int pid_fd;
    char *app_name{};
    FILE *log_stream{};
    int start_daemonized{};
};

