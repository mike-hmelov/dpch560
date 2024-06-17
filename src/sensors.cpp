#include "common.hpp"
#include "sensors.hpp"

Sensor::Sensor(FILE *logStream, std::string_view &function, std::string_view &name, int specific) 
                : fLogStream(logStream), fFunction(function), fName(name), fSpecific(specific)
{
}

bool Sensor::Init()
{
    int result = sensors_init(nullptr);
    if (result)
    {
        fprintf(fLogStream, "Failed to init sensors lib\n");
        return false;
    }
    sSensorCounter++;

    fprintf(fLogStream, "Using sensor %s to get %s temp\n", fName.data(), fFunction.data());
    result = sensors_parse_chip_name(fName.data(), &fRootChip);

    if (result)
    {
        fprintf(fLogStream, "Failed to find sensor %s\n", fName.data());
        return false;
    }

    int chip_nr = 0;
    fChip = (sensors_chip_name*)sensors_get_detected_chips(&fRootChip, &chip_nr);

    if (!fChip)
    {
        fprintf(fLogStream, "Failed to detect sensor with name %s\n", fName.data());
        return false;
    }
    return true;
}

Sensor::~Sensor()
{
    sensors_free_chip_name(&fRootChip);
    Sensor::GlobalDestroy();
}

int Sensor::Read()
{
    if (!fFeature)
    {
        int a = fSpecific;
        fFeature = (sensors_feature*)sensors_get_features(fChip, &a);
    }

    if (!fFeature)
    {
        fprintf(fLogStream, "Failed to read %s temp, cannot get feature\n", fFunction.data());
        return -1;
    }

    if (!fSubFeature)
    {
        int b = 0;
        fSubFeature = (sensors_subfeature*)sensors_get_all_subfeatures(fChip, fFeature, &b);
    }

    if (!fSubFeature)
    {
        fprintf(fLogStream, "Failed to read %s temp, cannot get sub feature\n", fFunction.data());
        return -1;
    }

    double val;
    int err = sensors_get_value(fChip, fSubFeature->number, &val);
    if (err)
    {
        fprintf(fLogStream, "ERROR: Can't get value of subfeature %s: %s\n",
                fName.data(), sensors_strerror(err));
        return -1;
    }
    else
    {
        return (int)val;
    }
}

int Sensor::sSensorCounter = 0;
void Sensor::GlobalDestroy()
{
    if (sSensorCounter == 0)
        sensors_cleanup();
    else
        sSensorCounter--;
}
