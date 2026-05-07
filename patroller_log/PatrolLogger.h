#ifndef PATROLLOGGER_H
#define PATROLLOGGER_H

/*
============ 组件说明 =============
本日志组件仅在巡视系统中使用，设计目的：
（1）让日志组件的引入尽可能轻量和简单，让使用日志组件的依赖尽可能少（只依赖标准库和本地文件系统，不再去依赖服务总线等重量级组件）；
（2）考虑到巡视系统中涉及多媒体信息调试，增加了日志记录多媒体调试信息的支持；

============ 配置方法  =============
本日志组件配置文件路径：$PRJHOME/resource/conf/patrollogger/patrollogger.conf
配置文件内容格式：
#日志输出级别，
#支持1（DEBUG）， 2（INFO）， 3（WARN）， 4（ERROR）， 5（NOLOG）如果不配置，默认为4（ERROR）
PLOG_LEVEL=1

#日志记录根目录，如果不配置，默认为/home/sunri/patrollogger/目录
PLOG_DIR=/home/sunri/patrollogger/

#日志是否同步输出到终端，
#支持0（不输出）， 1（输出），如果不配置，默认为1（输出）
PLOG_ENABLE_PRINT=1

#日志是否记录日志输出的位置信息（打印日志时所在源文件(所在行数)@所在函数名），
#支持0（不记录），1（记录），如果不配置，默认为1（记录）
PLOG_ENABLE_LOCATION=1

============= 使用方法 ==============
引入PatrolLogger.h头文件，然后使用PLOG_DEBUG, PLOG_INFO, PLOG_ERROR, PLOG_FATAL这四个宏输出即可，如：
PLOG_DEBUG("this is %s log, level is: %d", "debug", PLOG_LEVEL_DEBUG);
PLOG_INFO("this is %s log, level is: %d", "info", PLOG_LEVEL_INFO);
PLOG_WARN("this is %s log, level is: %d", "warn", PLOG_LEVEL_WARN);
PLOG_ERROR("this is %s log, level is: %d", "error", PLOG_LEVEL_ERROR);
注意：不要直接调用此头文件中定义的PLogDebug, PLogInfo等函数）

============= 查看方法 ==============
日志按日期记录到对应的进程所在的日志文件中，日志的文件名为获取到的进程名，
如果没获取到进程名，则被记录到PATROLLOGGER这个进程名对应的日志文件中。
例如，一个进程名为ProcessName的进程，在日志记录根目录中的存储位置为：
2022-09-12
     └─ ProcessName.log
2022-09-13
     └─ ProcessName.log
2022-09-14
     └─ ProcessName.log
使用普通的文本编辑器（gedit, vi等）查看这些日志文件即可

============== 附加功能 ===============
使用上述日志记录方法，单条日志的最大字节数不能大于8K，为了能记录大于8K的数据（因开发过程中涉及多媒体调试，调试数据很可能大于8K），
使用PLOG_RAWDATA_AS_RAW(const char *strFileName, const char *strSuffix, const char *data, int len)来将原始数据记录为原始文件，
使用PLOG_RAWDATA_AS_HEX(const char *strFileName, const char *strSuffix, const char *data, int len)来将原始数据记录为HEX文件，
记录的原始数据在日志记录根目录中的位置为（以strFileName为FileName, strSufix为.txt举例）：
2022-09-12
     └─ RawData
             └─ FileName.172005.002385.txt
             └─ FileName.172005.005385.txt
             └─ FileName.172008.003385.txt
此功能默认关闭，需要在日志配置文件中添加如下配置项开启：
#支持的配置：0（禁用记录原始数据的功能）， 1（启用记录原始数据的功能）
PLOG_ENABLE_RAWDATA=1

=============== 使用例程 ===============
#include "libCPatrolLog/PatrolLogger.h"

int main(int argc, char **argv)
{
    PLOG_DEBUG("Log debug");
    PLOG_DEBUG("Log type: %s", "debug");

    PLOG_INFO("Log info");
    PLOG_INFO("Log type: %s", "info");

    PLOG_WARN("Log warn");
    PLOG_WARN("Log type: %s", "warn");

    PLOG_ERROR("Log error");
    PLOG_ERROR("Log type: %s", "error");

    char data[PLOG_MSG_MAXLEN+1024] = {0};
    memset(data, '1', sizeof(data));

    PLOG_RAWDATA_AS_RAW("test", ".txt", data, sizeof(data));
    PLOG_RAWDATA_AS_HEX("test", ".hex", data, sizeof(data));

    return 0;
}
*/

#if defined(_WIN32)
    #if defined(LIBCPATROLLOG_EXPORTS)
        #define LIBCPATROLLOG_API __declspec(dllexport)
    #else
        #define LIBCPATROLLOG_API __declspec(dllimport)
    #endif
#endif

#if !defined(LIBCPATROLLOG_API)
    #if !defined(CYGSYS_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
        #define LIBCPATROLLOG_API __attribute__ ((visibility ("default")))
    #else
        #define LIBCPATROLLOG_API
    #endif
#endif

//日志信息的最大长度（不包括日志时间、日志类型和日志位置信息）
//超过这个长度的日志信息将会被截断
#define PLOG_MSG_MAXLEN    8192

enum PLogLevel:unsigned char
{
    PLOG_LEVEL_DEBUG = 1,   //调试信息
    PLOG_LEVEL_INFO = 2,      //告知信息
    PLOG_LEVEL_WARN = 3,    //警告信息
    PLOG_LEVEL_ERROR = 4,   //错误信息
    PLOG_LEVEL_NOLOG = 5   //不记录日志
} ;

LIBCPATROLLOG_API PLogLevel CurPLogLevel();

//请不要直接调用下面的函数，应该使用函数后面定义的宏来记录日志
LIBCPATROLLOG_API void PLogDebug(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...);
LIBCPATROLLOG_API void PLogInfo(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...);
LIBCPATROLLOG_API void PLogWarn(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...);
LIBCPATROLLOG_API void PLogError(const char * strSourcePath, const char * strFuncName, int nLineNumber, const char * strFmt, ...);


#define PLOG_DEBUG(PLogMsgFormat, ...) PLogDebug(__FILE__, __FUNCTION__, __LINE__, PLogMsgFormat, ##__VA_ARGS__)
#define PLOG_INFO(PLogMsgFormat, ...) PLogInfo(__FILE__, __FUNCTION__, __LINE__, PLogMsgFormat, ##__VA_ARGS__)
#define PLOG_WARN(PLogMsgFormat, ...) PLogWarn(__FILE__, __FUNCTION__, __LINE__, PLogMsgFormat, ##__VA_ARGS__)
#define PLOG_ERROR(PLogMsgFormat, ...) PLogError(__FILE__, __FUNCTION__, __LINE__, PLogMsgFormat, ##__VA_ARGS__)

LIBCPATROLLOG_API void PLogRawData(const char * strFileName, const char * strSuffix, const char * data, int len, bool bLogAsHex);
#define PLOG_RAWDATA_AS_RAW(FileName, Suffix, Data, Len)   PLogRawData(FileName, Suffix, Data, Len, false);
#define PLOG_RAWDATA_AS_HEX(FileName, Suffix, Data, Len)  PLogRawData(FileName, Suffix, Data, Len, true);

#endif //PATROLLOGGER_H
