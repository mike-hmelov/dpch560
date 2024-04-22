#ifndef CH560MONITOR_UTIL_H
#define CH560MONITOR_UTIL_H

#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#include "runtime_params.h"

#define MAGIC_HEADER 16
#define CELSIUS 19

void print_help(const char *name);
void temp_to_buf(u_int8_t *buf, int temp);
void setup_log_file(struct runtime_params *params);

#endif //CH560MONITOR_UTIL_H