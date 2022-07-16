#include <iostream>

#include <opencv2/opencv.hpp>
using namespace cv;

#include <boost/date_time/gregorian/gregorian.hpp>
using namespace boost::gregorian;
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

constexpr auto savePath = "../data/";

#include "SUF401GM.hpp"
#include "RTSP.hpp"
#include "Video.hpp"

#define VIDEO_MODE
constexpr int FPS = 20;

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
     * Choose a camera for image input
     */
    //RTSPCamera cap("rtsp://admin:@192.168.136.75:554/");
    SUF401GM cap(0);
    //VideoAsCamera cap("../data/video/v1.avi", 20);

    /**
     * Preparation for acquisition
     */
    Mat frame;
#ifdef VIDEO_MODE
    VideoWriter out;
    op = second_clock::local_time();
    timeString = to_iso_string(op);
    std::cout << "Video saved path : " + dirString + "/" + timeString + ".avi" << std::endl;

    frame = *(cap.GetFrame());
    out.open(
            dirString + "/" + timeString + ".avi",
            VideoWriter::fourcc('D', 'I', 'V', 'X'),
            FPS,
            Size(frame.cols, frame.rows)
    );
    if(!out.isOpened()) {
        std::cerr << "Create video failed!" << std::endl;
    }
#endif

    while(true) {
        frame = *(cap.GetFrame());
        if(!frame.empty()) {
            imshow("img", frame);
#ifdef VIDEO_MODE
            if(1 == frame.channels()) cvtColor(frame, frame, COLOR_GRAY2BGR);
#endif
        }
        else {
            std::cerr << "Image acquisition error!" << std::endl;
        }

        auto input = waitKey(1);
        if('q' == input) {
#ifdef VIDEO_MODE
            out.release();
#endif
            break;
        }

#ifdef VIDEO_MODE
        out.write(frame);
#else
        if('s' == input){
            String frameName = dirString;
            op = second_clock::local_time();
            timeString = to_iso_string(op);
            frameName.append("/").append(timeString).append(".jpg");
            imwrite(frameName, frame);
            std::cout << "Save frame in " << frameName << std::endl;
        }
#endif
    } //While loop end

#ifdef VIDEO_MODE
    out.release();
#endif
    destroyAllWindows();
    return 0;
}
