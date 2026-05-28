//#include "libCPatrolLog/PatrolLogger.h"
#include "PatrolLogger.h"
#include "GetConfigHelper.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>

#include <sys/time.h>

//#define DEFAULT_LOG_DIR  "/home/sunri/patrollogger/"
#define DEFAULT_LOG_DIR  "../patrollogger"
#define PATH_MAXLEN  512
#define LOCATION_MAXLEN     512
#define LOGPREFIX_MAXLEN    128
#define LOGFILENAME_MAXLEN  128
#define LOGPROCESSNAME_MAXLEN   128
#define DEFAULT_LOG_FILE_NAME   "PATROLLOGGER"

static PLogLevel eCurPLogLevel = PLOG_LEVEL_ERROR;   //日志级别
static bool bLogInited = false;  //日志组件是否已完成初始化
static char strLogDirPath[PATH_MAXLEN] = {0};    //日志目录
static bool bEnablePrint = true;    //是否将日志同步输出到终端
static bool bEnableLogLocation = true;  //是否记录日志在源码中的位置信息
static char strLogFileName[LOGFILENAME_MAXLEN] = {0};  //当前记录日志的文件名（不包含日期信息和后缀名）

static bool bEnableLogRawData = false;  //是否启用记录原始数据的功能

static char strLogFilePath[PATH_MAXLEN] = {0};   //当前进程和日期下，记录日志的日志文件路径
static int nLogFileYear = -1;
static int nLogFileMon = -1;
static int nLogFileDay = -1;

static std::mutex logMutex;			//用于多线程日志库写入
static std::mutex initPLogMutex;	//用于多线程初始化日志库

void CreateDir(char * path)
{
	printf("Making dir:%s\n", path);
	
    char data[PATH_MAXLEN];

    //判断是否是当前目录或/目录
    if ((strcmp(path, ".") == 0) || (strcmp(path, "/") == 0))
    {
        return;
    }

    //判断目录是否存在
    if (access(path, F_OK) == 0)
    {
        return;
    }
    else
    {
        //保存目录
        strcpy(data, path);
        //获取目录的父目录
        dirname(path);//返回文件全路径的路径名称，即去掉文件名
		mkdir(data, 0);
		chmod(data, 0777);
        //递归执行
        //CreateDir(path);
    }
	/*
    //创建目录
    if (mkdir(data, 0) == -1)
    {
        perror("mkdir error");
        exit(1);
    }

    chmod(data, 0777);
	*/
}

int GetSelfProcessName(char * name, int maxLen)
{
    char path[1024];
    memset(path, 0, sizeof(path));

    if (readlink("/proc/self/exe", path, sizeof(path) - 1) <= 0)//读取指向/usr/bin/bash的软连接，将当前程序的运行全路径+程序名复制到path里面
    {
        return 0;
    }

    char * pName = strrchr(path, '/');//从后开始查找

    if ((pName != NULL) && (strlen(pName) >= 2))
    {
        int nameLen = strlen(pName + 1);

        if (nameLen > maxLen)
        {
            strncpy(name, pName + 1, maxLen);
            return maxLen;
        }
        else
        {
            strncpy(name, pName + 1, nameLen);
            return nameLen;
        }
    }
    else
    {
        return 0;
    }
}

