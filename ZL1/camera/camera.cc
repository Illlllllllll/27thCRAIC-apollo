#include <iostream>
#include <vector>
#include <cyber/cyber.h>
#include "ZL1/proto/examples.pb.h"
#include <opencv2/opencv.hpp>

using apollo::ZL1::proto::Perception;

int main(int argc, char *argv[]) {
    // 初始化cyber
    apollo::cyber::Init(argv[0]);
    // 创建node
    auto talker_node = apollo::cyber::CreateNode("camera_writer_");
    // 创建writer，写Chatter类型消息
    auto talker = talker_node->CreateWriter<Perception>("/apollo/camera/data");
    // 创建一个Chatter类型的消息
    auto msg = std::make_shared<Perception>();

    std::vector<unsigned char> camera1_data;
    std::vector<unsigned char> camera2_data;
    std::vector<unsigned char> camera3_data;

    // 创建OpenCV视频捕获对象
    cv::VideoCapture cap1(0); // 打开默认摄像头0
    cv::VideoCapture cap2(2); // 打开默认摄像头1
    cv::VideoCapture cap3(4); // 打开默认摄像头2

    if (!cap1.isOpened())
    {
        AERROR << "Failed to open camera0.";
        return -1;
    }
    if (!cap2.isOpened())
    {
        AERROR << "Failed to open camera1.";
        return -1;
    }
    if (!cap3.isOpened())
    {
        AERROR << "Failed to open camera2.";
        return -1;
    }

    // 主循环
    while (apollo::cyber::OK())
    {
        cv::Mat frame1,frame2,frame3;

        cap1 >> frame1; // 读取一帧
        cap2 >> frame2; 
        cap3 >> frame3; 

        if (frame1.empty())
        {
            AERROR << "Failed to get the frame1.";
            break;
        }
        if (frame2.empty())
        {
            AERROR << "Failed to get the frame2.";
            break;
        }
        if (frame3.empty())
        {
            AERROR << "Failed to get the frame3.";
            break;
        }

        // 将图像数据编码为字节流
        cv::imencode(".jpg", frame1, camera1_data);
        cv::imencode(".jpg", frame2, camera2_data);
        cv::imencode(".jpg", frame3, camera3_data);
        
        // 设置图像数据到 ImageData 消息中
        std::string imageDataStr1(camera1_data.begin(), camera1_data.end());
        std::string imageDataStr2(camera2_data.begin(), camera2_data.end());
        std::string imageDataStr3(camera3_data.begin(), camera3_data.end());
        msg->set_camera1_data(imageDataStr1);
        msg->set_camera2_data(imageDataStr2);
        msg->set_camera3_data(imageDataStr3);
        talker->Write(msg);

        // 显示帧
        cv::imshow("Frame1", frame1);
        cv::imshow("Frame2", frame2);
        cv::imshow("Frame3", frame3);

        // 按'q'退出
        if (cv::waitKey(1) == 'q')
            break;
    }
    cap1.release();
    cap2.release();
    cap3.release();
    return 0;
}