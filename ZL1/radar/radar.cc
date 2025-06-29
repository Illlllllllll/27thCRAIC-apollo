#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cyber/cyber.h>
#include "ZL1/proto/examples.pb.h"
#define PACKET_HEADER 0x55AA
//雷达最大检测距离
#define max_dis 500 
//雷达最小检测距离
#define min_dis  40 
//雷达检测中间角
#define middle_angle 180 
//雷达检测左偏角
#define left_angle 100 
//雷达检测右偏角
#define right_angle 220 

using apollo::ZL1::proto::Perception;

int main(int argc, char *argv[]) 
{
    // 初始化cyber
    apollo::cyber::Init(argv[0]);
    // 创建node
    auto talker_node = apollo::cyber::CreateNode("radar_writer_");
    // 创建writer，写Chatter类型消息
    auto talker = talker_node->CreateWriter<Perception>("/apollo/radar/data");
    // 创建一个Perception类型的消息
    auto msg = std::make_shared<Perception>();

    int stm32USB = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY);
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
    // // 创建一个Chatter类型的消息
    // auto msg = std::make_shared<Chatter>();

    uint8_t readBuffer[90];
    struct Packet
    {
        uint16_t header;
        uint8_t number;
        uint16_t a_angle;
        uint16_t z_angle;
        uint16_t data[40];
    };
    struct Packet packet1;
    int left_sta=0;
    int right_sta=0;
    int all = 0;
    int direct = 0;

    while (apollo::cyber::OK()) {     

        // 清空缓冲区
        memset(readBuffer, 0, sizeof(readBuffer));
        packet1.header = 0;
        packet1.number = 0;
        packet1.a_angle = 0;
        packet1.z_angle = 0;
        packet1.data[40] = 0;
        int dis_sta=0;

        // 从STM32读取数据
        int bytesRead = read(stm32USB, readBuffer, sizeof(readBuffer));
        if (bytesRead < 0) {
            AERROR << "读取串口数据时发生错误";
            return -1;
        }
        int readBuffer_length = sizeof(readBuffer)/sizeof(readBuffer[0]);


        if (readBuffer_length!=0)
        {
            packet1.header =(readBuffer[0])  |  (static_cast<uint16_t>(readBuffer[1])  <<  8);
            if (packet1.header  == PACKET_HEADER )
            {
                packet1.number = static_cast<uint8_t>(readBuffer[3]);
                if (packet1.number == 1)
                {
                    all ++;
                }
                packet1.a_angle = (readBuffer[4]) | (static_cast<uint16_t>(readBuffer[5])  <<  8);
                packet1.z_angle = (readBuffer[6]) | (static_cast<uint16_t>(readBuffer[7])  <<  8);
                for (int i =0 ;i < packet1.number; i++ )
                {
                    packet1.data[i]=  (readBuffer[i+10]) | (static_cast<uint16_t>(readBuffer[i+11]) << 8);
                    if (packet1.data[i] <= max_dis*4 &&packet1.data[i] >4*min_dis)
                    {
                        dis_sta ++;
                        // std::cerr <<packet1.data[i] /4<< "d"<<std::endl;
                    }
                }
            } 
            packet1.a_angle = (packet1.a_angle >> 1)/64;
            packet1.z_angle = (packet1.z_angle >> 1)/64;
            if (dis_sta >= 5)
            {
                if ( packet1.a_angle>middle_angle && packet1.a_angle < right_angle && packet1.z_angle < 360 && packet1.a_angle < packet1.z_angle)
                {
                    right_sta ++;
                    // std::cerr <<packet1.a_angle<< "ar" << packet1.z_angle<<std::endl;
                }
                if (packet1.z_angle<middle_angle  && packet1.z_angle > left_angle&& packet1.a_angle < packet1.z_angle)
                {
                    left_sta ++;
                    // std::cerr <<packet1.a_angle <<  "al"<<  packet1.z_angle << std::endl;
                }
                if (packet1.a_angle>left_angle && packet1.a_angle<middle_angle && packet1.z_angle >middle_angle && packet1.z_angle < right_angle)
                {
                    if ((packet1.z_angle-middle_angle)>(middle_angle-packet1.a_angle))
                    {
                        right_sta ++;
                        // std::cerr <<packet1.a_angle <<  "mal"<<  packet1.z_angle << std::endl;
                    }
                    if ((packet1.z_angle-middle_angle)<(middle_angle-packet1.a_angle))
                    {
                        left_sta ++;
                        // std::cerr <<packet1.a_angle<< "mar" << packet1.z_angle<<std::endl;
                    }
                }
            }
        }
        if (all == 2)
        {
            if (right_sta || left_sta)
            {
                if (right_sta>=left_sta)
                {
                    direct  = 1;//turn left
                }
                if (left_sta >right_sta)
                {
                    direct = 2;//turn right
                }
            }
            else
            {
                direct = 0;
            }
            right_sta = 0;
            left_sta = 0;
            all = 1;
            AINFO << direct;
            msg->set_radar_data(direct);
            talker->Write(msg);
        }
    } 

    close(stm32USB);
    return 0;
}