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

    bool wcaqm(stAlgoInParas & params)//安全帽
    {
        try{
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            // 4️⃣ 发送请求
            auto res = cli.Post("/helmet", items);

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

        }catch(const std::exception & e){
            return false;
        }

        return false;
    }

    bool wtyl(stAlgoInParas & params)//物体遗留
    {
        try{
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

            // 2️⃣ 读取文件内容
            std::ifstream base_ifs(params.algo_base_iamge, std::ios::binary);
            if (!base_ifs) {
                PLOG_ERROR("open file failed\n");
                return false;
            }

            std::stringstream base_buffer;
            base_buffer << base_ifs.rdbuf();
            std::string base_file_content = base_buffer.str();


            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "recg.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            item.name = "referencceImage";
            item.content = base_file_content;
            item.filename = "base.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            // header
            httplib::Headers headers;
            headers.emplace("polygonPoint", params.image_coordinations);
            headers.emplace("function", "0");

            auto res = cli.Post("/perimeter", headers, items);

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

        }catch(const std::exception & e){
            PLOG_ERROR("exception: %s\n", e.what());
            return false;
        }

        return false;
    }

    bool wtyc(stAlgoInParas & params)//物体移除
    {
        try{
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

            // 2️⃣ 读取文件内容
            std::ifstream base_ifs(params.algo_base_iamge, std::ios::binary);
            if (!base_ifs) {
                PLOG_ERROR("open file failed\n");
                return false;
            }

            std::stringstream base_buffer;
            base_buffer << base_ifs.rdbuf();
            std::string base_file_content = base_buffer.str();


            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "recg.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            item.name = "referencceImage";
            item.content = base_file_content;
            item.filename = "base.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            // header
            httplib::Headers headers;
            headers.emplace("polygonPoint", params.image_coordinations);
            headers.emplace("function", "1");

            auto res = cli.Post("/perimeter", headers, items);

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

        }catch(const std::exception & e){
            PLOG_ERROR("exception: %s\n", e.what());
            return false;
        }

        return false;
    }

    bool ywrq(stAlgoInParas & params)//异物入侵
    {
        try{
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

            // 2️⃣ 读取文件内容
            std::ifstream base_ifs(params.algo_base_iamge, std::ios::binary);
            if (!base_ifs) {
                PLOG_ERROR("open file failed\n");
                return false;
            }

            std::stringstream base_buffer;
            base_buffer << base_ifs.rdbuf();
            std::string base_file_content = base_buffer.str();


            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "recg.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            item.name = "referencceImage";
            item.content = base_file_content;
            item.filename = "base.jpg";
            item.content_type = "image/jpeg";
            items.emplace_back(std::move(item));

            // header
            httplib::Headers headers;
            headers.emplace("polygonPoint", params.image_coordinations);
            headers.emplace("function", "2");

            auto res = cli.Post("/perimeter", headers, items);

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

        }catch(const std::exception & e){
            PLOG_ERROR("exception: %s\n", e.what());
            return false;
        }

        return false;
    }

    bool ryjjph(stAlgoInParas & params)//人员聚集
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
            item.filename = "recg.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            // header
            int maxNums = 5;
            httplib::Headers headers;
            headers.emplace("maxNums",std::to_string(maxNums));


            // 4️⃣ 发送请求
            auto res = cli.Post("/personGathering", headers, items);

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

    bool rydd(stAlgoInParas & params)//人员倒地
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));


            // 4️⃣ 发送请求
            auto res = cli.Post("/personFall", items);

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

    bool wcgz(stAlgoInParas & params)//忘穿工装
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));


            // 4️⃣ 发送请求
            auto res = cli.Post("/clothing", items);

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

    bool ryrq(stAlgoInParas & params)//人员入侵
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            httplib::Headers headers;
            headers.emplace("polygonPoint", params.image_coordinations);


            // 4️⃣ 发送请求
            auto res = cli.Post("/personIntrusion", headers, items);

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

    bool yxcr(stAlgoInParas & params)//越线闯入
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            httplib::Headers headers;
            headers.emplace("Points", params.image_coordinations);


            // 4️⃣ 发送请求
            auto res = cli.Post("/personMixingLine", headers, items);

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

    bool claqgk(stAlgoInParas & params)//车辆安全管控
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            httplib::Headers headers;
            headers.emplace("polygonPoint", params.image_coordinations);


            // 4️⃣ 发送请求
            auto res = cli.Post("/car", headers, items);

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

    bool yw_nc(stAlgoInParas & params)//异物-鸟巢
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            // 4️⃣ 发送请求
            auto res = cli.Post("/niaochao", items);

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

    bool xdwcr(stAlgoInParas & params)//小动物闯入
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

            std::vector<httplib::UploadFormData> items;

            httplib::UploadFormData item;
            item.name = "file";
            item.content = file_content;
            item.filename = "file.jpg";
            item.content_type = "image/jpeg";

            items.emplace_back(std::move(item));

            // 4️⃣ 发送请求
            auto res = cli.Post("/animal", items);

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

}