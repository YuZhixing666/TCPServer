#ifndef ERROR_WRAP_H
#define ERROR_WRAP_H

#define MAXLINE 1024

#include <errno.h> /* for definition of errno */
#include <stdarg.h>/* ISO C variable aruments */
#include <netinet/in.h>

static void err_doit(int errnoflag, int error, const char *fmt, va_list ap);
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif