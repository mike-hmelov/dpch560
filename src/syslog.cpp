#include "common.hpp"
#include "runtime_params.hpp"
#include "syslog.hpp"

SysLog::SysLog(const char* appName): _appName(appName)
{
    openlog(_appName.data(), LOG_PID | LOG_CONS, LOG_DAEMON);
    write(LOG_INFO, "Started %s", _appName.data());
}

void SysLog::write(int __pri, const char *__fmt, ...)
{
    va_list args;
    va_start(args, __fmt);
    syslog(__pri, __fmt, args);
    va_end(args);
}

SysLog::~SysLog() {
    /* Write system log and close it. */
    write(LOG_INFO, "Stopped %s", _appName.data());
    closelog();
}
