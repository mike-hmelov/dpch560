#pragma once

class Configuration
{
public:
    Configuration();

    ~Configuration();

public:
    int Read(int reload, const char *fileName);

    int Test(const char *fileName);

    [[nodiscard]] int Delay() const { return fDelay; }

public:
    std::string_view fCpuSensorName;
    std::string_view fGpuSensorName;
    std::string_view fCpuFunction;
    std::string_view fGpuFunction;
    int fCpuSensorFunction;
    int fGpuSensorFunction;

private:
    int fDelay;
};
