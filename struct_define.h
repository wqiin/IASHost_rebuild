#ifndef STRUCT_DEFINE_H
#define STRUCT_DEFINE_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "nlohmann/single_include/nlohmann/json.hpp"
using NJson = nlohmann::json;

struct stFTPSConfig{
    std::string ftps_ip;                //*ftps的主机地址
    std::uint16_t ftps_port;            //*ftps服务的端口
    std::string ftps_username;          //ftps帐号
    std::string ftps_password;          //ftps密码

    std::string ftps_local_path;        //ftps本地下载存放文件的路径
    std::string ftps_remote_path;       //ftps上传文件存放路径
    std::string ftps_base_image_path;   //ftps基准图片本地存放路径
};

struct stAlgoConfig{
    std::string algo_host_ip = "127.0.0.1";         //算法主机ip(该程序默认跟算法主机在同一主机上，用于调试)
    bool is_multi_process = false;                  //是否启用多进程模式，为true时为启用，否则为不启用
    std::string controling_file_path;               //控制文件路径，只能填写在当前用户拥有文件生成权限的路径
    std::string algo_upt_path;                      //算法更新本地文件保存路径
    std::string local_listening_ip = "0.0.0.0";     //本机的监听ip
    std::uint8_t alog_process_count = 1;            //工作进程的数量

    std::uint16_t algo_listening_port;              //http异常回馈监听端口
    std::uint16_t mode_listening_port;              //更新模型监听端口
};

template<typename T>
struct stPoint
{
    T x = 0;
    T y = 0;
};


struct stAlgoInParas{
    std::string algo_ret_js_data;           //算法识别后，返回的JSON数据
    std::string algo_base_iamge;            //算法基准图
    std::string image_from_patrol_host;     //存放从巡视主机下载回来的图片
    std::string image_coordinations;        //存在配置的图片坐标字符串，为一个JSON字符串
    std::uint16_t port;                     //算法调用的端口号
};


using Points_t = NJson;//std::vector<stPoint<int>>;
using AlgoHandler_t = bool(*)(stAlgoInParas & );
struct stSysConfig{
    using Port_t = std::uint16_t;
    std::unordered_map<std::string, Port_t> algo_2_port;    //key为算法名称，value为该算法对应的调用端口号
    std::unordered_set<std::string> allowed_ips;            //允许链接到本机的IP地址
    std::unordered_map<std::string, Points_t> algo_2_rectangle;  //key为算法名称，value为其对应的矩形坐标

    std::unordered_map<std::string, AlgoHandler_t> algo_2_functionality;//key为算法名称，value为其对应的函数指针
};


struct stObjectList{
    std::string object_id;
    std::vector<std::string> type_list;
    std::string image_url_path;
    std::vector<std::string> img_url_lists;
};

struct stPatrolParams
{
    std::string ip;
    std::uint16_t port;
    std::string request_id;
    std::vector<stObjectList> obj_list;
};




#endif // STRUCT_DEFINE_H
