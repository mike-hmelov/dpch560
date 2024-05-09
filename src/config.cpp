#include <cstring>
#include <cerrno>
#include <cstdlib>

#include <syslog.h>

#include "runtime_params.hpp"
#include "config.hpp"

Configuration::Configuration() {
    fDelay = 2;
    strncpy(fCpuSensorName, "k10temp-pci-00c3", sizeof(fCpuSensorName));
    strncpy(fGpuSensorName, "amdgpu-pci-0300", sizeof(fGpuSensorName));
}

Configuration::~Configuration() = default;

int Configuration::Read(int reload, const char *fileName) {
    if (fileName == nullptr) return 0;

    FILE *conf_file = fopen(fileName, "r");

    if (conf_file == nullptr) {
        syslog(LOG_ERR, "Can not open config file: %s, error: %s", fileName, strerror(errno));
        return -1;
    }

//    target->delay = 1;
//    strcpy(target->cpu_sensor_name, "k10temp-pci-00c3");
//    strcpy(target->gpu_sensor_name, "amdgpu-pci-1200");

//    ret = fscanf(conf_file, "%d", &target->delay);

//    if (ret > 0) {
//        if (reload == 1) {
//            syslog(LOG_INFO, "Reloaded configuration file %s of %s",
//                   runtime->conf_file_name,
//                   runtime->app_name);
//        } else {
//            syslog(LOG_INFO, "Configuration of %s read from file %s",
//                   runtime->app_name,
//                   runtime->conf_file_name);
//        }
//    }

    fclose(conf_file);

    return 0;
}

int Configuration::Test(const char *fileName) {
//    FILE *conf_file = NULL;
//    int ret;
//    struct config target = {0};
//
//    conf_file = fopen(fileName, "r");
//
//    if (conf_file == NULL) {
//        fprintf(stderr, "Can't read config file %s\n",
//                fileName);
//        return EXIT_FAILURE;
//    }
//
//    ret = fscanf(conf_file, "%d", &target.delay);
//
//    if (ret <= 0) {
//        fprintf(stderr, "Wrong config file %s\n",
//                fileName);
//    }
//
//    fclose(conf_file);
//
//    if (ret > 0)
    return EXIT_SUCCESS;
//    else
//        return EXIT_FAILURE;
}

