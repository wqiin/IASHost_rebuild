#ifndef _FTPS_H
#define _FTPS_H

#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <string>

#if defined(_WIN32)
    #if defined(LIBCFTPSFILECLIENT_EXPORTS)
        #define LIBCFTPSFILECLIENT_API __declspec(dllexport)
    #else
        #define LIBCFTPSFILECLIENT_API __declspec(dllimport)
    #endif
#endif

#if !defined(LIBCFTPSFILECLIENT_API)
    #if !defined(CYGSYS_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
        #define LIBCFTPSFILECLIENT_API __attribute__ ((visibility ("default")))
    #else
        #define LIBCFTPSFILECLIENT_API
    #endif
#endif

typedef struct FTPS_hostinfo//没有判断构造函数中传入参数的长度是否超过下列字符串容量
{
    char remoteIp[51] = {0};//保存远端ip
    char remotePort[31] = {0};//保存远端端口
    char username[31] = {0};//保存ftp登录用户名
    char password[31] = {0};//保存ftp登录用户密码
} HostInfo;

//FTPS的加密模式
enum ENUM_ENCRYPTE_MODE
{
    IMPLICIT_ENCRYPTED = 1,//隐示加密
    EXPLICIT_ENCTYPTED,//显示加密
};

struct WriteThis {
  const char *readptr;
  size_t sizeleft;
};

//注意：：
//本客户端实例化的对象只能在单独的线程中使用
//如果需要多线程使用，请为每个线程实例化一个此类的对象
//错误信息输出到日志
class CFtpsClient{
public:
    //remoteIp-远端Ip， remotePort-远端端口， username-ftp登录的用户名, password-ftp登录的用户密码, encrypeMode-ftps的加密模式（隐式->1或显式->2）默认为隐示加密
    CFtpsClient(const char * remoteIp, const char * remotePort, const char * username, const char * password, const char encryptMode = IMPLICIT_ENCRYPTED);
    ~CFtpsClient();

public:
    //对外接口，传入的路径都必须为绝对路径，且传入的路径或 重命名的新旧文件名长度不能超过1024个字节，否则直接返回错误
    bool ftps_upFile(const std::string & strPicData, const char * remotepath); //文件上传，strPicData为需要上传到远端的二进制数据，remotepath远端保存的路径
    bool ftps_upFile(const char * localpath, const char * remotepath); //文件上传，localpath为需要上传到远端的本地文件（绝对路径），remotepath远端保存的路径
    bool ftps_downFile(std::string & strData, const char * remotepath); //文件下载，strData存放从远端下载过来的图片二进制数据，remotepath为需要下载的文件路径（绝对路径）
    bool ftps_downFile(const char * localpath, const char * remotepath); //文件下载，localpath为本地保存的路径（绝对路径），remotepath为需要下载的文件路径（绝对路径）
    bool ftps_delFile(const char * remotepath);//删除远端的图片(绝对路径)
    bool isRunning();

private:
    char m_chEncryptMode;//加密模式(显示或隐示)
    HostInfo hostinfo;//保存ftp连接和登录的参数
    bool m_bisRunning;
    FILE * m_pUploadFile;
    FILE * m_pDownloadFile;

private://类中使用函数
    size_t onUploadReadData(void * ptr, size_t size, size_t nmemb);
    size_t onDownloadWriteData(void * ptr, size_t size, size_t nmemb);

    static size_t UploadCallBack(void * ptr, size_t size, size_t nmemb, void * stream);//上传回调
    static size_t uploadReadCallback(void * ptr, size_t size, size_t nmemb, void * stream); //回调函数
    static size_t downloadWriteCallback(void * ptr, size_t size, size_t nmemb, void * stream);
    static size_t downCallBack(void * ptr, size_t size, size_t nmemb, void * stream);//下载写文件回调
    void CreateDir(const char * path);
};

#endif
