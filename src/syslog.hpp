#pragma once

class SysLog {
public:
    explicit SysLog(DaemonRuntime& params);
    ~SysLog();

public:
    void write(int _pri, const char *_fmt, ...);

private:
    std::string_view app_name;
};