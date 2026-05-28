#ifndef HTTP_SERVICE_IMPL_HPP
#define HTTP_SERVICE_IMPL_HPP


#include "httplib.h"
#include "macro_define.h"
#include "nlohmann/single_include/nlohmann/json.hpp"
#include "struct_define.h"
#include "qthread.hpp"
#include "FtpsClient.h"
#include "sys_config.h"
#include "PatrolLogger.h"


#include <functional>
#include <filesystem>
namespace fs = std::filesystem;

using NJson = nlohmann::json;


namespace HTTP_SERVICE_IMPL
{
    //算法分析
    static std::function<void(const httplib::Request &, httplib::Response &)> algo_analyse = [](const httplib::Request & req, httplib::Response & resp){
        resp.status = 200;

        auto length = req.get_header_value("Content-Length", "0");
        auto body_size  = std::stoll(length);
        if(0 == body_size || HTTP_REQUEST_BODY_MAX_LENGTH > body_size){//非法报文
            resp.set_content(INVALID_HTTP_PACKAGE, PACK_TYPE);
            return;
        }

        //报文类型
        auto pack_type = req.get_header_value("Content-Type");
        if(PACK_TYPE != pack_type){
            resp.set_content(INVALID_HTTP_PACKAGE, PACK_TYPE);
            return ;
        }

        //解析http的body部分
        stPatrolParams patrol_params;
        auto js_body = NJson::parse(req.body);
        patrol_params.ip = js_body["requestHostIp"];
        patrol_params.port = js_body["requestHostPort"];
        patrol_params.request_id = js_body["requestId"];

        //预分配空间
        patrol_params.obj_list.reserve(js_body["objectList"].size());
        int index = 0;
        for(auto & obj : js_body["objectList"]){
            auto & obj_item = patrol_params.obj_list.at(index);

            obj_item.object_id = obj["objectId"];
            obj_item.image_url_path = obj["imageUrlList"];

            for(auto & type : obj["typeList"]){
                obj_item.type_list.emplace_back(type);
            }

            for(auto & url : obj["imageUrlList"]){
                obj_item.img_url_lists.emplace_back(url);
            }

            index += 1;
        }

        //处理body中的请求
        qthread::ThreadPool pool(4);
        std::vector<std::future<NJson>> futures;
        for(auto & item : patrol_params.obj_list){
            auto futures_ret = pool.submit([&item, &patrol_params]()->NJson{
                NJson js_ret;
                js_ret["objectId"] = item.object_id;
                js_ret["results"] = NJson::array();

                CFtpsClient ftpsClt("ip", "port", "username", "password");
                for(const auto & img_url : item.img_url_lists){
                    std::string pic_bin_data;

                    if(!ftpsClt.ftps_downFile(pic_bin_data, img_url.c_str())){
                        js_ret["results"].push_back(ALGO_PIC_ERROR);
                        continue;
                    }

                    //判别基准图字段不为空时，去取基准图
                    std::string base_image;
                    if(!item.image_url_path.empty() && !ftpsClt.ftps_downFile(base_image, item.image_url_path.c_str())){
                        js_ret["results"].push_back(ALGO_PIC_ERROR);
                        continue;
                    }

                    //取出图片后，根据Type类型，选择调用函数和端口
                    for(const auto & algo_type : item.type_list){
                        if(SYS_CFG_INST.is_configured_rectangle_params(algo_type)){//配置了矩形框的算法
                            auto points = SYS_CFG_INST.get_algo_points(algo_type);
                            if(points.empty()){
                                js_ret["results"].push_back(ALGO_ANALY_FAILED);
                                continue;
                            }
                        }

                        //检查是否该算法配置了相应的调用端口
                        if(!SYS_CFG_INST.is_algo_configured_port(algo_type)){
                            js_ret["results"].push_back(ALGO_ANALY_FAILED);
                            continue;
                        }

                        //得到算法类型对应的函数指针
                        auto algo_func = SYS_CFG_INST.get_algo_functionality(algo_type);
                        if(!algo_func){
                            js_ret["results"].push_back(ALGO_ANALY_FAILED);
                            continue;
                        }

                        //调用算法
                        stAlgoInParas algo_params;
                        algo_params.algo_base_iamge = std::move(base_image);
                        algo_params.image_from_patrol_host = std::move(pic_bin_data);
                        algo_params.port = SYS_CFG_INST.get_algo_configured_port(algo_type);
                        algo_params.image_coordinations = SYS_CFG_INST.is_configured_rectangle_params(algo_type);
                        if(!(*algo_func)(algo_params)){
                            js_ret["results"].push_back(ALGO_ANALY_FAILED);
                            continue;
                        }

                        //解析算法调用结束后的JSON数据，做进一步处理
                        auto &&  algo_ret_js = NJson::parse(algo_params.algo_ret_js_data, nullptr, false);
                        if(algo_ret_js.is_discarded()){
                            js_ret["results"].push_back(ALGO_ANALY_FAILED);
                            continue;
                        }

                        auto code_value = std::stoi(algo_ret_js["value"].dump());;//存放识别后的value值，0--无缺陷，1--有缺陷//还需要验证code这个字段，
                        if(ALGO_RECG_VALUE_WITH_DEFECT == code_value){//识别后的结果有缺陷，需要在基准图上绘制识别区域的多边形(如果有的话)
                            auto code = std::stoi(algo_ret_js["code"].dump());//获取识别后图片的code值
                            if(ALGO_RECG_FAILURE == code){
                                js_ret["results"].push_back(ALGO_ANALY_FAILED);
                                continue;
                            }

                            auto pos = algo_ret_js["pos"];//获取pos的value
                            auto algo_ret_type = algo_ret_js["type"];//获取算法的识别类型

                            //在基准图中绘制举行

                            js_ret["resImageUrl"] = "";
                            js_ret["pos"] = "";
                        }

                        js_ret["results"].push_back(algo_ret_js);
                    }

                }

                return js_ret;
            });

            futures.emplace_back(std::move(futures_ret));
        }

        //等待所有任务执行完成
        NJson response_js;
        response_js["requestId"] = patrol_params.request_id;
        response_js["resultsList"] = NJson::array();

        for(auto & future : futures){
            if(future.valid()){
                response_js["resultsList"].push_back(future.get());
            }
        }

        resp.set_content(response_js.dump(4), PACK_TYPE);
    };

