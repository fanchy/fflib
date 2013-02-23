
#ifndef _FF_TIME_TOOL_H_
#define _FF_TIME_TOOL_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
using namespace std;

//! 获取特定时间的unix 时间戳
struct time_tool_t
{
    static long today_at_zero()//!今日零点时间戳
    {
        time_t now   = ::time(NULL);
        tm    tm_val = *::localtime(&now);
        long ret = (long)now - tm_val.tm_hour*3600 - tm_val.tm_min*60 - tm_val.tm_sec;
        return ret;
    }
    static long next_month()//!下个月开始时间戳
    {
        time_t now   = ::time(NULL);
        tm    tm_val = *::localtime(&now);

        //! 计算这个月有多少天
        static int help_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        int month_day_num = help_month[tm_val.tm_mon];
        if (1 == tm_val.tm_mon)//! 检查2月闰月
        {
            if ((tm_val.tm_year + 1900) % 4 == 0) ++ month_day_num;
        }

        long ret = (long)now + (month_day_num - tm_val.tm_mday)*86400 + (23 - tm_val.tm_hour)*3600 +
                   (59 - tm_val.tm_min)*60 + (60 - tm_val.tm_sec);
        return ret;
    }
};

#endif

