#include "common.hpp"
#include "runtime_params.hpp"

DaemonRuntime::DaemonRuntime() {
    pid_fd = -1;
}

DaemonRuntime::~DaemonRuntime() {
    free(app_name);
    if (conf_file_name != nullptr)
        free(conf_file_name);
    if (log_file_name != nullptr)
        free(log_file_name);
    if (pid_file_name != nullptr)
        free(pid_file_name);
    if (log_stream != stdout) {
        fclose(log_stream);
    }
}
