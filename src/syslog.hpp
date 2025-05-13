#pragma once

class SysLog {
public:
    explicit SysLog(const char* appName);
    ~SysLog();

public:
    void write(int _pri, const char *_fmt, ...);

private:
    std::string_view _appName;
};