void CheckAndInitPLog()
{
    if (bLogInited)	return;

    std::lock_guard<std::mutex> locker(initPLogMutex);  //多线程日志库
    if (!bLogInited){
        printf("===== Start initializing PatrolLogger ...\n");
        std::map<std::string, std::string> configMap;
        char strConfigFilePath[PATH_MAXLEN] = {0};
        char * strEnvPrjHome = getenv("PRJHOME");

        if (strEnvPrjHome == NULL){
            printf("$PRJHOME is empty!\n");
        }else{
            printf("$PRJHOME=%s\n", strEnvPrjHome);
            snprintf(strConfigFilePath, PATH_MAXLEN, "%s/resource/conf/patrollogger/patrollogger.conf", strEnvPrjHome);
            printf("PatrolLogger Configuration File Path: %s\n", strConfigFilePath);
            GetConfigHelper::ReadConfig(std::string(strConfigFilePath), configMap);
        }

        std::string strConfigLevel = configMap["PLOG_LEVEL"];
        std::string strConfigLogDirPath = configMap["PLOG_DIR"];
        std::string strConfigEnablePrint = configMap["PLOG_ENABLE_PRINT"];
        std::string strConfigEnableLogLocation = configMap["PLOG_ENABLE_LOCATION"];
        std::string strConfigEnableLogRawData = configMap["PLOG_ENABLE_RAWDATA"];
        int nConfigLevel = atoi(strConfigLevel.c_str());

        if (nConfigLevel == 0){
            nConfigLevel = PLOG_LEVEL_ERROR;
        }

        if (strConfigLogDirPath.empty()){
            strConfigLogDirPath = DEFAULT_LOG_DIR;
        }

        if (strConfigEnablePrint.empty()){
            bEnablePrint = true;
        }else{
            int nConfigEnablePrint = atoi(strConfigEnablePrint.c_str());
            bEnablePrint = (nConfigEnablePrint != 0);
        }

        if (strConfigEnableLogLocation.empty()){
            bEnableLogLocation = true;
        }else{
            int nConfigEnableLogLocation = atoi(strConfigEnableLogLocation.c_str());
            bEnableLogLocation = (nConfigEnableLogLocation != 0);
        }

        if (strConfigEnableLogRawData.empty()){
            bEnableLogRawData = false;
        }else{
            int nLogRawData = atoi(strConfigEnableLogRawData.c_str());
            bEnableLogRawData = (nLogRawData != 0);
        }

        if (!strConfigLogDirPath.empty()){
            if (access(strConfigLogDirPath.c_str(), 0) == -1){
                printf("%s is not existed!\n", strConfigLogDirPath.c_str());
                printf("now making it ...\n");
                char path[PATH_MAXLEN] = {0};
                strcpy(path, strConfigLogDirPath.c_str());
                CreateDir(path);
            }

            strcpy(strLogDirPath, strConfigLogDirPath.c_str());
        }

        eCurPLogLevel = PLogLevel(nConfigLevel);
        char strProcessName[LOGPROCESSNAME_MAXLEN] = {0};
        int resLen = GetSelfProcessName(strProcessName, sizeof(strProcessName));

        if (strlen(strProcessName) == 0){
            strcpy(strLogFileName, DEFAULT_LOG_FILE_NAME);
        }else{
            strncpy(strLogFileName, strProcessName, resLen);
        }

        printf("The PatrolLogger Configurations:\n log_dir=%s\n log_level=%d\n enable_print=%d\n process_name=%s\n logFileName=%s\n log_location=%d\n log_rawdata=%d\n",
               strLogDirPath, eCurPLogLevel, bEnablePrint, strProcessName, strLogFileName, bEnableLogLocation, bEnableLogRawData);
        bLogInited = true;
        printf("===== Initialization of PatrolLogger completed!\n");
    }
}

void checkAndCreateLogFilePath(int nCurYear, int nCurMon, int nCurDay)
{
    if ((nLogFileYear < 0) || (nLogFileYear != nCurYear) || (nLogFileMon != nCurMon) || (nLogFileDay != nCurDay))
    {
        char strLogFilePathDir[PATH_MAXLEN] = {0};
        snprintf(strLogFilePathDir, PATH_MAXLEN, "%s/%0.4d-%0.2d-%0.2d/", strLogDirPath, nCurYear, nCurMon, nCurDay);

        if (access(strLogFilePathDir, 0) == -1)
        {
            printf("%s is not existed!\n", strLogFilePathDir);
            printf("now making it ...\n");
            char path[PATH_MAXLEN] = {0};
            strcpy(path, strLogFilePathDir);
            CreateDir(path);
        }

        memset(strLogFilePath, 0, sizeof(strLogFilePath));
        snprintf(strLogFilePath, PATH_MAXLEN, "%s/%s.log", strLogFilePathDir, strLogFileName);
        nLogFileYear = nCurYear;
        nLogFileMon = nCurMon;
        nLogFileDay = nCurDay;
    }
}

