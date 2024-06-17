#pragma once

class Display {
public:
    Display(FILE *log_stream);
    ~Display();

    bool Valid() const;
    void Intro() const;

private:
    bool fValid;
    FILE *fLogStream;
public:
    hid_device* handle;
};
