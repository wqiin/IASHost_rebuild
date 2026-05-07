#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "struct_define.h"
#include "algo_impl.h"
#include "macro_define.h"

class SysConfig
{
private:
    SysConfig(){
        auto & mp = algo_params_config.algo_2_functionality;
        mp.insert(MAKE_ALGO_PARI(wcaqm));
        mp.insert(MAKE_ALGO_PARI(hzyw));
        mp.insert(MAKE_ALGO_PARI(wtyl));
        mp.insert(MAKE_ALGO_PARI(wtyc));
        mp.insert(MAKE_ALGO_PARI(ywrq));
        mp.insert(MAKE_ALGO_PARI(ryjjph));
        mp.insert(MAKE_ALGO_PARI(rydd));
        mp.insert(MAKE_ALGO_PARI(wcgz));
        mp.insert(MAKE_ALGO_PARI(ryrq));
        mp.insert(MAKE_ALGO_PARI(yxcr));
        mp.insert(MAKE_ALGO_PARI(claqgk));
        mp.insert(MAKE_ALGO_PARI(yw_nc));
        mp.insert(MAKE_ALGO_PARI(xdwcr));

        //读取所有配置
        read_all_config();
    }   //不允许外部构造

protected:
    void read_all_config();

    void read_ftps_config();
    void read_configured_points();
    void read_system_config();
    void read_allowed_ips();

    void read_port_mapping();

public:
    static const SysConfig & get_inst(){
        static SysConfig inst;
        return inst;
    }

    const stAlgoConfig & get_http_config() const
    {
        return http_config;
    }

    const stSysConfig & get_system_config() const
    {
        return algo_params_config;
    }

    const stFTPSConfig & get_ftps_config() const
    {
        return ftps_config;
    }

    //算法是否配置了矩形坐标
    bool is_configured_rectangle_params(const std::string & algo_type) const {
        return algo_params_config.algo_2_rectangle.count(algo_type) ? true : false;
    }

    //获取该算法类型对应的坐标JSON字符串
    const Points_t & get_algo_points(const std::string & algo_type) const
    {
        return algo_params_config.algo_2_rectangle.at(algo_type);
    }

    //判断该算法类型是配置了调用端口
    bool is_algo_configured_port(const std::string & algo_type) const
    {
        return algo_params_config.algo_2_port.count(algo_type) ? true : false;
    }

    //获取算法配置的调用端口
    int get_algo_configured_port(const std::string & algo_type) const
    {
        return algo_params_config.algo_2_port.count(algo_type) ? algo_params_config.algo_2_port.at(algo_type) : INVALID_PORT;
    }

    //返回算法类型对应的函数指针
    AlgoHandler_t get_algo_functionality(const std::string & algo_type) const
    {
        const auto & mp = algo_params_config.algo_2_functionality;
        return mp.count(algo_type) ? mp.at(algo_type) : nullptr;
    }

protected:
    stFTPSConfig ftps_config;           //ftps客户端配置
    stAlgoConfig http_config;           //http服务配置
    stSysConfig algo_params_config;     //算法参数配置
};


#define SYS_CFG_INST SysConfig::get_inst()
#define FTPS_CFG SysConfig::get_inst().get_ftps_config()
#define HTTP_CFG SysConfig::get_inst().get_http_config()
#define ALGO_PARAMS_CFG SysConfig::get_inst().get_system_config()


#endif // SYS_CONFIG_H
