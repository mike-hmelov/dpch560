#ifndef CH560MONITOR_CONFIG_H
#define CH560MONITOR_CONFIG_H

struct config {
    int delay;
};

int read_conf_file(int reload, struct runtime_params* runtime, struct config* target);
int test_conf_file(char *_conf_file_name);
void clean_config(struct config *config);

#endif //CH560MONITOR_CONFIG_H
