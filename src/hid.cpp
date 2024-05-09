#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

#include "runtime_params.hpp"
#include "hid.hpp"

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005

HID::HID(FILE *log_stream) {
    fLogStream = log_stream;
    fValid = false;
    if (hid_init()) {
        fprintf(fLogStream, "FAILED: init hid api lib failed\n");
        return;
    }

    handle = hid_open(CH560_VENDOR_ID, CH560_PRODUCT_ID, nullptr);

    if (!handle) {
        fprintf(fLogStream, "FAILED: open device failed\n");
        return;
    }

    hid_set_nonblocking(handle, 1);

    fValid = true;
}

HID::~HID()
{
    fLogStream = nullptr;

    if (handle != nullptr)
        hid_close(handle);
    hid_exit();
}

bool HID::Valid() const
{
    return fValid;
}

void HID::Intro() const {
    u_int8_t buf[2] = {16, 170};
    hid_write(handle, buf, 64);
}