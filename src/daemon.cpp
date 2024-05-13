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

#include "common.hpp"
#include "runtime_params.hpp"
#include "demonize.hpp"
#include "config.hpp"
#include "util.hpp"
#include "hid.hpp"
#include "sensors.hpp"

static volatile int running = 0;
Configuration appConfig;
DaemonRuntime appRuntime;


/**
 * \brief Callback function for handling signals.
 * \param	sig	identifier of signal
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        fprintf(appRuntime.log_stream, "Debug: stopping daemon ...\n");
        /* Unlock and close lockfile */
        if (appRuntime.pid_fd != -1) {
            lockf(appRuntime.pid_fd, F_ULOCK, 0);
            close(appRuntime.pid_fd);
        }
        /* Try to delete lockfile */
        if (appRuntime.pid_file_name != nullptr) {
            unlink(appRuntime.pid_file_name);
        }
        running = 0;
        /* Reset signal handling to default behavior */
        signal(SIGINT, SIG_DFL);
    } else if (sig == SIGHUP) {
        fprintf(appRuntime.log_stream, "Debug: reloading daemon config file ...\n");
        appConfig.Read(1, appRuntime.conf_file_name);
    } else if (sig == SIGCHLD) {
        fprintf(appRuntime.log_stream, "Debug: received SIGCHLD signal\n");
    }
}

int parse_command_line(int argc, char *argv[], int *should_exit);

void cleanup(void);

void report_usage_and_temp(HID& hid, unsigned char cpu_usage, unsigned char gpu_usage, int cpu_temp, int gpu_temp);

/* Main function */
int main(int argc, char *argv[]) {
    int should_exit = 0;
    int result = parse_command_line(argc, argv, &should_exit);

    if (should_exit) {
        return result;
    }

    /* When daemonizing is requested at command line. */
    if (appRuntime.start_daemonized) {
        /* It is also possible to use glibc function deamon()
         * at this point, but it is useful to customize your daemon. */
        daemonize(appRuntime);
    }

    /* Open system log and write message to it */
    openlog(appRuntime.app_name, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Started %s", appRuntime.app_name);

    /* Daemon will handle two signals */
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);

    setup_log_file(appRuntime);

    /* Read configuration from config file */
    if (appRuntime.conf_file_name) {
        fprintf(appRuntime.log_stream, "Reading config file from: %s\n", appRuntime.conf_file_name);
        appConfig.Read(0, appRuntime.conf_file_name);
    } else {
        fprintf(appRuntime.log_stream, "No config file given, using defaults\n");
    }

    HID hid(appRuntime.log_stream);
    if(!hid.Valid()){
        cleanup();
        return result;
    }

    result = init_sensors(appConfig, appRuntime);
    if (result) {
        cleanup();
        return result;
    }
    /* This global variable can be changed in function handling signal */
    running = 1;

    unsigned char cpu_usage = 0;
    unsigned char gpu_usage = 0;

    /* Never ending loop of server */
    while (running == 1) {
        report_usage_and_temp(hid, cpu_usage, gpu_usage, cpu_temp(appRuntime), gpu_temp(appRuntime));

        /* Real server should use select() or poll() for waiting at
         * asynchronous event. Note: sleep() is interrupted, when
         * signal is received. */
        sleep(appConfig.Delay());
    }
    cleanup();

    return EXIT_SUCCESS;
}

void report_usage_and_temp(HID& hid, unsigned char cpu_usage, unsigned char gpu_usage, int cpu_temp, int gpu_temp) {
    u_int8_t buf[64] = {0};
    buf[0] = MAGIC_HEADER;
    buf[1] = CELSIUS;
    buf[2] = cpu_usage;
    temp_to_buf(buf + 3, cpu_temp);
    buf[6] = CELSIUS;
    buf[7] = gpu_usage;
    temp_to_buf(buf + 8, gpu_temp);

    int written = hid_write(hid.handle, buf, 64);
    fprintf(appRuntime.log_stream, "written cpuU %d, gpuU: %d, cpuT:%d, gpuT:%d, written:%d\n", cpu_usage,
            gpu_usage, cpu_temp, gpu_temp, written);
    fflush(appRuntime.log_stream);
}

int parse_command_line(int argc, char *argv[], int *should_exit) {
    option long_options[] = {
            {"conf_file", required_argument, 0, 'c'},
            {"test_conf", required_argument, 0, 't'},
            {"log_file",  required_argument, 0, 'l'},
            {"help",      no_argument,       0, 'h'},
            {"daemon",    no_argument,       0, 'd'},
            {"pid_file",  required_argument, 0, 'p'},
            {nullptr, 0,                        0, 0}
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
            case 't': {
                *should_exit = 1;
                Configuration testConfig;
                return testConfig.Test(optarg);
            }
            case 'c':
                appRuntime.conf_file_name = strdup(optarg);
                break;
            case 'l':
                appRuntime.log_file_name = strdup(optarg);
                break;
            case 'p':
                appRuntime.pid_file_name = strdup(optarg);
                break;
            case 'd':
                appRuntime.start_daemonized = 1;
                break;
            default:
                break;
        }
    }
    appRuntime.app_name = strdup(argv[0]);
    *should_exit = 0;
    return EXIT_SUCCESS;
}

void cleanup(void) {
    /* Close log file, when it is used. */
    if (appRuntime.log_stream != stdout) {
        fclose(appRuntime.log_stream);
        appRuntime.log_stream = nullptr;
    }

    /* Write system log and close it. */
    syslog(LOG_INFO, "Stopped %s", appRuntime.app_name);
    closelog();
    cleanup_sensors(appRuntime);
}


// TODO define monitor usb ids
// TODO make full rebrand daemon
// TODO make rpm
// TODO make deb
// TODO make static or lib dependent
// TODO learn cpu usage
// TODO send real cpu usage to display
// TODO learn gpu usage
// TODO send readl gpu usage to display
// TODO lean gpu temp
// TODO send real gpu temp to display
// TODO make gpu configurable
// TODO make grade configurable
// TODO rotate logs
// TODO make ids parametrized (support other devices??)
// TODO find feature by name - rather then just [0]