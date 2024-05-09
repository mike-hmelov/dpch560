#pragma once

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
