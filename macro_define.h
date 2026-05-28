#ifndef MACRO_DEFINE_H
#define MACRO_DEFINE_H

#include <cstdint>

constexpr const char * HTTP_REQUEST_URL = "/picAnalyse";                        //算法请求的URL地址
constexpr const char * HTTP_ALGO_UPT_URL = "/algorithmUpdate";                  //算法更新请求URL地址
constexpr const char * HTTP_ALGO_UPT_RESULT_URL = "/algorithmUpdateResult";     //算法更新结果地址

constexpr std::uint64_t HTTP_REQUEST_BODY_MAX_LENGTH = 10000;       //http的baody最大字节数 - 编译器常量
constexpr const char * PACK_TYPE = "application/json";              //http报文类型
constexpr const char * ALGO_ANALY_FAILED = "{\"code\":\"2002\"}";       //算法分析失败
constexpr const char * ALGO_PIC_ERROR = "{\"code\":\"2001\"}";          //图像数据错误   //算法分析失败
constexpr const char * INVALID_HTTP_PACKAGE = "{\"code\":400}";         //非法报文

constexpr std::uint8_t ALGO_RECG_VALUE_NO_DEFECT = 0;	      //算法识别后无缺陷
constexpr std::uint8_t ALGO_RECG_VALUE_WITH_DEFECT = 1;	      //算法识别后有缺陷
constexpr std::int32_t ALGO_RECG_FAILURE = 2000;              //算法分析失败

constexpr std::int32_t INVALID_PORT = -1;                     //无效端口号

#endif // MACRO_DEFINE_H
