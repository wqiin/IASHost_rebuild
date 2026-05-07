#include "sys_config.h"
#include <filesystem>
#include <fstream>

#include "nlohmann/single_include/nlohmann/json.hpp"

namespace fs = std::filesystem;
using NJson = nlohmann::json;

void SysConfig::read_all_config()
{
    read_ftps_config();
    read_configured_points();
    read_system_config();

    read_allowed_ips();
    read_port_mapping();
}

void SysConfig::read_allowed_ips()
{
    std::ifstream ifs("../config/allowed_ips.js");
    if(!ifs.is_open()){
        return ;
    }

    NJson js;
    try{
        ifs >> js;
    }catch(const std::exception & e){
        return ;
    }

    for(auto & ip : js["allowed_ips"]){
        algo_params_config.allowed_ips.insert(ip);
    }
}

void SysConfig::read_port_mapping()
{
    std::ifstream ifs("../config/algo_port_mapping.js");
    if(!ifs.is_open()){
        return ;
    }

    NJson js;
    try{
        ifs >> js;
    }catch(const std::exception & e){
        return ;
    }

    for(auto & [k, v] : js.items()){
        if(k.find("comment") != std::string::npos){
            continue;
        }

        algo_params_config.algo_2_port.emplace(k, v);
    }
}

void SysConfig::read_ftps_config()
{
    std::ifstream ifs("../config/ftps_config.js");
    if(!ifs.is_open()){
        return ;
    }

    NJson js;
    try{
        ifs >> js;
    }catch(const std::exception & e){
        return ;
    }

    ftps_config.ftps_ip = js["ftps_ip"];
    ftps_config.ftps_port = js["ftps_port"];
    ftps_config.ftps_username = js["ftps_username"];
    ftps_config.ftps_password = js["ftps_password"];

    ftps_config.ftps_local_path = "../image_from_remote_host";
    ftps_config.ftps_remote_path = "/fater_recg";
    ftps_config.ftps_base_image_path = "../base_image_from_remote_host";
}

void SysConfig::read_configured_points()
{
    for(const auto & file : fs::directory_iterator("../condig/coordination")){
        if(!file.is_regular_file()){
            continue;
        }

        std::ifstream ifs(file.path());
        if(!ifs.is_open()){
            return ;
        }

        NJson js;
        try{
            ifs >> js;
        }catch(const std::exception & e){
            continue ;
        }


        auto && file_name_without_suffix = file.path().stem().string();
        algo_params_config.algo_2_rectangle[file_name_without_suffix].push_back(js);
    }
}

void SysConfig::read_system_config()
{
    std::ifstream ifs("../config/system_config.js");
    if(!ifs.is_open()){
        return ;
    }

    NJson js;
    try{
        ifs >> js;
    }catch(const std::exception & e){
        return ;
    }

    http_config.mode_listening_port = js["model_updating_port"];
    http_config.local_listening_ip = js["local_listening_ip"];
    http_config.algo_listening_port = js["http_listening_port"];
    http_config.algo_host_ip = js["algo_host_ip"];

    http_config.algo_upt_path = js["algo_upt_file_path"];
    http_config.controling_file_path = js["single_daemon_controling_file_path"];
    http_config.alog_process_count = js["working_process_num"];
    http_config.is_multi_process = js["multi_working_process"];
}

