#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void GetFileNames(const string& path, vector<string>& filenames){
    DIR *pDir;
    struct dirent* ptr;
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

constexpr int board_w = 9;
constexpr int board_h = 6;
constexpr int NPoints = board_w * board_h;//棋盘格内角点总数
constexpr int boardSize = 100; //mm

int main() {
    vector<string> file_name;
    string folderPath = "./data/20220227";

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
    for (int j = 0; j < NPoints; j++)
    {
        object.push_back(Point3f((j % board_w) * boardSize, (j / board_w) * boardSize, 0));
    }

    cv::Matx33d intrinsics;//z:相机内参
    cv::Vec4d distortion_coeff;//z:相机畸变系数

    vector<vector<Point3f> > objectv;
    vector<vector<Point2f> > imagev;

    Size corrected_size(1280, 720);
    Mat mapx, mapy;
    Mat corrected;

    ofstream intrinsicfile("intrinsics_front1103.txt");
    ofstream disfile("dis_coeff_front1103.txt");

    int num = 0;
    bool bCalib = false;
    while (num < file_name.size()){
        image = imread(file_name[num]);

        if (image.empty())
            break;
        imshow("corner_image", image);
        waitKey(500);
        cvtColor(image, grayimage, COLOR_BGR2GRAY);
        // tempgray = grayimage;
        bool findchessboard = checkChessboard(grayimage, ChessBoardSize);

        if (findchessboard)
        {
            bool find_corners_result = findChessboardCorners(grayimage, ChessBoardSize, tempcorners, 3);
            if (find_corners_result)
            {
                cornerSubPix(grayimage, tempcorners, Size(5, 5), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
                drawChessboardCorners(image, ChessBoardSize, tempcorners, find_corners_result);
                imshow("corner_image", image);
                waitKey(500);

                objectv.push_back(object);
                imagev.push_back(tempcorners);
                cout << "capture " << num << " pictures" << endl;
            }
        }
        tempcorners.clear();
        num++;
    }

    cv::fisheye::calibrate(objectv, imagev, cv::Size(image.cols,image.rows), intrinsics, distortion_coeff, cv::noArray(), cv::noArray(), flag, cv::TermCriteria(3, 20, 1e-6));
    fisheye::initUndistortRectifyMap(intrinsics, distortion_coeff, cv::Matx33d::eye(), intrinsics, corrected_size, CV_16SC2, mapx, mapy);

    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            intrinsicfile<<intrinsics(i,j)<<"\t";
        }
        intrinsicfile<<endl;
    }
    for(int i=0; i<4; ++i)
    {
        disfile<<distortion_coeff(i)<<"\t";
    }
    intrinsicfile.close();
    disfile.close();

    num = 0;
    while (num < file_name.size())
    {
        image = imread(file_name[num++]);

        if (image.empty())
            break;
        remap(image, corrected, mapx, mapy, INTER_LINEAR, BORDER_TRANSPARENT);

        imshow("corner_image", image);
        imshow("corrected", corrected);
        waitKey(0);
    }

    cv::destroyWindow("corner_image");
    cv::destroyWindow("corrected");

    image.release();
    grayimage.release();
    corrected.release();
    mapx.release();
    mapy.release();

    return 0;
}
