#include <iostream>
#include <fstream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include "Undistort.hpp"
#include "Localization.h"

int main(int argc, char** argv) {

    /**
     * Network camera
     */
    String url = "rtsp://admin:@192.168.136.75:554/";
    VideoCapture cap(url);
    if (!cap.isOpened()) {
        std::cerr << "Cap open failure!" << std::endl;
        return 0;
    }

    /**
     * Load image size
     */
    int inputWidth, inputHeight;
    inputWidth = static_cast<int>(strtol(argv[1], nullptr, 10));
    inputHeight = static_cast<int>(strtol(argv[2], nullptr, 10));
    Size correctedSize(inputWidth, inputHeight);

    /**
     * Initialize undistort map
     */

    while (true) {
        Mat frame, corrected;
        namedWindow("Undistort");
        cap >> frame;
        if (!frame.empty()) {
            Undistort::GetInstance(correctedSize).ExecuteUndistort(frame, corrected);
            imshow("Undistort", corrected);
        } else {
            std::cerr << "Image acquisition error!" << std::endl;
            continue;
        }

        auto input = waitKey(1);
        if ('q' == input) break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}
