#ifndef ALGO_IMPL_H
#define ALGO_IMPL_H


#include "struct_define.h"

namespace ALGO
{
    //相关算法检测方法
    bool wcaqm(stAlgoInParas & stAlgoParas);//安全帽
    bool hzyw(stAlgoInParas & stAlgoParas);//火灾烟雾
    bool wtyl(stAlgoInParas & stAlgoParas);//物体遗留
    bool wtyc(stAlgoInParas & stAlgoParas);//物体移除
    bool ywrq(stAlgoInParas & stAlgoParas);//异物入侵
    bool ryjjph(stAlgoInParas & stAlgoParas);//人员聚集
    bool rydd(stAlgoInParas & stAlgoParas);//人员倒地
    bool wcgz(stAlgoInParas & stAlgoParas);//忘穿工装
    bool ryrq(stAlgoInParas & stAlgoParas);//人员入侵
    bool yxcr(stAlgoInParas & stAlgoParas);//越线闯入
    bool claqgk(stAlgoInParas & stAlgoParas);//车辆安全管控
    bool yw_nc(stAlgoInParas & stAlgoParas);//异物-鸟巢
    bool xdwcr(stAlgoInParas & stAlgoParas);//小动物闯入
}


#define MAKE_ALGO_PARI(func) \
    std::make_pair(#func, &ALGO::func)

#endif // ALGO_IMPL_H
