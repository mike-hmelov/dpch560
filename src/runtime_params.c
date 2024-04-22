#include "runtime_params.h"

void clean_runtime(struct runtime_params *params) {
    /* Free allocated memory */
    free(params->app_name);
    if (params->conf_file_name != NULL)
        free(params->conf_file_name);
    if (params->log_file_name != NULL)
        free(params->log_file_name);
    if (params->pid_file_name != NULL)
        free(params->pid_file_name);
}

