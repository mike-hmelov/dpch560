#include "common.hpp"
#include "runtime_params.hpp"
#include "display.hpp"

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005
#define MAGIC_HEADER 16
#define CELSIUS 19

Display::Display(FILE *log_stream)
{
    fLogStream = log_stream;
    fValid = false;
    if (hid_init())
    {
        fprintf(fLogStream, "FAILED: init hid api lib failed\n");
        return;
    }

    fHandle = hid_open(CH560_VENDOR_ID, CH560_PRODUCT_ID, nullptr);

    if (!fHandle)
    {
        fprintf(fLogStream, "FAILED: open device failed\n");
        return;
    }

    hid_set_nonblocking(fHandle, 1);

    fValid = true;
}

Display::~Display()
{
    fLogStream = nullptr;

    if (fHandle != nullptr)
        hid_close(fHandle);
    fHandle = nullptr;
    hid_exit();
}

bool Display::Valid() const
{
    return fValid;
}

void Display::Intro() const
{
    u_int8_t buf[2] = {16, 170};
    hid_write(fHandle, buf, 64);
}

void temp_to_buf(u_int8_t *buf, int temp)
{
    int current = temp;
    *(buf + 2) = current % 10;
    current /= 10;
    *(buf + 1) = current % 10;
    current /= 10;
    *(buf) = current % 10;
}

void Display::Write(int cpu_usage, int cpu_temp, int gpu_usage, int gpu_temp)
{
    u_int8_t buf[64] = {0};
    buf[0] = MAGIC_HEADER;
    buf[1] = CELSIUS;
    buf[2] = cpu_usage;
    temp_to_buf(buf + 3, cpu_temp);
    buf[6] = CELSIUS;
    buf[7] = gpu_usage;
    temp_to_buf(buf + 8, gpu_temp);

    int written = hid_write(fHandle, buf, 64);
    fprintf(fLogStream, "written cpuU %d, gpuU: %d, cpuT:%d, gpuT:%d, written:%d\n", cpu_usage,
            gpu_usage, cpu_temp, gpu_temp, written);
    fflush(fLogStream);
}