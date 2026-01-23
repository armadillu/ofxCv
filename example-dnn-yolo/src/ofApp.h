#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

// OpenCV 4.x YOLO Object Detection Example
// Uses the DNN module to run YOLO inference for object detection

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    // DNN
    cv::dnn::Net net;
    bool modelLoaded;

    // Detection parameters
    float confThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;

    // Results
    struct Detection {
        int classId;
        float confidence;
        cv::Rect box;
    };
    std::vector<Detection> detections;

    // Class names (COCO dataset)
    std::vector<std::string> classNames;

    // Performance
    float inferenceTime;

    // Colors for drawing
    std::vector<ofColor> colors;

    void loadClassNames();
    void postProcess(const cv::Mat& frame, const std::vector<cv::Mat>& outputs);
};
