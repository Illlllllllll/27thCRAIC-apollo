#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cyber/cyber.h>
#include "ZL1/proto/examples.pb.h"

using apollo::ZL1::proto::Chatter;

int main(int argc, char *argv[]) {
    // 初始化cyber
    apollo::cyber::Init(argv[0]);
    // 创建node
    auto talker_node_1 = apollo::cyber::CreateNode("control_writer_1");
    // 创建writer，写Chatter类型消息
    auto talker_1 = talker_node_1->CreateWriter<Chatter>("/apollo/control/ToPerception");
    // 创建node
    auto talker_node_2 = apollo::cyber::CreateNode("control_writer_2");
    // 创建writer，写Chatter类型消息
    auto talker_2 = talker_node_2->CreateWriter<Chatter>("/apollo/control/ToCollect");

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

    char readBuffer[256];
    // 创建一个Chatter类型的消息
    auto msg = std::make_shared<Chatter>();

    while (apollo::cyber::OK()) {     
        // 清空缓冲区
        memset(readBuffer, 0, sizeof(readBuffer));
        // 从STM32读取数据
        int bytesRead = read(stm32USB, readBuffer, sizeof(readBuffer));
        if (bytesRead < 0) {
            AERROR << "读取串口数据时发生错误";
            return -1;
        }
        int readBuffer_length = sizeof(readBuffer)/sizeof(readBuffer[0]);
        if (readBuffer_length!=0){
            int start = static_cast<int>(readBuffer[0]);
            if (start == 127){
                int driving_pattern = static_cast<int>(readBuffer[1]);
                int ctrl_gear = static_cast<int>(readBuffer[2]);
                int real_speed = static_cast<int>(readBuffer[3]);
                int ctrl_speed = static_cast<int>(readBuffer[4]);
                int ctrl_angle = static_cast<int>(readBuffer[5]) * 1.25;
                int ctrl_brake = static_cast<int>(readBuffer[6]);
                int ctrl_park = static_cast<int>(readBuffer[7]);
                int camera_id = static_cast<int>(readBuffer[8]);
                if(driving_pattern == 0){
                    AINFO  << "模块名称: " << "[control]" 
                    << "驾驶模式: " << "[人工驾驶]"
                    << "档位"  << "[" << ctrl_gear  << "]"
                    << "实际速度" << "[" << real_speed << "]"
                    << "控制速度" << "[" << ctrl_speed << "]"
                    << "控制角度" << "[" << ctrl_angle << "]"
                    << "刹车信号" << "[" << ctrl_brake << "]"
                    << "驻车信号"  << "[" << ctrl_park  << "]"
                    << "摄像头编号"  << "[" << camera_id+1  << "]";
                }else if(driving_pattern == 1){
                    AINFO  << "模块名称: " << "[control]"
                    << "驾驶模式: " << "[自动驾驶]"
                    << "档位"  << "[" << ctrl_gear  << "]"
                    << "实际速度" << "[" << real_speed << "]"
                    << "控制速度" << "[" << ctrl_speed << "]"
                    << "控制角度" << "[" << ctrl_angle << "]"
                    << "刹车信号" << "[" << ctrl_brake << "]"
                    << "驻车信号"  << "[" << ctrl_park  << "]"
                    << "摄像头编号"  << "[" << camera_id+1  << "]";
                    msg->set_ctrl_park(ctrl_park);
                    talker_1->Write(msg);
                }else if(driving_pattern == 2){
                    AINFO  << "模块名称: " << "[control]"
                    << "驾驶模式: " << "[数据采集]"
                    << "档位"  << "[" << ctrl_gear  << "]"
                    << "实际速度" << "[" << real_speed << "]"
                    << "控制速度" << "[" << ctrl_speed << "]"
                    << "控制角度" << "[" << ctrl_angle << "]"
                    << "刹车信号" << "[" << ctrl_brake << "]"
                    << "驻车信号"  << "[" << ctrl_park  << "]"
                    << "摄像头编号"  << "[" << camera_id+1  << "]";
                    msg->set_ctrl_angle(ctrl_angle);
                    msg->set_camera_id(camera_id);
                    talker_2->Write(msg);
                }else{}
            } 
        }
    }
    close(stm32USB);
    return 0;
}