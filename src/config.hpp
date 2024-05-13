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
    char fszCpuSensorName[100]{};
    char fszGpuSensorName[100]{};
public:
    std::string_view fCpuSensorName;
    std::string_view fGpuSensorName;
};

