#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <vector>
#include <memory>
#include "cyber/component/component.h"
#include "ZL1/proto/examples.pb.h"

using apollo::cyber::Component;
using apollo::cyber::ComponentBase;
using apollo::cyber::Writer;
using apollo::ZL1::proto::Perception;
using apollo::ZL1::proto::Result;
using apollo::ZL1::proto::Driver;

// 继承以Driver为参数的Component模版类
class PlanningCommonComponent : public Component<Result,Perception> {
  public:
    bool Init() override;
    // Proc() 函数的两个参数表示两个channel中的最新的信息
    bool Proc(const std::shared_ptr<Result>& msg0,const std::shared_ptr<Perception>& msg1) override;
  private:
    int park_tab;
    int target_gear;
    int target_speed;
    int last_angle;
    int target_angle;
    int last_brake;
    int target_brake;
};
// 将CommonComopnentSample注册在cyber中
CYBER_REGISTER_COMPONENT(PlanningCommonComponent)