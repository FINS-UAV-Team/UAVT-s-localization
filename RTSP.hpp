#include <iostream>
#include <opencv2/opencv.hpp>

class RTSPCamera {
public:
    explicit RTSPCamera(String url);
    ~RTSPCamera();

    cv::Mat* GetFrame();

private:
    cv::VideoCapture* cap = nullptr;
    cv::Mat* frame = nullptr;
};

RTSPCamera::RTSPCamera(int url) {
    cap = new VideoCapture (url);
    if(!cap->isOpened()) {
        std::cerr << "Cap open failure!" << std::endl;
    }
}

RTSPCamera::~RTSPCamera() {
    cap.release();
}

int *RTSPCamera::GetFrame() {
    cap >> (*frame);
}
