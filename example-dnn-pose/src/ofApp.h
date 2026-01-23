#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

// OpenCV 4.x Human Pose Estimation Example
// Uses OpenPose or similar models for body keypoint detection

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

    // Pose keypoints (COCO format: 18 points)
    struct Keypoint {
        cv::Point2f position;
        float confidence;
    };
    std::vector<std::vector<Keypoint>> detectedPoses;

    // Parameters
    float confidenceThreshold;
    int inputWidth, inputHeight;

    // Performance
    float inferenceTime;

    // Skeleton connections (COCO format)
    std::vector<std::pair<int, int>> skeleton;
    std::vector<std::string> keypointNames;

    void processOutput(const cv::Mat& output, int frameWidth, int frameHeight);
};
