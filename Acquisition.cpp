#include <iostream>

#include <opencv2/opencv.hpp>
using namespace cv;

#include <boost/date_time/gregorian/gregorian.hpp>
using namespace boost::gregorian;
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

constexpr auto savePath = "./data/";

#include "CameraApi.h" //相机SDK的API头文件

unsigned char * g_pRgbBuffer;     //处理后数据缓存区

int main() {

    /**
     * Create directory for image storage.
     */
    date today = day_clock::local_day();
    const String dirString = savePath + to_iso_string(today);

    fs::path dir(dirString);
    if(!fs::exists(dir)){
        create_directory(dir);
        std::cout << "Create directory \"" << dirString << '\"' << std::endl;
    }

    ptime op = second_clock::local_time();
    std::cout << op << std::endl;
    String timeString;

    /**
     * MindVision
     */
    int                     iCameraCounts = 1;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList;
    int                     hCamera;
    tSdkCameraCapbility     tCapability;      //设备描述信息
    tSdkFrameHead           sFrameInfo;
    BYTE*			        pbyBuffer;
    int                     iDisplayFrames = 10000;
    Mat* iplImage = NULL;
    int                     channel=3;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList,&iCameraCounts);
    printf("state = %d\n", iStatus);

    printf("count = %d\n", iCameraCounts);
    //没有连接设备
    if(iCameraCounts==0){
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList,-1,-1,&hCamera);

    //初始化失败
    printf("state = %d\n", iStatus);
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera,&tCapability);

    CameraSetExposureTime(hCamera, 8000);
    CameraSetAnalogGain(hCamera, 3);

    //
    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);
    //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(hCamera);

    /*其他的相机参数设置
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
         CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
         CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
         更多的参数的设置方法，，清参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    */

    if(tCapability.sIspCapacity.bMonoSensor){
        channel=1;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        channel=3;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_BGR8);
    }

    String url = "rtsp://admin:@192.168.136.75:554/";
    VideoCapture cap(url);
    if(!cap.isOpened()){
        std::cerr << "Cap open failure!" << std::endl;
        return 0;
    }

    Mat frame;
    namedWindow("img");

    while(true){

        if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS) {
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);

            cv::Mat matImage(
                    cv::Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
                    sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
                    g_pRgbBuffer
            );
            frame = matImage;
        }

        //cap >> frame;
        if(!frame.empty()){
            imshow("img", frame);
        }
        else {
            std::cerr << "Image acquisition error!" << std::endl;
        }

        auto input = waitKey(1);
        if('q' == input) break;
        if('s' == input){
            String frameName = dirString;
            op = second_clock::local_time();
            timeString = to_iso_string(op);
            frameName.append("/").append(timeString).append(".jpg");
            imwrite(frameName, frame);
            std::cout << "Save frame in " << frameName << std::endl;
        }

        CameraReleaseImageBuffer(hCamera,pbyBuffer);
    }

    cap.release();
    destroyAllWindows();
    return 0;
}