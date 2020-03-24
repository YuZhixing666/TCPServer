#ifndef LOGGER_H
#define LOGGER_H

#define ERROR   "ERROR"
#define WARN    "WARN"
#define INFO    "INFO"


void log_error(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_info(const char *fmt, ...);

#endif
