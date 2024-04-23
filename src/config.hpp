#pragma once

class Configuration {
public:
    Configuration();

    ~Configuration();

public:
    int Read(int reload, const char *fileName);

    int Test(const char *fileName);


public:
    int fDelay;
    char fCpuSensorName[100];
    char fGpuSensorName[100];
};

