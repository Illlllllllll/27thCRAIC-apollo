#include <memory>
#include "cyber/component/component.h"
#include "ZL1/proto/examples.pb.h"
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

using apollo::cyber::Component;
using apollo::cyber::ComponentBase;
using apollo::ZL1::proto::Chatter;
using apollo::ZL1::proto::Perception;

class CollectCommonComponent : public Component<Chatter,Perception> {
  public:
    bool Init() override;
    // Proc() 函数的两个参数表示两个channel中的最新的信息
    bool Proc(const std::shared_ptr<Chatter>& msg0,
    const std::shared_ptr<Perception>& msg1) override;
  private:
    std::string savePath;
    std::stringstream ss;
    std::string outputPath;
    int frameCount;

};
// 将CommonComopnentSample注册在cyber中
CYBER_REGISTER_COMPONENT(CollectCommonComponent)