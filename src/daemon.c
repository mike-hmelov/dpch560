/*
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 * Contributor(s): Jiri Hnidek <jiri.hnidek@tul.cz>.
 * Contributor(s): Mihail Hmelov <darkdemon@inbox.lv>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <hidapi/hidapi.h>
#include "runtime_params.h"
#include "config.h"

#define CH560_VENDOR_ID 0x3633
#define CH560_PRODUCT_ID 0x0005

#define MAGIC_HEADER 16
#define CELSIUS 19

static volatile int running = 0;
static struct config app_config = {.delay = 1};
static struct runtime_params app_runtime_params = {.pid_fd=-1};

/**
 * \brief Callback function for handling signals.
 * \param	sig	identifier of signal
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        fprintf(app_runtime_params.log_stream, "Debug: stopping daemon ...\n");
        /* Unlock and close lockfile */
        if (app_runtime_params.pid_fd != -1) {
            lockf(app_runtime_params.pid_fd, F_ULOCK, 0);
            close(app_runtime_params.pid_fd);
        }
        /* Try to delete lockfile */
        if (app_runtime_params.pid_file_name != NULL) {
            unlink(app_runtime_params.pid_file_name);
        }
        running = 0;
        /* Reset signal handling to default behavior */
        signal(SIGINT, SIG_DFL);
    } else if (sig == SIGHUP) {
        fprintf(app_runtime_params.log_stream, "Debug: reloading daemon config file ...\n");
        read_conf_file(1, &app_runtime_params, &app_config);
    } else if (sig == SIGCHLD) {
        fprintf(app_runtime_params.log_stream, "Debug: received SIGCHLD signal\n");
    }
}

void daemonize(struct runtime_params* params);

void print_help(const char *name);

int parse_command_line(int argc, char *argv[], int *should_exit);

void cleanup(void);

void temp_to_buf(u_int8_t *buf, int temp);

void report_usage_and_temp(unsigned char cpu_usage, unsigned char gpu_usage, int cpu_temp, int gpu_temp);

/* Main function */
int main(int argc, char *argv[]) {
    int should_exit = 0;
    int result = parse_command_line(argc, argv, &should_exit);

    if (should_exit) {
        return result;
    }

    /* When daemonizing is requested at command line. */
    if (app_runtime_params.start_daemonized) {
        /* It is also possible to use glibc function deamon()
         * at this point, but it is useful to customize your daemon. */
        daemonize(&app_runtime_params);
    }

    /* Open system log and write message to it */
    openlog(app_runtime_params.app_name, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Started %s", app_runtime_params.app_name);

    /* Daemon will handle two signals */
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);

    /* Try to open log file to this daemon */
    if (app_runtime_params.log_file_name != NULL) {
        app_runtime_params.log_stream = fopen(app_runtime_params.log_file_name, "a+");
        if (app_runtime_params.log_stream == NULL) {
            syslog(LOG_ERR, "Can not open log file: %s, error: %s",
                   app_runtime_params.log_file_name, strerror(errno));
            app_runtime_params.log_stream = stdout;
        }
    } else {
        app_runtime_params.log_stream = stdout;
    }

    /* Read configuration from config file */
    if (app_runtime_params.conf_file_name) {
        fprintf(app_runtime_params.log_stream, "Reading config file from: %s\n", app_runtime_params.conf_file_name);
        read_conf_file(0, &app_runtime_params, &app_config);
    } else {
        fprintf(app_runtime_params.log_stream, "No config file given, using defaults\n");
    }

    if (hid_init()) {
        fprintf(app_runtime_params.log_stream, "FAILED: init hid api lib failed\n");
        cleanup();
        return EXIT_FAILURE;
    }

    app_runtime_params.hid_handle = hid_open(CH560_VENDOR_ID, CH560_PRODUCT_ID, NULL);

    if (!app_runtime_params.hid_handle) {
        fprintf(app_runtime_params.log_stream, "FAILED: open device failed\n");
        cleanup();
        return EXIT_FAILURE;
    }

    hid_set_nonblocking(app_runtime_params.hid_handle, 1);

    u_int8_t buf[2] = {16, 170};
    hid_write(app_runtime_params.hid_handle, buf, 64);

    /* This global variable can be changed in function handling signal */
    running = 1;

    unsigned char cpu_usage = 0;
    unsigned char gpu_usage = 0;
    int cpu_temp = 0;
    int gpu_temp = 0;


    /* Never ending loop of server */
    while (running == 1) {
        report_usage_and_temp(cpu_usage, gpu_usage, cpu_temp, gpu_temp);

        /* Real server should use select() or poll() for waiting at
         * asynchronous event. Note: sleep() is interrupted, when
         * signal is received. */
        sleep(app_config.delay);
    }
    cleanup();

    return EXIT_SUCCESS;
}

