#include "hid.h"

int init_hid_device(struct runtime_params* params) {
    if (hid_init()) {
        fprintf(params->log_stream, "FAILED: init hid api lib failed\n");
        return EXIT_FAILURE;
    }

    params->hid_handle = hid_open(CH560_VENDOR_ID, CH560_PRODUCT_ID, NULL);

    if (!params->hid_handle) {
        fprintf(params->log_stream, "FAILED: open device failed\n");
        return EXIT_FAILURE;
    }

    hid_set_nonblocking(params->hid_handle, 1);

    u_int8_t buf[2] = {16, 170};
    hid_write(params->hid_handle, buf, 64);

    return 0;
}