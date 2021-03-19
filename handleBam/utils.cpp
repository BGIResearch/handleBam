/*
 * File: utils.cpp
 * Created Data: 2020-5-12
 * Author: fxzhao
 * Contact: <zhaofuxiang@genomics.cn>
 *
 * Copyright (c) 2020 BGI-Research
 */

#ifdef _WIN32
#include <direct.h>
#define popen _popen
#define pclose _pclose
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

#include "utils.h"
#include <sstream>
#include <string.h>

#include <chrono>
#include <filesystem>
#include <random>
namespace fs = std::filesystem;

using std::string;
using std::random_device;
using std::default_random_engine;

int exec_shell(const char* cmd, std::vector< std::string >& resvec)
{
    resvec.clear();
    FILE* pp = popen(cmd, "r");  // make pipe
    if (!pp)
    {
        return -1;
    }
    char tmp[1024];  // store the stdout per line
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        if (tmp[strlen(tmp) - 1] == '\n')
        {
            tmp[strlen(tmp) - 1] = '\0';
        }
        resvec.push_back(tmp);
    }

    // close pipe, the return code is cmd's status
    // returns the exit status of the terminating command processor
    // -1 if an error occurs
    int rtn = pclose(pp);
#ifndef _WIN32
    rtn = WEXITSTATUS(rtn);
#endif

    return rtn;
}

size_t physical_memory_used_by_process()
{
    int result = 0;
#ifndef _WIN32
    FILE* file = fopen("/proc/self/status", "r");
    char  line[128];
    while (fgets(line, 128, file) != nullptr)
    {
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            int len = strlen(line);

            const char* p = line;
            for (; std::isdigit(*p) == false; ++p)
            {
            }

            line[len - 3] = 0;
            result        = atoi(p);

            break;
        }
    }
    fclose(file);
#endif

    return result;
}

vector< string > split_str(const std::string& str, char delim, bool skip_empty)
{
    std::istringstream iss(str);
    vector< string >   res;
    for (std::string item; getline(iss, item, delim);)
        if (skip_empty && item.empty())
            continue;
        else
            res.push_back(item);
    return res;
}

bool check_file_older(const std::string& first, const std::string& second)
{
    fs::path p1(first), p2(second);
    auto     t1 = fs::last_write_time(p1);
    auto     t2 = fs::last_write_time(p2);

    return (std::chrono::duration_cast< std::chrono::seconds >(t1 - t2).count() <= 0);
}

string rand_str(int length)
{
    char tmp;
    string buffer;
    
    random_device rd;
    default_random_engine random(rd());
    
    for (int i = 0; i < length; i++) {
        tmp = random() % 36;
        if (tmp < 10) {
            tmp += '0';
        } else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return buffer;
}
