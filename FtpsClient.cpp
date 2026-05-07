#include "FtpsClient.h"
#include "PatrolLogger.h"
#include <string>
#include<sys/stat.h>
#include<libgen.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

CFtpsClient::CFtpsClient(const char * remoteIp, const char * remotePort, const char * username, const char * password, const char encryptMode)
{
    m_chEncryptMode = encryptMode;
    m_pUploadFile = NULL;
    m_pDownloadFile = NULL;
    m_bisRunning = false;
    memset(&hostinfo, 0, sizeof(hostinfo));
    strncpy(hostinfo.remoteIp, remoteIp, sizeof(hostinfo.remoteIp) - 1);
    strncpy(hostinfo.remotePort, remotePort, sizeof(hostinfo.remotePort) - 1);
    strncpy(hostinfo.username, username, sizeof(hostinfo.username) - 1);
    strncpy(hostinfo.password, password, sizeof(hostinfo.password) - 1);
    curl_global_init(CURL_GLOBAL_DEFAULT);//初始化curl
}

bool CFtpsClient::ftps_upFile(const std::string & strPicData, const char * remotepath)
{
    if (remotepath == NULL || strlen(remotepath) >= 1024)
    {
        PLOG_ERROR("Remoteppath is NULL or too long(path::%s)\n", remotepath);
        return false;
    }

    if(strPicData.empty())
    {
        PLOG_ERROR("Pic to Send is empty");
        return false;
    }

    CURL * curl = NULL;
    CURLcode res;

    char urlkey[600] = {0};
    snprintf(urlkey, 599, "%s:%s", hostinfo.username, hostinfo.password);

    char hosturl[2000] = {0};
    if (m_chEncryptMode == 1)//隐示加密（默认）
    {
        snprintf(hosturl, 1999, "ftps://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }
    else if (m_chEncryptMode == 2)//显示加密
    {
        snprintf(hosturl, 1999, "ftp://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }

    curl = curl_easy_init();

    struct WriteThis upload;
    upload.readptr = strPicData.c_str();
    upload.sizeleft = strPicData.size();

    if (curl)
    {
        m_bisRunning = true;
        curl_easy_setopt(curl, CURLOPT_URL, hosturl);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, CFtpsClient::UploadCallBack);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload);
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, upload.sizeleft);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        m_bisRunning = false;
    }
    else
    {
        m_bisRunning = false;
        PLOG_ERROR("CURL is NULL\n");
        return false;
    }

    if (res == CURLE_OK)
    {
        PLOG_INFO("Upload A Mem-File As Remote File:%s Successed", remotepath);
        return true;
    }
    else
    {
        PLOG_ERROR("Upload A Mem-File As Remote File:%s Failed(res=%d-->%s)", remotepath, res, curl_easy_strerror(res));
        return false;
    }
    return true;
}

bool CFtpsClient::ftps_upFile(const char * localpath, const char * remotepath)
{
    if (localpath == NULL || remotepath == NULL)
    {
        PLOG_ERROR("Localpath or Remoteppath is NULL\n");
        return false;
    }

    if (strlen(remotepath) >= 1024 || strlen(localpath) >= 1024)
    {
        PLOG_ERROR("Too long directory\n");
        return false;
    }

    CURL * curl = NULL;
    CURLcode res;
    //const char* urlkey = "uk:Sys@7950"; //服务器用户名密码
    char urlkey[600] = {0};
    snprintf(urlkey, 599, "%s:%s", hostinfo.username, hostinfo.password);
    m_pUploadFile = fopen(localpath, "rb");

    if (m_pUploadFile == NULL)
    {
        PLOG_ERROR("Fopen error\n");
        return false;
    }

    fseek(m_pUploadFile, 0L, SEEK_END);
    size_t iFileSize = ftell(m_pUploadFile);//得到文件大小
    fseek(m_pUploadFile, 0L, SEEK_SET);
    char hosturl[2000] = {0};

    if (m_chEncryptMode == 1)//隐示加密（默认）
    {
        snprintf(hosturl, 1999, "ftps://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }
    else if (m_chEncryptMode == 2)//显示加密
    {
        snprintf(hosturl, 1999, "ftp://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }

    curl = curl_easy_init();
    if (curl)
    {
        m_bisRunning = true;
        curl_easy_setopt(curl, CURLOPT_URL, hosturl); //使用curl指定使用ftps
        //curl_easy_setopt(curl, CURLOPT_URL, "ftps://10.6.220.65:10012/kk/upload.tar.gz");//使用curl指定使用ftps
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey); //指定用户名和密码
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, CFtpsClient::uploadReadCallback);//回调，显示文件传输进度，可注释
        curl_easy_setopt(curl, CURLOPT_READDATA, this);//指定上传文件的大小
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, iFileSize);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);//
	
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        fclose(m_pUploadFile);
        m_pUploadFile = NULL;
    }
    else
    {
        m_bisRunning = false;
        fclose(m_pUploadFile);
        m_pUploadFile = NULL;
        PLOG_ERROR("CURL is NULL\n");
        return false;
    }
    m_bisRunning = false;

    if (res == 0 || res == CURLE_PARTIAL_FILE)//CURLE_PARTIAL_FILE为server_side error, thus ignoring this error
    {
        PLOG_INFO("Upload Local File:%s As Remote File:%s Successed", localpath, remotepath);
        return true;
    }
    else
    {
        PLOG_ERROR("Upload Local File:%s As Remote File:%s Failed\n(ret_value::%d)", localpath, remotepath, res);
        return false;
    }
}

//将远端的数据读到参数1中
bool CFtpsClient::ftps_downFile(std::string & strData, const char * remotepath)
{
    if ( remotepath == NULL)
    {
        PLOG_ERROR("Localpath or Remoteppath is NULL\n");
        return false;
    }

    if (strlen(remotepath) >= 1024)
    {
        PLOG_ERROR("Too long directory\n");
        return false;
    }

    CURL * curl = NULL;
    CURLcode res;
    //const char* urlkey = "uk:Sys@7950"; //服务器用户名密码
    char urlkey[340] = {0};
    snprintf(urlkey, 339, "%s:%s", hostinfo.username, hostinfo.password);
    char hosturl[2000] = {0};

    if (m_chEncryptMode == 1)//隐示加密（默认）
    {
        snprintf(hosturl, 1999, "ftps://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }
    else if (m_chEncryptMode == 2)//显示加密
    {
        snprintf(hosturl, 1999, "ftp://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }

    curl = curl_easy_init();
    if (curl)
    {
        m_bisRunning = true;
        //curl_easy_setopt(curl, CURLOPT_URL, "ftps://10.6.220.65:10012/kk/download.zip");
        curl_easy_setopt(curl, CURLOPT_URL, hosturl);
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CFtpsClient::downloadWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strData);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
        res = curl_easy_perform(curl);//执行
        
        curl_easy_cleanup(curl);
    }
    else
    {
        m_bisRunning = false;
        PLOG_ERROR("CURL is NULL\n");
        return false;
    }

    m_bisRunning = false;

    if (res == 0 || res == CURLE_PARTIAL_FILE)//CURLE_PARTIAL_FILE为server_side error, thus ignoring this error
    {
        PLOG_INFO("Download Remote File:%s Successed", remotepath);
        return true;
    }
    else
    {
        PLOG_ERROR("Download Remote File:%s Failed(res = %d)", remotepath, res);
        return false;
    }
}

bool CFtpsClient::ftps_downFile(const char * localpath, const char * remotepath)
{
    if (localpath == NULL || remotepath == NULL)
    {
        PLOG_ERROR("Localpath or Remoteppath is NULL\n");
        return false;
    }

    if (strlen(remotepath) >= 1024 || strlen(localpath) >= 1024)
    {
        PLOG_ERROR("Too long directory\n");
        return false;
    }

    CURL * curl = NULL;
    CURLcode res;
    //const char* urlkey = "uk:Sys@7950"; //服务器用户名密码
    char urlkey[340] = {0};
    snprintf(urlkey, 339, "%s:%s", hostinfo.username, hostinfo.password);
    
    //加上本地路径是否存在判断，如果不存在本地路径，则创建
    CreateDir(localpath);

    m_pDownloadFile = fopen(localpath, "wb+");

    if (NULL == m_pDownloadFile)
    {
        curl_easy_cleanup(curl);
        PLOG_ERROR("Fopen failed\n");
        return false;
    }

    char hosturl[2000] = {0};

    if (m_chEncryptMode == 1)//隐示加密（默认）
    {
        snprintf(hosturl, 1999, "ftps://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }
    else if (m_chEncryptMode == 2)//显示加密
    {
        snprintf(hosturl, 1999, "ftp://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }

    curl = curl_easy_init();

    if (curl)
    {
        m_bisRunning = true;
        //curl_easy_setopt(curl, CURLOPT_URL, "ftps://10.6.220.65:10012/kk/download.zip");
        curl_easy_setopt(curl, CURLOPT_URL, hosturl);
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CFtpsClient::downCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 256000L);
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);

        res = curl_easy_perform(curl);//执行 
        
        curl_easy_cleanup(curl);
        fclose(m_pDownloadFile);
        m_pDownloadFile = NULL;
    }
    else
    {
        m_bisRunning = false;
        fclose(m_pDownloadFile);
        m_pDownloadFile = NULL;
        PLOG_ERROR("CURL is NULL\n");
        return false;
    }

    m_bisRunning = false;

    if (res == 0)
    {
        PLOG_INFO("Download Remote File:%s As Local File:%s Successed", remotepath, localpath);
        return true;
    }
    else
    {
        PLOG_ERROR("Download Remote File:%s As Local File:%s Failed(res=%d)", remotepath, localpath,res);
        return false;
    }
}

bool CFtpsClient::ftps_delFile(const char * remotepath)//删除远端给定路径下的文件
{
    if (strlen(remotepath) >= 1024 )
    {
        PLOG_ERROR("Too long remotepath\n");
        return false;
    }

    CURL * curl = NULL;
    CURLcode res ;
    char urlkey[340] = {0}; //服务器用户名密码
    snprintf(urlkey, 339, "%s:%s", hostinfo.username, hostinfo.password);
    curl = curl_easy_init();
    char hosturl[2000] = {0};

    if (m_chEncryptMode == 1)
    {
        snprintf(hosturl, 1999, "ftps://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }
    else if (m_chEncryptMode == 2)
    {
        snprintf(hosturl, 1999, "ftp://%s:%s%s", hostinfo.remoteIp, hostinfo.remotePort, remotepath);
    }

    if (curl)
    {
        m_bisRunning = true;
        curl_easy_setopt(curl, CURLOPT_URL, hosturl);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        
        struct curl_slist * ftpCmdList = NULL;
        char cmd[1024+5] = {0};
        snprintf(cmd, 1024 + 4, "DELE %s", remotepath);//指定需要删除的文件
        ftpCmdList = curl_slist_append(ftpCmdList, cmd);
        
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, ftpCmdList);
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
        
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    else
    {
        PLOG_ERROR("CURL is NULL\n");
        return false;
    }

    m_bisRunning = false;

    if (res == 0)
    {
        PLOG_INFO("Delete the File:%s Successed\n", remotepath);
        return true;
    }
    else
    {
        PLOG_ERROR("Delete the File:%s Failed(res = %d)\n", remotepath, res);
        return false;
    }
}

void CFtpsClient::CreateDir(const char * path)
{
  char Path[1024] = {0};
  
  for(int ii = 0; ii < strlen(path); ii++)
  {
    Path[ii] = path[ii];
    if(Path[ii] == '/')
    {
      if(access(Path, F_OK) == 0) continue;
      else
      {
    if(mkdir(Path, 0) == -1)	PLOG_ERROR("mkdir Error");
	chmod(Path, 0777);
      }
    }
  }
}

bool CFtpsClient::isRunning()
{
    return m_bisRunning;
}

size_t CFtpsClient::onUploadReadData(void * ptr, size_t size, size_t nmemb)
{
    curl_off_t nread;
    size_t retcode = fread(ptr, size, nmemb, m_pUploadFile);
    //nread = (curl_off_t)retcode;
    return retcode;
}

CFtpsClient::~CFtpsClient()
{
    curl_global_cleanup();
}

size_t CFtpsClient::onDownloadWriteData(void * ptr, size_t size, size_t nmemb)
{
    int written = fwrite(ptr, size, nmemb, m_pDownloadFile);
    return written;
}

size_t CFtpsClient::uploadReadCallback(void * ptr, size_t size, size_t nmemb, void * stream) //回调函数
{
    CFtpsClient * ftps = static_cast<CFtpsClient *>(stream);
    return ftps->onUploadReadData(ptr, size, nmemb);
}

size_t CFtpsClient::UploadCallBack(void * ptr, size_t size, size_t nmemb, void * stream)//上传回调
{
    struct WriteThis *upload = (struct WriteThis *)stream;
    size_t max = size * nmemb;
    if(max < 1) return 0;

    if(upload->sizeleft)
    {
        size_t copylen = max;
        if(copylen > upload->sizeleft)  copylen = upload->sizeleft;
        memcpy(ptr, upload->readptr, copylen);
        upload->readptr += copylen;
        upload->sizeleft -= copylen;
        return copylen;
  }

  return 0;
}

size_t CFtpsClient::downloadWriteCallback(void * ptr, size_t size, size_t nmemb, void * stream)
{
    std::string * pTemp = (std::string *)stream;
    *pTemp += std::string((char *)ptr, nmemb * size);
    return nmemb * size;
}

size_t CFtpsClient::downCallBack(void * ptr, size_t size, size_t nmemb, void * stream)//下载写文件回调
{
  CFtpsClient * ftps = static_cast<CFtpsClient *>(stream);
  return ftps->onDownloadWriteData(ptr, size, nmemb);
}
