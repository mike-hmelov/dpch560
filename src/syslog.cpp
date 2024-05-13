#include "runtime_params.hpp"
#include "syslog.hpp"

SysLog::SysLog(DaemonRuntime& params): app_name(params.app_name)
{
    openlog(app_name.data(), LOG_PID | LOG_CONS, LOG_DAEMON);
    write(LOG_INFO, "Started %s", app_name.data());
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
    write(LOG_INFO, "Stopped %s", app_name.data());
    closelog();
}
