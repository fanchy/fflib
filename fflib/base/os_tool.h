#ifndef _FF_OS_TOOL_H_
#define _FF_OS_TOOL_H_

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include <string>
#include <vector>
using namespace std;

namespace ff
{

struct os_tool_t
{
    static int ls(const string& path_, vector<string>& ret_)
    {
        const char* dir = path_.c_str();
        DIR *dp;
        struct dirent *entry;

        if((dp = opendir(dir)) == NULL) {
            fprintf(stderr,"cannot open directory: %s\n", dir);
            return -1;
        }

        while((entry = readdir(dp)) != NULL) {
             if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;
            ret_.push_back(entry->d_name);
        }
        closedir(dp);
        return 0;
    }
    static bool is_dir(const string& name_)
    {
        struct stat s;
        if(lstat(name_.c_str(), &s) < 0){ 
            return false;
        }
        return S_ISDIR(s.st_mode);
    }
};
}

#endif
