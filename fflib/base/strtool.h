#ifndef _STRTOOL_H_
#define _STRTOOL_H_


#include <string.h>
#include <iostream>
#include <vector>
using namespace std;

struct strtool
{
static string trim(const string& str)
{
    string::size_type pos = str.find_first_not_of(' ');
    if (pos == string::npos)
    {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

static int split(const string& str, vector<string>& ret_, string sep = ",")
{
    if (str.empty())
    {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

static string replace(const string& str, const string& src, const string& dest)
{
    string ret;

    string::size_type pos_begin = 0;
    string::size_type pos       = str.find(src);
    while (pos != string::npos)
    {
        //cout <<"replacexxx:" << pos_begin <<" " << pos <<"\n";
        ret.append(str.c_str() + pos_begin, pos - pos_begin);
        ret += dest;
        pos_begin = pos + src.length();
        pos       = str.find(src, pos_begin);
    }
    if (pos_begin < str.length())
    {
        ret.append(str.begin() + pos_begin, str.end());
    }
    return ret;
}

size_t utf8_words_num(const char* s_)
{
    size_t ret = 0;
    const char* p = s_;
    for (unsigned char c = (unsigned char)(*p); c != 0; c = (unsigned char)(*p))
    {
        ++ret;
        if (c <= 127)
        {
            p += 1;
        }
        else if (c < 192)
        {
            p   += 2;
        }
        else if (c < 223)
        {
            p   += 3;
        }
        else
        {
            p   += 4;
        }
    }
    return ret;
}
};

typedef strtool strtool_t;
#endif

