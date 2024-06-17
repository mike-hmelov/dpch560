#pragma once

class Sensor
{
public:
    Sensor(FILE *logStream, std::string_view &function, std::string_view &name, int specific = 0);
    ~Sensor();

protected:
    static void GlobalDestroy();
    static int sSensorCounter;

public:
    bool Init();
    int Read();

private:
    FILE *fLogStream;
    std::string_view &fFunction;
    std::string_view &fName;
    sensors_chip_name fRootChip;
    sensors_chip_name *fChip;
    sensors_feature *fFeature;
    sensors_subfeature *fSubFeature;
    int fSpecific;
};