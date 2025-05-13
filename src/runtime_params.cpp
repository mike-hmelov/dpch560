#include "common.hpp"
#include "runtime_params.hpp"
#include "util.hpp"
#include "config.hpp"

DaemonRuntime::DaemonRuntime()
{
    pid_fd = -1;
}

void DaemonRuntime::ParseCommandLine(int argc, char *argv[])
{
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

    while ((value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &option_index)) != -1) {
        switch (value) {
        case 'h':
            print_help(argv[0]);
            _shouldExit = true;
            _exitCode = EXIT_SUCCESS;
            break;
        case '?':{
            print_help(argv[0]);
            _shouldExit = true;
            _exitCode = EXIT_FAILURE;
            break;
        }
        case 't': {
                Configuration testConfig;
                _exitCode = testConfig.Test(optarg);
                _shouldExit = true;
                break;
        }
        case 'c':
            conf_file_name = strdup(optarg);
            break;
        case 'l':
            log_file_name = strdup(optarg);
            break;
        case 'p':
            pid_file_name = strdup(optarg);
            break;
        case 'd':
            _startAsDaemon = true;
            break;
        default:
            break;
        }
    }
    app_name = strdup(argv[0]);
    _exitCode = EXIT_SUCCESS;
}

DaemonRuntime::~DaemonRuntime()
{
    free(app_name);
    if (conf_file_name != nullptr)
        free(conf_file_name);
    if (log_file_name != nullptr)
        free(log_file_name);
    if (pid_file_name != nullptr)
        free(pid_file_name);
    if (log_stream != stdout && log_stream != nullptr)
     {
        fclose(log_stream);
    }
}