void DoLog(PLogLevel eLogLevel, const char * strLocation, const char * strMessage)
{
    std::lock_guard<std::mutex> locker(logMutex);

    if (strlen(strLogDirPath) == 0)
    {
        return;
    }

    struct timeval tv;
    struct timezone tz;
    struct tm * t;
    gettimeofday(&tv, &tz);

    t = localtime(&tv.tv_sec);
    int nYear = 1900 + t->tm_year;
    int nMonth = 1 + t->tm_mon;
    int nDay = t->tm_mday;
    int nHour = t->tm_hour;
    int nMin = t->tm_min;
    int nSec = t->tm_sec;
    int nUSec = tv.tv_usec;

    checkAndCreateLogFilePath(nYear, nMonth, nDay);//每次写日志都会去检查这个目录的情况
    char logPrefix[LOGPREFIX_MAXLEN] = {0};
    snprintf(logPrefix, LOGPREFIX_MAXLEN,  "[%0.2d:%0.2d:%0.2d.%0.6d] ", nHour, nMin, nSec, nUSec);

    if (eLogLevel == PLOG_LEVEL_DEBUG){
        strcat(logPrefix, "[DEBUG]");
    }else if (eLogLevel == PLOG_LEVEL_INFO){
        strcat(logPrefix, "[INFO]");
    }else if (eLogLevel == PLOG_LEVEL_WARN){
        strcat(logPrefix, "[WARN]");
    }else if (eLogLevel == PLOG_LEVEL_ERROR){
        strcat(logPrefix, "[ERROR]");
    }

    int logFd = open(strLogFilePath, O_WRONLY | O_CREAT | O_APPEND, 0666);
	//FILE * logFd = fopen(strLogFilePath, "ab");
    if (logFd == -1)
    {
        int errorNumber = errno;
        printf("open %s error, errno: %d\n", strLogFilePath, errorNumber);
    }
    else
    {
        write(logFd, logPrefix, strlen(logPrefix));

        if (bEnableLogLocation && (strLocation != NULL))
        {
            write(logFd, " ", 1);
            write(logFd, strLocation, strlen(strLocation));
        }

        write(logFd, ": ", 2);
        write(logFd, strMessage, strlen(strMessage));
        write(logFd, "\n", 1);
        close(logFd);
    }

    if (bEnablePrint)
    {
        if (bEnableLogLocation && (strLocation != NULL))
        {
            printf("%s %s: %s\n", logPrefix, strLocation, strMessage);
        }
        else
        {
            printf("%s: %s\n", logPrefix, strMessage);
        }
    }
}

LIBCPATROLLOG_API PLogLevel CurPLogLevel()
{
    CheckAndInitPLog();
    return eCurPLogLevel;
}

LIBCPATROLLOG_API void PLogDebug(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...)
{
    CheckAndInitPLog();

    if (strSourcePath == NULL || strFuncName == NULL || strFmt == NULL)
    {
        return;
    }

    if (eCurPLogLevel > PLOG_LEVEL_DEBUG)   //配置文件中的日志等级高于此常量时，不记录日志
    {
        return;
    }

    std::vector<char> msg(PLOG_MSG_MAXLEN, 0);
    va_list args;
    va_start(args, strFmt);
    vsnprintf(msg.data(), PLOG_MSG_MAXLEN, strFmt, args);
    va_end(args);

    if (bEnableLogLocation)
    {
        char strLocation[LOCATION_MAXLEN] = {0};
        snprintf(strLocation, LOCATION_MAXLEN, "%s(%d)@%s", strSourcePath, nLineNumber, strFuncName);
        DoLog(PLOG_LEVEL_DEBUG, strLocation, msg.data());
    }
    else
    {
        DoLog(PLOG_LEVEL_DEBUG, NULL, msg.data());
    }
}

LIBCPATROLLOG_API void PLogInfo(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...)
{
    CheckAndInitPLog();

    if (strSourcePath == NULL || strFuncName == NULL || strFmt == NULL)
    {
        return;
    }

    if (eCurPLogLevel > PLOG_LEVEL_INFO)
    {
        return;
    }

    std::vector<char> msg(PLOG_MSG_MAXLEN, 0);
    va_list args;
    va_start(args, strFmt);
    vsnprintf(msg.data(), PLOG_MSG_MAXLEN, strFmt, args);//往容器中输入数据
    va_end(args);

    if (bEnableLogLocation)
    {
        char strLocation[LOCATION_MAXLEN] = {0};
        snprintf(strLocation, LOCATION_MAXLEN, "%s(%d)@%s", strSourcePath, nLineNumber, strFuncName);
        DoLog(PLOG_LEVEL_INFO, strLocation, msg.data());
    }
    else
    {
        DoLog(PLOG_LEVEL_INFO, NULL, msg.data());
    }
}

LIBCPATROLLOG_API void PLogWarn(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...)
{
    CheckAndInitPLog();

    if (strSourcePath == NULL || strFuncName == NULL || strFmt == NULL)
    {
        return;
    }

    if (eCurPLogLevel > PLOG_LEVEL_WARN)
    {
        return;
    }

    std::vector<char> msg(PLOG_MSG_MAXLEN, 0);
    va_list args;
    va_start(args, strFmt);
    vsnprintf(msg.data(), PLOG_MSG_MAXLEN, strFmt, args);
    va_end(args);

    if (bEnableLogLocation)
    {
        char strLocation[LOCATION_MAXLEN] = {0};
        snprintf(strLocation, LOCATION_MAXLEN, "%s(%d)@%s", strSourcePath, nLineNumber, strFuncName);
        DoLog(PLOG_LEVEL_WARN, strLocation, msg.data());
    }
    else
    {
        DoLog(PLOG_LEVEL_WARN, NULL, msg.data());
    }
}

