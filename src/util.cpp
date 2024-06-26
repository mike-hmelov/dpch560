#include "common.hpp"
#include "runtime_params.hpp"
#include "util.hpp"

void setup_log_file(DaemonRuntime& params) {
    /* Try to open log file to this daemon */
    if (params.log_file_name != nullptr) {
        params.log_stream = fopen(params.log_file_name, "a+");
        if (params.log_stream == nullptr) {
            syslog(LOG_ERR, "Can not open log file: %s, error: %s",
                   params.log_file_name, strerror(errno));
            params.log_stream = stdout;
        }
    } else {
        params.log_stream = stdout;
    }
}

