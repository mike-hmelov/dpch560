#pragma once

class Configuration {
public:
    Configuration();

    ~Configuration();

public:
    int Read(int reload, const char *fileName);

    int Test(const char *fileName);

    [[nodiscard]] int Delay() const { return fDelay; }

private:
    int fDelay;
public:
    char fCpuSensorName[100]{};
    char fGpuSensorName[100]{};
};

