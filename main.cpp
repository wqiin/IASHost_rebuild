#include "httplib.h"
#include "sys_config.h"
#include "http_service_impl.hpp"
#include "macro_define.h"

int main(int argc, char *argv[])
{
    httplib::Server algo_server;//算法请求服务server
    algo_server.Post(HTTP_REQUEST_URL, HTTP_SERVICE_IMPL::algo_analyse);
    algo_server.listen("0.0.0.0", 1234);//会阻塞主线程

    return 0;
}
