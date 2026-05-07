#ifndef GETCONFIGHELPER_H
#define GETCONFIGHELPER_H

/*
    配置文件格式：
    key1 = value1
    key2  =  value2

    使用方法：
    std::map<std::string, std::string> m;
    GetConfig::ReadConfig("test2.cfg", m);
    GetConfig::PrintConfig(m);
*/

#include <string>
#include <map>

#define COMMENT_CHAR '#'

class GetConfigHelper
{
public:
    static bool ReadConfig(const std::string & filename, std::map<std::string, std::string> & m);
    static void PrintConfig(const std::map<std::string, std::string> & m);
};

#endif // GETCONFIGHELPER_H
