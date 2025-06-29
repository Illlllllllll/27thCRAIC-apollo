#include <cyber/cyber.h>
#include "ZL1/planning/planning.h"

// 将整数转换为十六进制字符串
std::string intToHexString(int value) {
    std::stringstream stream;
    stream << std::hex << value;
    return stream.str();
}

// 将十六进制字符串转换为字节
std::vector<uint8_t> hexStringToBytes(const std::string& hexString) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hexString.length(); i += 2) {
        std::string byteString = hexString.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, 0, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

int USBWriter(int target_gear, int target_speed, int target_angle, int target_brake){
  int stm32USB = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
  if (stm32USB == -1) {
      AERROR << "无法打开STM32的USB串口";
      return -1;
  }
  struct termios tty;
  if (tcgetattr(stm32USB, &tty) != 0) {
      AERROR << "无法获取串口属性";
      return -1;
  }
  // 配置串口参数
  tty.c_cflag &= ~PARENB; // 无奇偶校验
  tty.c_cflag &= ~CSTOPB; // 1位停止位
  tty.c_cflag |= CS8;     // 8位数据位
  tty.c_cflag &= ~CRTSCTS; // 无硬件流控制
  tty.c_cflag |= CREAD | CLOCAL; // 启用接收和忽略Modem控制信号
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // 非规范模式，禁止回显和信号处理
  tty.c_oflag &= ~OPOST; // 输出模式原始数据
  tty.c_cc[VMIN] = 1;  // 至少读取一个字节
  tty.c_cc[VTIME] = 10; // 读取超时时间（单位：0.1s）
  cfsetispeed(&tty, B115200); // 设置输入波特率为115200bps
  cfsetospeed(&tty, B115200); // 设置输出波特率为115200bps
  if (tcsetattr(stm32USB, TCSANOW, &tty) != 0) {
      AERROR << "无法设置串口属性";
      return -1;
  }
  int data[] = {200,target_gear,target_speed,target_angle*0.8,target_brake,201};
  for(int i=0;i<=5;i++){
      // 打印整型数据
      AINFO << "Received data: " << data[i];
      // 发送数据
      std::string hexString = intToHexString(data[i]);
      std::vector<uint8_t> bytes = hexStringToBytes(hexString);
      write(stm32USB, bytes.data(), bytes.size());
  }
  close(stm32USB);
  return 0;
}

// 模块初始化函数，在加载该模块时调用
bool PlanningCommonComponent::Init() {

  target_gear = 0; //档位信号，0表示前进，1表示倒车
  target_speed = 0; //速度变量，初始速度为0, 最大速度范围为0-100
  target_angle = 90; //角度变量，初始角度为90, 最大角度范围为40-140
  target_brake = 1; //刹车信号，0表示启动，1表示刹车
  AINFO << "Commontest component init";
  return true;
}
// 在接收通道有消息到达时调用
bool PlanningCommonComponent::Proc(const std::shared_ptr<Result>& msg0,const std::shared_ptr<Perception>& msg1) {
  
  // int ctrl_park = msg0->ctrl_park(); // 接收驻车信号
  int cls_result_1 = msg0->cls_result_1(); // 接收图像分类结果
  std::string det_result_1 = msg0->det_result_1(); // 接收目标检测结果
  int radar_data = msg1->radar_data(); // 接收雷达信号

  // 将消息格式化输出
//   AINFO << "component: "     << "[planning]"  
//         << "ctrl_park : [" << ctrl_park  << "]"
//         << "cls_result_1: [" << cls_result_1 << "]"
//         << "det_result_1: [" << det_result_1 << "]"
//         << "radar_data: [" << radar_data << "]";

// 逻辑处理
if(radar_data == 0){
    if(det_result_1=="red"){
        target_brake=1;
    }else{
        target_brake=0;
    }
    if(cls_result_1==40||cls_result_1==140){
        target_speed=40;
    }else{
        target_speed=40;
    }

    if(cls_result_1==50){
        target_angle=101;
    }
    else if(cls_result_1==90){  //直走
        target_angle=101;
    }
    else if(cls_result_1==135){ // 车库
        target_angle=140;
    }
    else{
        target_angle=cls_result_1;
    }
}else if(radar_data == 1){
    target_angle=70;
    target_speed=35;
}else if(radar_data == 2){
    target_angle=125;
    target_speed=25;
}
    
USBWriter(target_gear,target_speed,target_angle,target_brake);
  if(det_result_1=="red"){
    AINFO << "人行道前停车礼让行人\n人行道前停车礼让行人\n人行道前停车礼让行人\n人行道前停车礼让行人\n人行道前停车礼让行人\n";
  }
  return true;
}