    //算法更新
    static std::function<void(const httplib::Request &, httplib::Response &)> algo_update = [](const httplib::Request & req, httplib::Response & resp){
        resp.status = 200;

        auto length = req.get_header_value("Content-Length", "0");
        auto body_size  = std::stoll(length);
        if(0 == body_size || HTTP_REQUEST_BODY_MAX_LENGTH > body_size){//非法报文
            resp.set_content(INVALID_HTTP_PACKAGE, PACK_TYPE);
            return;
        }

        //报文类型
        auto pack_type = req.get_header_value("Content-Type");
        if(PACK_TYPE != pack_type){
            resp.set_content(INVALID_HTTP_PACKAGE, PACK_TYPE);
            return ;
        }

        stAlgoUptParams upt_params;
        auto js_body = NJson::parse(req.body);
        if(js_body.empty()){
            resp.set_content(INVALID_HTTP_PACKAGE, PACK_TYPE);
            return ;
        }

        upt_params.ip = js_body["requestHostIp"];
        upt_params.port = js_body["requestHostPort"];
        upt_params.request_id = js_body["requestId"];
        upt_params.algo_path = js_body["algorithmPath"];

        //异步处理返回结果的请求 - 显示制定忽略返回值
        std::ignore = std::async(std::launch::async, [upt_params]()->void{
            CFtpsClient ftpsClt("ip", "port", "username", "password");
            auto local_algo_upt_path = HTTP_CFG.algo_upt_path + fs::path(upt_params.algo_path).filename().string();
            auto file_opt_ret = ftpsClt.ftps_downFile(local_algo_upt_path.c_str(), upt_params.algo_path.c_str());

            NJson ret_js;
            ret_js["requestId"] = upt_params.request_id;
            ret_js["result"] = std::to_string(file_opt_ret);

            httplib::Client cli(upt_params.ip, upt_params.port);
            cli.set_connection_timeout(5);    // 可选
            cli.set_read_timeout(10);

            auto ret = cli.Post(HTTP_ALGO_UPT_RESULT_URL, ret_js.dump(4), PACK_TYPE);
            if(!ret){
                PLOG_ERROR("Some errors occured when sent algorithm updated result to target host.");
            }
        });

    };
}

#endif // HTTP_SERVICE_IMPL_HPP
