#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>

#include "opencv2/opencv.hpp"

#include "Fisheye.h"

using namespace cv;
using namespace std;

void GetFileNames(const string& path, vector<string>& filenames){
    DIR *pDir;
    struct dirent *ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout << "Folder doesn't Exist!" << endl;
        return;
    }

    while((ptr = readdir(pDir)) != nullptr){
        if (strcmp(ptr->d_name, ".") != 0  && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
        }
    }
    closedir(pDir);
}

void ShowUndistortImage(vector<string>& file_name, Mat& mapx, Mat& mapy, bool showImage = false){
    Mat raw, corrected;
    for(const auto& file_it : file_name){
        raw = imread(file_it);
        if (raw.empty()) break;
        remap(raw, corrected, mapx, mapy, INTER_LINEAR, BORDER_TRANSPARENT);
        if(showImage){
            imshow("corrected", corrected);
            waitKey(0);
        }
    }
    raw.release();
    corrected.release();
}

int main() {
    vector<string> file_name;
    string folderPath = "./data/20220301";

    GetFileNames(folderPath,file_name);

    Mat image, grayimage;
    Size ChessBoardSize = cv::Size(board_w, board_h);
    vector<Point2f> tempcorners;

    int flag = 0;
    flag |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    //flag |= cv::fisheye::CALIB_CHECK_COND;
    flag |= cv::fisheye::CALIB_FIX_SKEW;
    //flag |= cv::fisheye::CALIB_USE_INTRINSIC_GUESS;

    vector<Point3f> object;
    for (auto j = 0; j < NPoints; j++){
        object.emplace_back(static_cast<float>((j % board_w) * squareSize), static_cast<float>((j / board_w) * squareSize), 0);
    }

    cv::Matx33d intrinsics;//z:相机内参
    cv::Vec4d distortion_coeff;//z:相机畸变系数

    vector<vector<Point3f>> objectv;
    vector<vector<Point2f>> imagev;

    Size corrected_size(1280, 720);

    /**
     * Find corners
     */
    for(const auto& file_it : file_name){
        image = imread(file_it);
        if (image.empty()) break;
/*        imshow("corner_image", image);
        waitKey(100);*/

        cvtColor(image, grayimage, COLOR_BGR2GRAY);
        if (checkChessboard(grayimage, ChessBoardSize)) {
            if (findChessboardCorners(grayimage, ChessBoardSize, tempcorners, 3)){
                cornerSubPix(grayimage, tempcorners, Size(5, 5), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 100, 0.1));
                drawChessboardCorners(image, ChessBoardSize, tempcorners, true);
                imshow("corner_image", image);
                waitKey(500);

                objectv.push_back(object);
                imagev.push_back(tempcorners);
                //cout << "capture " << num << " pictures" << endl;
            }
        }
        tempcorners.clear();
    }
    Size imageSize(image.cols, image.rows);
    cout << "Image size: " << image.cols << " cols, " << image.rows << " rows" << endl;
    image.release();
    grayimage.release();

    /**
     * Fisheye undistort
     */
    Mat mapx, mapy;
    cv::fisheye::calibrate(objectv, imagev, imageSize, intrinsics, distortion_coeff, cv::noArray(), cv::noArray(), flag, cv::TermCriteria(3, 100, DBL_EPSILON));
    fisheye::initUndistortRectifyMap(intrinsics, distortion_coeff, cv::Matx33d::eye(), intrinsics, corrected_size, CV_16SC2, mapx, mapy);

    /**
     * File operation for parameter log
     */
    ofstream intrinsicfile("intrinsics.txt");
    ofstream disfile("dis_coeff.txt");
    for(auto i = 0; i < 3; i++){
        for(auto j = 0; j < 3; j++){
            intrinsicfile << intrinsics(i, j) << "\t";
        }
        intrinsicfile << endl;
    }
    for(auto i = 0; i < 4; i++){
        disfile << distortion_coeff(i) << "\t";
    }
    intrinsicfile.close();
    disfile.close();

    ShowUndistortImage(file_name, mapx, mapy);

    destroyAllWindows();

    mapx.release();
    mapy.release();

    return 0;
}
