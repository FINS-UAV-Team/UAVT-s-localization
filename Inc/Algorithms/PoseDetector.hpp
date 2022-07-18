#ifndef __POSE_DETECTOR_HPP
#define __POSE_DETECTOR_HPP

#include <opencv2/opencv.hpp>

extern "C" {
#include "apriltag.h"
#include "tag16h5.h"
#include "common/getopt.h"
}

class PoseDetector {
public:
    static PoseDetector* GetInstance();
    void Detect(cv::Mat* frame, bool _showResult);

protected:
    PoseDetector();
    ~PoseDetector();

private:
    cv::Mat output;
    bool showResult = true;
    apriltag_family_t* tf = tag16h5_create();
    apriltag_detector_t* td = apriltag_detector_create();
    zarray_t* detections = nullptr;
    static PoseDetector* poseDetector;

    void ShowDetectResult();
};

PoseDetector* PoseDetector::poseDetector = nullptr;

PoseDetector::PoseDetector() {
    /**
     * Configurations
     */
    apriltag_detector_add_family(td, tf);
    td->quad_decimate = 0;
    td->quad_sigma = 0;
    td->nthreads = 3;
    td->debug = false;
    td->refine_edges = false;
}

PoseDetector::~PoseDetector() {
    tag16h5_destroy(tf);
    apriltag_detector_destroy(td);
}

PoseDetector *PoseDetector::GetInstance() {
    if(nullptr == poseDetector) {
        poseDetector = new PoseDetector;
    }
    return poseDetector;
}

void PoseDetector::Detect(Mat *frame, bool _showResult = true) {
    showResult = _showResult;
    if(showResult) {
        output = frame->clone();
    }
    // Make an image_u8_t header for the Mat data
    image_u8_t im = { .width = frame->cols,
            .height = frame->rows,
            .stride = frame->cols,
            .buf = frame->data
    };

    detections = apriltag_detector_detect(td, &im);
    if(showResult) {
        ShowDetectResult();
    }
    apriltag_detections_destroy(detections);
}

void PoseDetector::ShowDetectResult() {
    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);
        line(output, Point(det->p[0][0], det->p[0][1]),
             Point(det->p[1][0], det->p[1][1]),
             Scalar(0, 0xff, 0), 2);
        line(output, Point(det->p[0][0], det->p[0][1]),
             Point(det->p[3][0], det->p[3][1]),
             Scalar(0, 0, 0xff), 2);
        line(output, Point(det->p[1][0], det->p[1][1]),
             Point(det->p[2][0], det->p[2][1]),
             Scalar(0xff, 0, 0), 2);
        line(output, Point(det->p[2][0], det->p[2][1]),
             Point(det->p[3][0], det->p[3][1]),
             Scalar(0xff, 0, 0), 2);

        stringstream ss;
        ss << det->id;
        String text = ss.str();
        int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
        double fontscale = 1.0;
        int baseline;
        Size textsize = getTextSize(text, fontface, fontscale, 2,
                                    &baseline);
        putText(output, text, Point(det->c[0]-textsize.width/2,
                                       det->c[1]+textsize.height/2),
                fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
    }

    cv::imshow("result", output);
}

#endif //__POSE_DETECTOR_HPP
