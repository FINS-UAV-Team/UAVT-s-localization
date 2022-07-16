#ifndef __VIDEO_HPP
#define __VIDEO_HPP

#include <ctime>
#include <opencv2/opencv.hpp>

class VideoAsCamera {
public:
    VideoAsCamera(const String& file, int fps, bool convertColorToGray);
    ~VideoAsCamera();

    cv::Mat* GetFrame();

private:
    void FPSControl() const;

    cv::Mat* frame = new cv::Mat;
    VideoCapture video;
    int fps;
    bool convertColor;
};

VideoAsCamera::VideoAsCamera(const String &file, int fps, bool convertColorToGray = false) : fps(fps), convertColor(convertColorToGray) {
    video.open(file);
    if(!video.isOpened()) {
        std::cerr << "Video open failure!" << std::endl;
        return;
    }
}

VideoAsCamera::~VideoAsCamera() {
    video.release();
}

cv::Mat *VideoAsCamera::GetFrame() {
    if(!video.read(*frame)) {
        return nullptr;
    }
    if(convertColor) {
        cvtColor(*frame, *frame, COLOR_BGR2GRAY);
    }
    FPSControl();
    return frame;
}

void VideoAsCamera::FPSControl() const {
    static clock_t lastTime = clock();
    while(clock() - lastTime < (1000 / fps));
    lastTime = clock();
}

#endif //__VIDEO_HPP
