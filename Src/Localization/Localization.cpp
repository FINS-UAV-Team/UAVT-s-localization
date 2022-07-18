#include <iostream>
#include <fstream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "SUF401GM.hpp"
#include "RTSP.hpp"
#include "Video.hpp"
#include "FPS.hpp"

#include "Undistort.hpp"
#include "PoseDetector.hpp"

#include "Localization.h"

int main(int argc, char** argv) {

    /**
     * Camera
     */
    //RTSPCamera cap("rtsp://admin:@192.168.136.75:554/");
    SUF401GM cap(0);
    //VideoAsCamera cap("../data/video/v1.avi", 20, true);

//    /**
//     * Load image size
//     */
//    int inputWidth, inputHeight;
//    static Size correctedSize(720, 540);
//    if(argc > 2) {
//        inputWidth = static_cast<int>(strtol(argv[1], nullptr, 10));
//        inputHeight = static_cast<int>(strtol(argv[2], nullptr, 10));
//        cout << "Resolution: " << inputWidth << " * " << inputHeight << endl;
//        correctedSize = Size(inputWidth, inputHeight);
//    }

    FPS fps;
    cout << "Enabling AprilTag" << endl;

    while (true) {
        PoseDetector::GetInstance()->Detect(cap.GetFrame(), true);
        std::cout << "FPS: " << fps.Tick() << std::endl;

        auto input = waitKey(1);
        if ('q' == input) break;
    }
    destroyAllWindows();
    return 0;
}
