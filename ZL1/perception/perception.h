#include <gflags/gflags.h>
#include <string>
#include <omp.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include <cassert>
#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "ZL1/proto/examples.pb.h"
#include <opencv2/opencv.hpp>
#include "model_deploy/common/include/paddle_deploy.h"

using apollo::cyber::Component;
using apollo::cyber::ComponentBase;
using apollo::cyber::Writer;
using apollo::ZL1::proto::Chatter;
using apollo::ZL1::proto::Perception;
using apollo::ZL1::proto::Result;

struct Output_Box {
    int id;
    std::string name;
    float score;
    float x1, y1, x2, y2;
};

// 有两个消息源，继承以Chatter为参数的Component模版类
class PerceptionCommonComponent : public Component<Chatter,Perception> {
  public:
    bool Init() override;
    // Proc() 函数的两个参数表示两个channel中的最新的信息
    bool Proc(const std::shared_ptr<Chatter>& msg0,const std::shared_ptr<Perception>& msg1) override;

  private:
    std::shared_ptr<Writer<Result>> perception_writer_ = nullptr;
    PaddleDeploy::Model* model_cls_adv = nullptr;
    // PaddleDeploy::TensorRTEngineConfig engine_config_cls_adv;
    PaddleDeploy::PaddleEngineConfig engine_config_cls_adv;
    PaddleDeploy::Model* model_det = nullptr;
    // PaddleDeploy::TensorRTEngineConfig engine_config_det;
    PaddleDeploy::PaddleEngineConfig engine_config_det;
};

// 将CommonComopnentSample注册在cyber中
CYBER_REGISTER_COMPONENT(PerceptionCommonComponent)