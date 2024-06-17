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

#include <memory>

#include "common.hpp"
#include "runtime_params.hpp"
#include "demonize.hpp"
#include "config.hpp"
#include "util.hpp"
#include "display.hpp"
#include "sensors.hpp"
#include "syslog.hpp"

static volatile int running = 0;
Configuration appConfig;
DaemonRuntime appRuntime;
[[maybe_unused]] std::unique_ptr<SysLog> sysLog;

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

    sysLog = std::make_unique<SysLog>(appRuntime);

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

    Display display(appRuntime.log_stream);
    Sensor cpuSensor(appRuntime.log_stream, appConfig.fCpuFunction, appConfig.fCpuSensorName, appConfig.fCpuSensorFunction);
    Sensor gpuSensor(appRuntime.log_stream, appConfig.fGpuFunction, appConfig.fGpuSensorName, appConfig.fGpuSensorFunction);

    if(!display.Valid()){
        return -4;
    }
    if(!cpuSensor.Init() || !gpuSensor.Init())
    {
        return -5;
    }
    /* This global variable can be changed in function handling signal */
    running = 1;

    unsigned char cpu_usage = 0;
    unsigned char gpu_usage = 0;

    /* Never ending loop of server */
    while (running == 1) {
        display.Write(cpu_usage, cpuSensor.Read(), gpu_usage, gpuSensor.Read());

        /* Real server should use select() or poll() for waiting at
         * asynchronous event. Note: sleep() is interrupted, when
         * signal is received. */
        sleep(appConfig.Delay());
    }

    return EXIT_SUCCESS;
}

int parse_command_line(int argc, char *argv[], int *should_exit) {
    option long_options[] = {
            {"conf_file", required_argument, nullptr, 'c'},
            {"test_conf", required_argument, nullptr, 't'},
            {"log_file",  required_argument, nullptr, 'l'},
            {"help",      no_argument,       nullptr, 'h'},
            {"daemon",    no_argument,       nullptr, 'd'},
            {"pid_file",  required_argument, nullptr, 'p'},
            {nullptr, 0,                        nullptr, 0}
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


// TODO define monitor usb ids
// TODO make full rebrand daemon
// TODO make rpm
// TODO make deb
// TODO make static or lib dependent
// TODO learn cpu usage
// TODO send real cpu usage to display
// TODO learn gpu usage
// TODO send readl gpu usage to display
// TODO make gpu configurable
// TODO make grade configurable
// TODO rotate logs
// TODO make ids parametrized (support other devices??)
// TODO find feature by name - rather then just [0]