LIBCPATROLLOG_API void PLogError(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...)
{
    CheckAndInitPLog();

    if (strSourcePath == NULL || strFuncName == NULL || strFmt == NULL)
    {
        return;
    }

    if (eCurPLogLevel > PLOG_LEVEL_ERROR)
    {
        return;
    }

    std::vector<char> msg(PLOG_MSG_MAXLEN, 0);
    va_list args;
    va_start(args, strFmt);
    vsnprintf(msg.data(), PLOG_MSG_MAXLEN, strFmt, args);
    va_end(args);

    if (bEnableLogLocation)
    {
        char strLocation[LOCATION_MAXLEN] = {0};
        snprintf(strLocation, LOCATION_MAXLEN, "%s(%d)@%s", strSourcePath, nLineNumber, strFuncName);
        DoLog(PLOG_LEVEL_ERROR, strLocation, msg.data());
    }
    else
    {
        DoLog(PLOG_LEVEL_ERROR, NULL, msg.data());
    }
}

LIBCPATROLLOG_API void PLogRawData(const char * strFileName, const char * strSuffix, const char * data, int len, bool bLogAsHex)
{
    CheckAndInitPLog();

    if (!bEnableLogRawData)
    {
        return;
    }

    if (strFileName == NULL || strlen(strFileName) == 0)
    {
        return;
    }

    if (strSuffix == NULL || strlen(strSuffix) == 0)
    {
        return;
    }

    if (data == NULL)
    {
        return;
    }

    std::lock_guard<std::mutex> locker(logMutex);
    struct timeval tv;
    struct timezone tz;
    struct tm * t;
    gettimeofday(&tv, &tz);
    t = localtime(&tv.tv_sec);
    int nYear = 1900 + t->tm_year;
    int nMonth = 1 + t->tm_mon;
    int nDay = t->tm_mday;
    int nHour = t->tm_hour;
    int nMin = t->tm_min;
    int nSec = t->tm_sec;
    int nUSec = tv.tv_usec;//获取当前的日期，
    checkAndCreateLogFilePath(nYear, nMonth, nDay);
    char strLogRawDataFilePathDir[PATH_MAXLEN] = {0};
    snprintf(strLogRawDataFilePathDir, PATH_MAXLEN, "%s/%0.4d-%0.2d-%0.2d/RawData/", strLogDirPath, nYear, nMonth, nDay);

    if (access(strLogRawDataFilePathDir, 0) == -1)
    {
        char path[PATH_MAXLEN] = {0};
        strcpy(path, strLogRawDataFilePathDir);
        CreateDir(path);
    }

    char strLogRawDataFilePath[PATH_MAXLEN] = {0};
    memset(strLogRawDataFilePath, 0, sizeof(strLogRawDataFilePath));
    char strTimeSection[128] = {0};
    snprintf(strTimeSection, sizeof(strTimeSection), "%0.2d%0.2d%0.2d.%0.6d", nHour, nMin, nSec, nUSec);

    if (strSuffix[0] == '.')
    {
        snprintf(strLogRawDataFilePath, PATH_MAXLEN, "%s/%s.%s%s", strLogRawDataFilePathDir, strFileName, strTimeSection, strSuffix);
    }
    else
    {
        snprintf(strLogRawDataFilePath, PATH_MAXLEN, "%s/%s.%s.%s", strLogRawDataFilePathDir, strFileName, strTimeSection, strSuffix);
    }

    int logFd = open(strLogRawDataFilePath, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if (logFd == -1)
    {
        int errorNumber = errno;
        printf("open %s error, errno: %d\n", strLogRawDataFilePath, errorNumber);
    }
    else
    {
        if (bLogAsHex)
        {
            for (int i = 0; i < len; i++)
            {
                char buf[4] = {0};  //snprintf会将最后一个字节置为0，所以此处需要4个字节
                snprintf(buf, sizeof(buf), "%.2X ", data[i] & 0xFF);
                write(logFd, buf, strlen(buf));
            }
        }
        else
        {
            write(logFd, data, len);
        }

        close(logFd);
    }
}
