#include "algo_impl.h"
#include "PatrolLogger.h"
#include "httplib.h"
#include "sys_config.h"

namespace ALGO {

    //火灾烟雾
    bool hzyw(stAlgoInParas & params)
    {
        try {
            // 1️⃣ 构造 client
            httplib::Client cli(HTTP_CFG.algo_host_ip, params.port);
            cli.set_connection_timeout(5);    // 可选
            cli.set_read_timeout(10);

            // 2️⃣ 读取文件内容
            std::ifstream ifs(params.image_from_patrol_host, std::ios::binary);
            if (!ifs) {
                PLOG_ERROR("open file failed\n");
                return false;
            }

            std::stringstream buffer;
            buffer << ifs.rdbuf();
            std::string file_content = buffer.str();

            // 2️⃣ 构造 multipart
            // httplib::UploadFormData items = {
            //     {"file", file_content, "file.jpg", "image/jpeg"}
            // };
            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));


            // 4️⃣ 发送请求
            auto res = cli.Post("/smokeFire", items);

            // 5️⃣ 处理返回
            if (!res) {
                PLOG_ERROR("httplib request failed\n");
                return false;
            }

            if (res->status != 200) {
                PLOG_ERROR("http status error: %d\n", res->status);
                return false;
            }

            params.algo_ret_js_data = res->body;

            return true;
        }
        catch (const std::exception &e) {
            PLOG_ERROR("exception: %s\n", e.what());
            return false;
        }

        return false;
    }

    bool wcaqm(stAlgoInParas & stAlgoParas)//安全帽
    {
        return false;
    }

    bool wtyl(stAlgoInParas & stAlgoParas)//物体遗留
    {
        return false;
    }

    bool wtyc(stAlgoInParas & stAlgoParas)//物体移除
    {
        return false;
    }

    bool ywrq(stAlgoInParas & stAlgoParas)//异物入侵
    {
        return false;
    }

    bool ryjjph(stAlgoInParas & stAlgoParas)//人员聚集
    {
        return false;
    }

    bool rydd(stAlgoInParas & stAlgoParas)//人员倒地
    {
        return false;
    }

    bool wcgz(stAlgoInParas & stAlgoParas)//忘穿工装
    {
        return false;
    }

    bool ryrq(stAlgoInParas & stAlgoParas)//人员入侵
    {
        return false;
    }

    bool yxcr(stAlgoInParas & stAlgoParas)//越线闯入
    {
        return false;
    }

    bool claqgk(stAlgoInParas & stAlgoParas)//车辆安全管控
    {
        return false;
    }

    bool yw_nc(stAlgoInParas & stAlgoParas)//异物-鸟巢
    {
        return false;
    }

    bool xdwcr(stAlgoInParas & stAlgoParas)//小动物闯入
    {
        return false;
    }

}