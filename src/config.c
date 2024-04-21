#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include "runtime_params.h"
#include "config.h"

/**
 * \brief Read configuration from config file
 */
int read_conf_file(int reload, struct runtime_params* runtime, struct config* target)
{
    FILE *conf_file = NULL;
    int ret;

    if (runtime->conf_file_name == NULL) return 0;
    conf_file = fopen(runtime->conf_file_name, "r");

    if (conf_file == NULL) {
        syslog(LOG_ERR, "Can not open config file: %s, error: %s",
               runtime->conf_file_name, strerror(errno));
        return -1;
    }

    ret = fscanf(conf_file, "%d", &target->delay);

    if (ret > 0) {
        if (reload == 1) {
            syslog(LOG_INFO, "Reloaded configuration file %s of %s",
                   runtime->conf_file_name,
                   runtime->app_name);
        } else {
            syslog(LOG_INFO, "Configuration of %s read from file %s",
                   runtime->app_name,
                   runtime->conf_file_name);
        }
    }

    fclose(conf_file);

    return ret;
}

/**
 * \brief This function tries to test config file
 */
int test_conf_file(char *_conf_file_name)
{
    FILE *conf_file = NULL;
    int ret;
    struct config target = {0};

    conf_file = fopen(_conf_file_name, "r");

    if (conf_file == NULL) {
        fprintf(stderr, "Can't read config file %s\n",
                _conf_file_name);
        return EXIT_FAILURE;
    }

    ret = fscanf(conf_file, "%d", &target.delay);

    if (ret <= 0) {
        fprintf(stderr, "Wrong config file %s\n",
                _conf_file_name);
    }

    fclose(conf_file);

    if (ret > 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

/**
 * \brief Cleanup and resources allocated for config properties
 */
void clean_config(struct config *config)
{

}