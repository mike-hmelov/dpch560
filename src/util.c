#include "util.h"

void setup_log_file(struct runtime_params *params) {
    /* Try to open log file to this daemon */
    if (params->log_file_name != NULL) {
        params->log_stream = fopen(params->log_file_name, "a+");
        if (params->log_stream == NULL) {
            syslog(LOG_ERR, "Can not open log file: %s, error: %s",
                   params->log_file_name, strerror(errno));
            params->log_stream = stdout;
        }
    } else {
        params->log_stream = stdout;
    }
}

void temp_to_buf(u_int8_t *buf, int temp) {
    int current = temp;
    *(buf + 2) = current % 10;
    current /= 10;
    *(buf + 1) = current % 10;
    current /= 10;
    *(buf) = current % 10;
}

