#include <stdio.h>

/**
 * \brief Print help for this application
 */
void print_help(const char* name)
{
    printf("\n Usage: %s [OPTIONS]\n\n", name);
    printf("  Options:\n");
    printf("   -h --help                 Print this help and exit\n");
    printf("   -t --test_conf filename   Test configuration file and exit\n");
    printf("   -c --conf_file filename   Read configuration from the file\n");
    printf("   -l --log_file  filename   Write logs to the file\n");
    printf("   -d --daemon               Run as daemon\n");
    printf("   -p --pid_file  filename   PID file used by daemon\n");
    printf("\n");
}