#pragma once
#include <stdlib.h>

class DaemonRuntime
{
public:
    DaemonRuntime();
    ~DaemonRuntime();

public:
    void ParseCommandLine(int argc, char** argv);

    bool ExitAtOnce() const { return _shouldExit; }

    int ExitCode() const { return _exitCode; }

    bool StartAsDaemon() const { return _startAsDaemon; }

    const char* GetName() const { return app_name; }

private:
    bool _shouldExit = false;
    int _exitCode = EXIT_SUCCESS;
    bool _startAsDaemon = false;
    char* app_name = nullptr;

public:
    char* conf_file_name{};
    char* log_file_name{};
    char* pid_file_name{};
    int pid_fd;
    FILE* log_stream{};
};
