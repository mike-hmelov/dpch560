#pragma once

#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "runtime_params.hpp"

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005

int init_hid_device(DaemonRuntime& params);


