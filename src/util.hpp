#pragma once

#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#include "runtime_params.hpp"

#define MAGIC_HEADER 16
#define CELSIUS 19

void print_help(const char *name);

void temp_to_buf(u_int8_t *buf, int temp);

void setup_log_file(DaemonRuntime& params);
