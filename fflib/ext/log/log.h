#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include <stdio.h>

struct log_t
{
static int log_impl(const char* mod, const char* fmt, ...)
{
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "%s ", mod);

    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buff + len, sizeof(buff) - len - 1, fmt, vl);
    va_end(vl);
    printf("%s", buff);
    return 0;
}
};

#define BROKER  "BROKER"
#define RPC     "RPC"
#define FF      "FF"
#define MSG_BUS "MSG_BUS"

#define logdebug(content) {}//printf("\033[1;33mDEBUG "); log_t::log_impl content ; printf("\033[0m\n")
#define logtrace(content) {}//printf("TRACE "); log_t::log_impl content ; printf("\n")
#define loginfo(content) {}//printf("\033[1;32mINFO  "); log_t::log_impl content ; printf("\033[0m\n")
#define logwarn(content) printf("\033[1;34mWARN  "); log_t::log_impl content ; printf("\033[0m\n")
#define logerror(content) printf("\033[0;31mERROR "); log_t::log_impl content ; printf("\033[0m\n")
#define logfatal(content) printf("\033[0;35mFATAL "); log_t::log_impl content ; printf("\033[0m")

#endif

