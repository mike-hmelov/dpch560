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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "runtime_params.h"
#include "config.h"

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

/**
 * \brief This function will daemonize this app
 */
static void daemonize() {
    pid_t pid;
    int fd;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Ignore signal sent from child to parent process */
    signal(SIGCHLD, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
        close(fd);
    }

    /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");

    /* Try to write PID of daemon to lockfile */
    if (app_runtime_params.pid_file_name != NULL) {
        char str[20];
        app_runtime_params.pid_fd = open(app_runtime_params.pid_file_name, O_RDWR | O_CREAT, 0640);
        if (app_runtime_params.pid_fd < 0) {
            /* Can't open lockfile */
            exit(EXIT_FAILURE);
        }
        if (lockf(app_runtime_params.pid_fd, F_TLOCK, 0) < 0) {
            /* Can't lock file */
            exit(EXIT_FAILURE);
        }
        /* Get current PID */
        sprintf(str, "%d\n", getpid());
        /* Write PID to lockfile */
        write(app_runtime_params.pid_fd, str, strlen(str));
    }
}

void print_help(const char *name);

int parse_command_line(int argc, char *argv[], int *should_exit);

void clean_runtime(struct runtime_params *params);

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
        daemonize();
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

    /* This global variable can be changed in function handling signal */
    running = 1;

    /* Never ending loop of server */
    while (running == 1) {
        fprintf(app_runtime_params.log_stream, "Debug:\n");
        fflush(app_runtime_params.log_stream);

        /* TODO: dome something useful here */

        /* Real server should use select() or poll() for waiting at
         * asynchronous event. Note: sleep() is interrupted, when
         * signal is received. */
        sleep(app_config.delay);
    }

    /* Close log file, when it is used. */
    if (app_runtime_params.log_stream != stdout) {
        fclose(app_runtime_params.log_stream);
        app_runtime_params.log_stream = NULL;
    }

    /* Write system log and close it. */
    syslog(LOG_INFO, "Stopped %s", app_runtime_params.app_name);
    closelog();

    clean_config(&app_config);
    clean_runtime(&app_runtime_params);

    return EXIT_SUCCESS;
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

void clean_runtime(struct runtime_params *params) {
    /* Free allocated memory */
    free(params->app_name);
    if (params->conf_file_name != NULL) free(params->conf_file_name);
    if (params->log_file_name != NULL) free(params->log_file_name);
    if (params->pid_file_name != NULL) free(params->pid_file_name);
}


// TODO define monitor usb ids
// TODO include USB lib
// TODO send some fixed stuff to usb display
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