void report_usage_and_temp(unsigned char cpu_usage, unsigned char gpu_usage, int cpu_temp, int gpu_temp) {
    u_int8_t buf[64] = {0};
    buf[0] = MAGIC_HEADER;
    buf[1] = CELSIUS;
    buf[2] = cpu_usage;
    temp_to_buf(buf + 3, cpu_temp);
    buf[6] = CELSIUS;
    buf[7] = gpu_usage;
    temp_to_buf(buf + 8, gpu_temp);

    int written = hid_write(app_runtime_params.hid_handle, buf, 64);
    fprintf(app_runtime_params.log_stream, "written cpuU %d, gpuU: %d, cpuT:%d, gpuT:%d, written:%d\n", cpu_usage,
            gpu_usage, cpu_temp, gpu_temp, written);
    fflush(app_runtime_params.log_stream);
}

void temp_to_buf(u_int8_t *buf, int temp) {
    int current = temp;
    *(buf + 2) = current % 10;
    current /= 10;
    *(buf + 1) = current % 10;
    current /= 10;
    *(buf) = current % 10;
}

int parse_command_line(int argc, char *argv[], int *should_exit) {
    static struct option long_options[] = {
            {"conf_file", required_argument, 0, 'c'},
            {"test_conf", required_argument, 0, 't'},
            {"log_file",  required_argument, 0, 'l'},
            {"help",      no_argument,       0, 'h'},
            {"daemon",    no_argument,       0, 'd'},
            {"pid_file",  required_argument, 0, 'p'},
            {NULL, 0,                        0, 0}
    };
    int value, option_index = 0;

    /* Try to process all command line arguments */
    while ((value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &option_index)) != -1) {
        switch (value) {
            case 'h':
                print_help(argv[0]);
                *should_exit = 1;
                return EXIT_SUCCESS;
            case '?':
                print_help(argv[0]);
                *should_exit = 1;
                return EXIT_FAILURE;
            case 't':
                *should_exit = 1;
                return test_conf_file(optarg);
            case 'c':
                app_runtime_params.conf_file_name = strdup(optarg);
                break;
            case 'l':
                app_runtime_params.log_file_name = strdup(optarg);
                break;
            case 'p':
                app_runtime_params.pid_file_name = strdup(optarg);
                break;
            case 'd':
                app_runtime_params.start_daemonized = 1;
                break;
            default:
                break;
        }
    }
    app_runtime_params.app_name = strdup(argv[0]);
    *should_exit = 0;
    return EXIT_SUCCESS;
}

void cleanup(void) {
    if (app_runtime_params.hid_handle != NULL)
        hid_close(app_runtime_params.hid_handle);
    hid_exit();

    /* Close log file, when it is used. */
    if (app_runtime_params.log_stream != stdout) {
        fclose(app_runtime_params.log_stream);
        app_runtime_params.log_stream = NULL;
    }

    /* Write system log and close it. */
    syslog(LOG_INFO, "Stopped %s", app_runtime_params.app_name);
    closelog();
    clean_config(&app_config);
    /* Free allocated memory */
    free(app_runtime_params.app_name);
    if (app_runtime_params.conf_file_name != NULL)
        free((&app_runtime_params)->conf_file_name);
    if (app_runtime_params.log_file_name != NULL)
        free((&app_runtime_params)->log_file_name);
    if (app_runtime_params.pid_file_name != NULL)
        free((&app_runtime_params)->pid_file_name);
}


// TODO define monitor usb ids
// TODO make full rebrand daemon
// TODO make rpm
// TODO make deb
// TODO make static or lib dependent
// TODO learn cpu usage
// TODO send real cpu usage to display
// TODO learn cpu temp
// TODO send real cpu temp to display
// TODO learn gpu usage
// TODO send readl gpu usage to display
// TODO lean gpu temp
// TODO send real gpu temp to display
// TODO make gpu configurable
// TODO make grade configurable
// TODO rotate logs
// TODO make ids parametrized (support other devices??)