
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"
#include "error.h"

static void log_format(char *tag, const char *fmt, va_list ap) {
    time_t now;
    char *date;

    time(&now);
    if ((date = ctime(&now)) == NULL) {
        err_sys("ctime error");
    }
    date[strlen(date) - 1] = '\0';
    printf("%s [%s] ", date, tag);
    vprintf(fmt, ap);
    printf("\n"); 
}

void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_format(ERROR, fmt, ap);
    va_end(ap);
}

void log_warn(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_format(WARN, fmt, ap);
    va_end(ap);
}

void log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_format(INFO, fmt, ap);
    va_end(ap);    
}