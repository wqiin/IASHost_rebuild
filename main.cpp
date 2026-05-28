#include "httplib.h"
#include "sys_config.h"
#include "http_service_impl.hpp"
#include "macro_define.h"

#include "PatrolLogger.h"

#include <thread>

int main(int argc, char *argv[])
{
    PLOG_ERROR("Remoteppath is NULL or too long(path::)\n");

    //算法更新线程
    std::jthread th_algo_upt([](){
        httplib::Server algo_upt;
        algo_upt.Post(HTTP_ALGO_UPT_URL, HTTP_SERVICE_IMPL::algo_update);
        algo_upt.listen("0.0.0.0", 12345);
    });

    httplib::Server algo_server;//算法请求服务server
    algo_server.Post(HTTP_REQUEST_URL, HTTP_SERVICE_IMPL::algo_analyse);
    algo_server.listen("0.0.0.0", 1234);//会阻塞主线程

    return 0;
}
