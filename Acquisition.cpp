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

#include "SUF401GM.hpp"


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

    SUF401GM cap(0);
    Mat frame;
    namedWindow("img");

    while(true){
        frame = *(cap.GetFrame());
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
    }

    cap.Refresh();
    destroyAllWindows();
    return 0;
}
