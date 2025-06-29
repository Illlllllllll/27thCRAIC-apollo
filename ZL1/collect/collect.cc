#include <cyber/cyber.h>
#include "ZL1/collect/collect.h"

// 在加载component时调用
bool CollectCommonComponent::Init() {

  AINFO << "Commontest component init";
  frameCount = 0;
  savePath = "/apollo_workspace/myfile/image_data/"; // 设置保存路径
  return true;
}
// 在主channel有消息到达时调用
bool CollectCommonComponent::Proc(const std::shared_ptr<Chatter>& msg0,
const std::shared_ptr<Perception>& msg1) {

  int ctrl_angle = msg0->ctrl_angle();
  int camera_id = msg0->camera_id();

  if(camera_id == 0){
    // 读取字节流数据
    std::string img = msg1->camera1_data();
    std::vector<unsigned char> buf(img.begin(), img.end());
    // 还原图像
    cv::Mat image = cv::imdecode(buf, cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        AERROR << "Failed to decode image.";
        return -1;
    }
 
    // 保存帧到文件夹
    ss << savePath << ctrl_angle;
    outputPath = ss.str();

    // 获取文件目录路径
    boost::filesystem::path outputDir = boost::filesystem::path(outputPath);
    // 如果目录不存在，则创建目录
    if(!boost::filesystem::exists(outputDir)) {
        boost::filesystem::create_directories(outputDir);
    }
    ss.str("");

    ss << savePath << ctrl_angle << "/image" << frameCount << ".jpg";
    outputPath = ss.str();
    ss.str("");

    cv::imwrite(outputPath, image);

    frameCount++;

    // 将消息的序号格式化输出
    AINFO << "camera:" << camera_id << "image:" << ctrl_angle << " successful!" ;

  }else if(camera_id == 1){
    // 读取字节流数据
    std::string img = msg1->camera2_data();
    std::vector<unsigned char> buf(img.begin(), img.end());
    // 还原图像
    cv::Mat image = cv::imdecode(buf, cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        AERROR << "Failed to decode image.";
        return -1;
    }
    
    // 保存帧到文件夹
    ss << savePath << ctrl_angle;
    outputPath = ss.str();

    // 获取文件目录路径
    boost::filesystem::path outputDir = boost::filesystem::path(outputPath);
    // 如果目录不存在，则创建目录
    if(!boost::filesystem::exists(outputDir)) {
        boost::filesystem::create_directories(outputDir);
    }
    ss.str("");

    ss << savePath << ctrl_angle << "/image" << frameCount << ".jpg";
    outputPath = ss.str();
    ss.str("");

    cv::imwrite(outputPath, image);

    frameCount++;

    // 将消息的序号格式化输出
    AINFO << "camera:" << camera_id << "image:" << ctrl_angle << " successful!" ;

  }else if(camera_id == 2){
    // 读取字节流数据
    std::string img = msg1->camera3_data();
    std::vector<unsigned char> buf(img.begin(), img.end());
    // 还原图像
    cv::Mat image = cv::imdecode(buf, cv::IMREAD_UNCHANGED);

    if (image.empty()) {
        AERROR << "Failed to decode image.";
        return -1;
    }
    
    // 保存帧到文件夹
    ss << savePath << ctrl_angle;
    outputPath = ss.str();

    // 获取文件目录路径
    boost::filesystem::path outputDir = boost::filesystem::path(outputPath);
    // 如果目录不存在，则创建目录
    if(!boost::filesystem::exists(outputDir)) {
        boost::filesystem::create_directories(outputDir);
    }
    ss.str("");

    ss << savePath << ctrl_angle << "/image" << frameCount << ".jpg";
    outputPath = ss.str();
    ss.str("");

    cv::imwrite(outputPath, image);

    frameCount++;

    // 将消息的序号格式化输出
    AINFO << "cameraID:" << camera_id+1 << "type:" << ctrl_angle << " successful!" ;

  }else{}

  return true;
}