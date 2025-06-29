#include <cyber/cyber.h>
#include "ZL1/perception/perception.h"

using namespace cv;

struct BoxInfo {
    int id;
    std::string c;
    double s;
    double x;
    double y;
    double w;
    double h;
};

std::vector<BoxInfo> extractBoxValues(const std::string& boxes) {

  std::regex pattern(R"(Box\((\d+)\s+(\w+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)\))");
  std::vector<BoxInfo> boxInfos;
  std::sregex_iterator iter(boxes.begin(), boxes.end(), pattern);
  std::sregex_iterator end;

  for (; iter != end; ++iter) {
      std::smatch match = *iter;
      BoxInfo boxInfo;
      boxInfo.id = std::stoi(match[1]);
      boxInfo.c = match[2];
      boxInfo.s = std::stod(match[3]);
      boxInfo.x = std::stod(match[4]);
      boxInfo.y = std::stod(match[5]);
      boxInfo.w = std::stod(match[6]);
      boxInfo.h = std::stod(match[7]);
      boxInfos.push_back(boxInfo);
  }

  // 排序函数，结果按照分值降序排序
  std::sort(boxInfos.begin(), boxInfos.end(), [](const BoxInfo& a, const BoxInfo& b) {
      return a.s > b.s;
  });

  return boxInfos;
}

// 模块初始化函数，在加载该模块时调用
bool PerceptionCommonComponent::Init() {

    perception_writer_ = node_->CreateWriter<Result>("/apollo/perception/data");
    // 模型来源，det/seg/clas/paddlex，分别表示模型来源于PaddleDetection、PaddleSeg、PaddleClas和PaddleX
    model_cls_adv = PaddleDeploy::CreateModel("paddlex");
    model_det = PaddleDeploy::CreateModel("paddlex");
    // 模型初始化
    model_cls_adv->Init("/apollo_workspace/myfile/model/PPLCNet/inference_model/model.yml");//图像分类模型初始化
    model_det->Init("/apollo_workspace/myfile/model/Picodet/inference_model/model.yml");//目标检测模型初始化
    
    // 推理引擎初始化
    // 图像分类引擎初始化
    engine_config_cls_adv.model_filename = "/apollo_workspace/myfile/model/PPLCNet/inference_model/model.pdmodel";;
    engine_config_cls_adv.params_filename = "/apollo_workspace/myfile/model/PPLCNet/inference_model/model.pdiparams";;
    engine_config_cls_adv.gpu_id = 0;
    engine_config_cls_adv.use_gpu = true;
    model_cls_adv->PaddleEngineInit(engine_config_cls_adv);
    // 目标检测引擎初始化
    engine_config_det.model_filename = "/apollo_workspace/myfile/model/Picodet/inference_model/model.pdmodel";
    engine_config_det.params_filename = "/apollo_workspace/myfile/model/Picodet/inference_model/model.pdiparams";
    engine_config_det.gpu_id = 0;
    engine_config_det.use_gpu = true;
    model_det->PaddleEngineInit(engine_config_det);

    return true;
}
// 在接收通道有消息到达时调用
bool PerceptionCommonComponent::Proc(const std::shared_ptr<Chatter>& msg0,const std::shared_ptr<Perception>& msg1) {
    
    // 准备数据
    auto msg = std::make_shared<Result>();
    // 读取camera模块发送的1号摄像头的字节流数据
    std::string camera1 = msg1->camera1_data();
    std::vector<unsigned char> buf1(camera1.begin(), camera1.end());
    // 字节流数据还原成图像
    cv::Mat img1 = cv::imdecode(buf1, cv::IMREAD_UNCHANGED);
    if (img1.empty()) {
        AERROR << "Failed to decode image1.";
        return -1;
    }
    // 1号摄像头图像存入image1中
    std::vector<cv::Mat> image1;
    image1.push_back(std::move(img1));
    
    // 读取camera模块发送的2号摄像头的字节流数据
    std::string camera2 = msg1->camera2_data();
    std::vector<unsigned char> buf2(camera2.begin(), camera2.end());
    // 字节流数据还原成图像
    cv::Mat img2 = cv::imdecode(buf2, cv::IMREAD_UNCHANGED);
    cv::Mat img_test = img2;
    if (img2.empty()) {
        AERROR << "Failed to decode image2.";
        return -1;
    }
    std::vector<cv::Mat> image2;
    // 2号摄像头图像存入image2中
    image2.push_back(std::move(img2));

    // 读取camera模块发送的3号摄像头的字节流数据
    std::string camera3 = msg1->camera3_data();
    std::vector<unsigned char> buf3(camera3.begin(), camera3.end());
    // 字节流数据还原成图像
    cv::Mat img3 = cv::imdecode(buf3, cv::IMREAD_UNCHANGED);
    if (img3.empty()) {
        AERROR << "Failed to decode image3.";
        return -1;
    }
    // 3号摄像头图像存入image3中
    std::vector<cv::Mat> image3;
    image3.push_back(std::move(img3));

    // 预测推理
    std::vector<PaddleDeploy::Result> results_cls_adv; // results_cls_adv存放图像分类推理结果
    model_cls_adv->Predict(image3, &results_cls_adv, 1); // 将image1输入模型，执行图像分类推理
    std::vector<PaddleDeploy::Result> results_det_adv; // results_det_adv存放目标检测推理结果
    model_det->Predict(image1, &results_det_adv, 1); // 将image3输入模型，执行目标检测推理

    // 图像分类结果处理
    std::ostringstream oss_cls_adv;
    oss_cls_adv << results_cls_adv[0];
    std::string str_cls_adv = oss_cls_adv.str();
    // 提取括号中的内容
    std::string content_cls_adv = str_cls_adv.substr(str_cls_adv.find("(")+1, str_cls_adv.find(")")-str_cls_adv.find("(")-1);
    std::istringstream iss_cls_adv(content_cls_adv);
    // 逐个读取并转换元素
    int id_cls_adv;
    int c_cls_adv;
    double s_cls_adv;
    iss_cls_adv >> id_cls_adv >> c_cls_adv >> s_cls_adv;
    // 打印输出每个元素
    AINFO << "ID: " << id_cls_adv << " type: " << c_cls_adv << " score: " << s_cls_adv;
    
    // 目标检测结果处理
    int id_det_adv;
    std::string c_det_adv;
    double s_det_adv,x_det_adv,y_det_adv,w_det_adv,h_det_adv;
    std::ostringstream oss_det_adv;
    oss_det_adv << results_det_adv[0];
    std::string str_det_adv = oss_det_adv.str();
    std::vector<BoxInfo> Box = extractBoxValues(str_det_adv);
    // 输出分值最高的类型（当分值大于等于0.7时）
    if (!Box.empty() && Box[0].s >= 0.7) {
        id_det_adv=Box[0].id;c_det_adv=Box[0].c;s_det_adv=Box[0].s;
        x_det_adv=Box[0].x;y_det_adv=Box[0].y;w_det_adv=Box[0].w;h_det_adv=Box[0].h;
    }else{
        id_det_adv=0;c_det_adv="null";s_det_adv=0;x_det_adv=0;y_det_adv=0;w_det_adv=0;h_det_adv=0;
    }
    // 打印输出每个元素
    AINFO << " ID: " << id_det_adv << " type: " << c_det_adv << " score: " << s_det_adv << 
    " X: " << x_det_adv << " Y: " << y_det_adv << " Width: " << w_det_adv << " Height: " << h_det_adv;

    // 发送推理结果
    msg->set_cls_result_1(c_cls_adv);
    msg->set_det_result_1(c_det_adv);
    perception_writer_->Write(msg);

    return true;
}