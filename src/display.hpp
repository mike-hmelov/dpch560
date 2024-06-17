#pragma once

class Display {
public:
    Display(FILE *log_stream);
    ~Display();

    bool Valid() const;
    void Intro() const;
    void Write(int cpu_usage, int cpu_temp, int gpu_usage, int gpu_temp);

private:
    bool fValid;
    FILE *fLogStream;
    hid_device* fHandle;
};
