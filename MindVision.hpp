#ifndef __MINDVISION_HPP
#define __MINDVISION_HPP

#include "CameraApi.h" //相机SDK的API头文件

#define CAMERA_NUM 1

/**
 * @brief MindVision SDK initialization and camera detection.
 * @input None
 * @author Xi wang
 * @usage Undistort::GetInstance(correctedSize).ExecuteUndistort(frame, corrected),
where correctedSize is a cv::Size variable with 2 dimensions that indicates the size of undistort image,
frame and corrected are the input and output image with cv::Mat type.
 * @note Singleton design pattern, which means the class can only be constructed once.
 */
class MindVision {
public:
    static MindVision* GetInstance();
    int EnumerateDevices();
    tSdkCameraDevInfo* GetDevice(int index);
protected:
    MindVision();
private:
    static MindVision* mindVision;
    tSdkCameraDevInfo* cameraList = nullptr;
    int cameraCnt;
};

MindVision* MindVision::mindVision = nullptr;

MindVision::MindVision() {
    CameraSdkInit(1); //中文模式
    EnumerateDevices();
}

MindVision* MindVision::GetInstance() {
    if(mindvision == nullptr) {
        mindVision = new MindVision;
    }

    return mindVision;
}

int MindVision::EnumerateDevices() {
    if(cameraList != nullptr) {
        delete [] cameraList;
        cameraList == nullptr; //For safety, in case the following new operator failed.
    }
    cameraList = new tSdkCameraDevInfo [CAMERA_NUM];
    cameraCnt = CAMERA_NUM;
    int status = CameraEnumerateDevice(&cameraList, &cameraCnt);
    if(status != CAMERA_STATUS_SUCCESS) {
        return -1;
    }

    int i = 0;
    for(auto camera : cameraList) {
        std::cout << "相机序号：" << i << "" << std::endl;
        std::cout << "产品名称：" << camera->acProductName << "" << std::endl;
        std::cout << std::endl;
        i++;
    }

    return cameraCnt;
}

tSdkCameraDevInfo* MindVision::GetDevice(int index) {
    return (cameraList + i);
}

#endif //__MINDVISION_HPP
