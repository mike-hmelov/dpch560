#pragma once

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005

class HID {
public:
    HID(FILE *log_stream);
    ~HID();

    bool Valid() const;
    void Intro() const;

private:
    bool fValid;
    FILE *fLogStream;
public:
    hid_device* handle;
};
