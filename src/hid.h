#ifndef CH560MONITOR_HID_H
#define CH560MONITOR_HID_H

#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "runtime_params.h"

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005

int init_hid_device(struct runtime_params* params);

#endif //CH560MONITOR_HID_H

