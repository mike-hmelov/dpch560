#include "common.hpp"
#include "runtime_params.hpp"
#include "config.hpp"
#include "sensors.hpp"

int init_sensors(Configuration& config, DaemonRuntime& params) {
    fprintf(params.log_stream, "Using sensor %s to get CPU temp\n", config.fCpuSensorName.data());

    int result = sensors_init(nullptr);
    if (result) {
        fprintf(params.log_stream, "Failed to init sensors lib\n");
        return result;
    }

    result = sensors_parse_chip_name(config.fCpuSensorName.data(), &params.cpu_root_chip);

    if (result) {
        fprintf(params.log_stream, "Failed to find sensor %s\n", config.fCpuSensorName.data());
        return result;
    }

    int chip_nr = 0;
    params.cpu_chip = sensors_get_detected_chips(&params.cpu_root_chip, &chip_nr);

    if(!params.cpu_chip){
        fprintf(params.log_stream, "Failed to detect sensor with name %s\n", config.fCpuSensorName.data());
        return -1;
    }

    result = sensors_parse_chip_name(config.fGpuSensorName.data(), &params.gpu_root_chip);

    if (result) {
        fprintf(params.log_stream, "Failed to find sensor %s\n", config.fGpuSensorName.data());
        return result;
    }

    chip_nr = 0;
    params.gpu_chip = sensors_get_detected_chips(&params.gpu_root_chip, &chip_nr);

    if(!params.gpu_chip){
        fprintf(params.log_stream, "Failed to detect sensor with name %s\n", config.fGpuSensorName.data());
        return -1;
    }

    return 0;
}

void cleanup_sensors(DaemonRuntime& params) {
    sensors_free_chip_name(&params.cpu_root_chip);
    sensors_free_chip_name(&params.gpu_root_chip);
    sensors_cleanup();
}

int cpu_temp(DaemonRuntime& params) {
    if (!params.cpu_feature) {
        int a = 0;
        params.cpu_feature = sensors_get_features(params.cpu_chip, &a);
    }

    if (!params.cpu_feature) {
        fprintf(params.log_stream, "Failed to read CPU temp, cannot get feature\n");
        return -1;
    }

    if (!params.cpu_sub) {
        int b = 0;
        params.cpu_sub = sensors_get_all_subfeatures(params.cpu_chip, params.cpu_feature, &b);
    }

    if (!params.cpu_sub) {
        fprintf(params.log_stream, "Failed to read CPU temp, cannot get sub feature\n");
        return -1;
    }

    double val;
    int err = sensors_get_value(params.cpu_chip, params.cpu_sub->number, &val);
    if (err) {
        fprintf(params.log_stream, "ERROR: Can't get value of subfeature %s: %s\n",
                params.cpu_sub->name, sensors_strerror(err));
        return -1;
    } else {
        return (int) val;
    }
}

int gpu_temp(DaemonRuntime& params) {
    if (!params.gpu_feature) {
        int a = SENSORS_FEATURE_TEMP;
        params.gpu_feature = sensors_get_features(params.gpu_chip, &a);
    }

    if (!params.gpu_feature) {
        fprintf(params.log_stream, "Failed to read GPU temp, cannot get feature\n");
        return -1;
    }

    if (!params.gpu_sub) {
        int b = 0;
        params.gpu_sub = sensors_get_all_subfeatures(params.gpu_chip, params.gpu_feature, &b);
    }

    if (!params.gpu_sub) {
        fprintf(params.log_stream, "Failed to read GPU temp, cannot get sub feature\n");
        return -1;
    }

    double val;
    int err = sensors_get_value(params.gpu_chip, params.gpu_sub->number, &val);
    if (err) {
        fprintf(params.log_stream, "ERROR: Can't get value of subfeature %s: %s\n",
                params.gpu_sub->name, sensors_strerror(err));
        return -1;
    } else {
        return (int) val;
    }
}