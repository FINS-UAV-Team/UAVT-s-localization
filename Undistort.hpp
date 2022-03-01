#ifndef LOCALIZATION_UNDISTORT_HPP
#define LOCALIZATION_UNDISTORT_HPP

#include <opencv2/opencv.hpp>

constexpr int DEFAULT_IMAGE_WIDTH = 1280;
constexpr int DEFAULT_IMAGE_HEIGHT = 720;

/**
 * @brief Use the params storaged in the files to undistort the raw image.
 * @author Xi wang
 * @note Singleton design pattern, which means the class can only be constructed once.
 */
class Undistort{
public:
    static Undistort& GetInstance(const cv::Size& _correctedSize);
    bool ExecuteUndistort(Mat& rawImage, Mat& corrected);

protected:
    explicit Undistort(const cv::Size& correctedSize);

private:
    static Undistort* undistort; //Instance
    Mat mapx, mapy;
    static cv::Size correctedSize;

    static bool LoadCalibrationParams(Matx33d& intrinsics, Vec4d& distortion_coeff);
};

Undistort* Undistort::undistort = nullptr;
cv::Size Undistort::correctedSize/*(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT)*/;

Undistort::Undistort(const cv::Size& _correctedSize) {
    /**
     * Load calibration parameter from file
     */
    Matx33d intrinsics;//相机内参
    Vec4d distortion_coeff;//相机畸变系数
    LoadCalibrationParams(intrinsics, distortion_coeff);
    fisheye::initUndistortRectifyMap(intrinsics, distortion_coeff, cv::Matx33d::eye(), intrinsics, correctedSize, CV_16SC2, mapx, mapy);
}

Undistort &Undistort::GetInstance(const Size &_correctedSize) {
    /**
     * Check the size
     */
    if((_correctedSize.height > 100) && (_correctedSize.width > 100)) {
        if (_correctedSize != correctedSize) {
            correctedSize = _correctedSize;
            delete undistort;
            undistort = new Undistort(correctedSize);
        }
    }
    else {
        if (nullptr == undistort) {
            undistort = new Undistort(correctedSize);
        }
    }

    return *undistort;
}

bool Undistort::LoadCalibrationParams(Matx33d& intrinsics, Vec4d& distortion_coeff){

    ifstream intrinsicFile("intrinsics.txt");
    ifstream disFile("dis_coeff.txt");

    for(auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            intrinsicFile >> intrinsics(i, j);
        }
    }
    cout << "Intrinsics:\r\n" << intrinsics << endl;
    for(auto i = 0; i < 4; i++){
        disFile >> distortion_coeff(i);
    }
    cout << "Distortion:\r\n" << distortion_coeff << endl;

    return true;
}

bool Undistort::ExecuteUndistort(Mat& rawImage, Mat& corrected) {
    remap(rawImage, corrected, mapx, mapy, INTER_LINEAR, BORDER_TRANSPARENT);
    return true;
}

#endif //LOCALIZATION_UNDISTORT_HPP
