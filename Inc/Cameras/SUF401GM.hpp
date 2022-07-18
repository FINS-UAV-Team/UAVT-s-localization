#ifndef __SUF401GM_HPP
#define __SUF401GM_HPP

#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "CameraApi.h" //SDK API
#include "MindVision.hpp"

/**
 * @brief Driver for MindVision's industrial camera SUF401GM.
 * @input Camera ID, which can be obtained by
 * @author Xi wang
 * @usage Undistort::GetInstance(correctedSize).ExecuteUndistort(frame, corrected),
where correctedSize is a cv::Size variable with 2 dimensions that indicates the size of undistort image,
frame and corrected are the input and output image with cv::Mat type.
 * @note Singleton design pattern, which means the class can only be constructed once.
 */
class SUF401GM {
public:
    explicit SUF401GM(int index);
    ~SUF401GM();

    cv::Mat* GetFrame();

private:
    int handle;
    cv::Mat* frame = nullptr;

    tSdkCameraCapbility capability;
    tSdkFrameHead sFrameInfo;
    unsigned char* cameraData;
    unsigned char* rawData;
    unsigned char* matData;     //处理后数据缓存区

    void Acquire();
    void Process();

    bool get = false, finish = false;
};

SUF401GM::SUF401GM(int index) {
    int status = CameraInit(MindVision::GetInstance()->GetDevice(index), -1, -1, &handle);
    if(status != CAMERA_STATUS_SUCCESS) {
        std::cerr << "SUF401GM camera failure!" << std::endl;
        return;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(handle, &capability);
    // Options

    // Allocate memory for image buffer
    rawData = (unsigned char*)malloc(capability.sResolutionRange.iHeightMax * capability.sResolutionRange.iWidthMax);
    matData = (unsigned char*)malloc(capability.sResolutionRange.iHeightMax * capability.sResolutionRange.iWidthMax);

    frame = new cv::Mat (
            cv::Size(capability.sResolutionRange.iWidthMax, capability.sResolutionRange.iHeightMax),
            CV_8UC1,
            matData
    );

    // Enable camera
    CameraPlay(handle);
    // Gray image
    CameraSetIspOutFormat(handle, CAMERA_MEDIA_TYPE_MONO8);

    std::thread threadAcquired(&SUF401GM::Acquire, this);
    std::thread threadProcess(&SUF401GM::Process, this);

    threadAcquired.detach();
    threadProcess.detach();
}

SUF401GM::~SUF401GM() {
    CameraStop(handle);
    CameraUnInit(handle);
    free(matData);
}

cv::Mat* SUF401GM::GetFrame() {
    while(!finish);
    finish = false;
    return frame;
}

void SUF401GM::Acquire() {
    while(true) {
        if (CameraGetImageBuffer(handle, &sFrameInfo, &cameraData, 1000) == CAMERA_STATUS_SUCCESS) {
            memcpy(rawData, cameraData, sFrameInfo.iWidth * sFrameInfo.iHeight);
            CameraReleaseImageBuffer(handle, cameraData);
            get = true;
        }
    }
}

void SUF401GM::Process() {
    while(true) {
        while(!get);
        CameraImageProcess(handle, rawData, matData, &sFrameInfo);
        get = false;
        finish = true;
    }
}

#endif //__SUF401GM_HPP
