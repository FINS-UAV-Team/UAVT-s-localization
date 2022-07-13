#ifndef __SUF401GM_HPP
#define __SUF401GM_HPP

#include <opencv2/opencv.hpp>
#include "CameraApi.h" //相机SDK的API头文件
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
    explicit SUF401GM(tSdkCameraDevInfo*);
    explicit SUF401GM(int index);

    cv::Mat* GetFrame();
    void Refresh();

private:
    int handle;
    tSdkCameraCapbility capability;
    unsigned char* g_pRgbBuffer;     //处理后数据缓存区
    cv::Mat* frame = nullptr;
    BYTE* pbyBuffer;
};

SUF401GM::SUF401GM(tSdkCameraDevInfo* device) {
    int status = CameraInit(device, -1, -1, &handle);
    if(status != CAMERA_STATUS_SUCCESS) {
        std::cerr << "SUF401GM camera failure!" << std::endl;
        return;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(handle, &capability);
    // Options
    CameraSetExposureTime(handle, 8000);
    CameraSetAnalogGain(handle, 3);
    // Allocate memory for image buffer
    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);
    // Enable camera
    CameraPlay(handle);
    // Gray image
    CameraSetIspOutFormat(handle, CAMERA_MEDIA_TYPE_MONO8);
}

SUF401GM::SUF401GM(int index) {
    SUF401GM(MindVision::GetInstance()->GetDevice(index));
}

cv::Mat* SUF401GM::GetFrame() {
    tSdkFrameHead           sFrameInfo;

    if(CameraGetImageBuffer(handle, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS) {
        CameraImageProcess(handle, pbyBuffer, g_pRgbBuffer, &sFrameInfo);

        if(frame != nullptr) {
            delete frame;
            frame = nullptr;
        }

        frame = new cv::Mat (
                cv::Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
                sFrameInfo.uiMediaType == CV_8UC1,
                g_pRgbBuffer
        );
    }

    return frame;
}

void SUF401GM::Refresh() {
    CameraReleaseImageBuffer(handle, pbyBuffer);
}

#endif //__SUF401GM_HPP
