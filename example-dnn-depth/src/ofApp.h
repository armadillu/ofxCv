#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

// OpenCV 4.x Monocular Depth Estimation Example
// Uses MiDaS ONNX model for single-image depth prediction

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    cv::dnn::Net net;
    bool modelLoaded = false;

    int inputWidth = 384;
    int inputHeight = 384;
    float inferenceTime = 0.0f;

    bool showDepthMap = true;
    bool showColorized = true;
    bool autoNormalize = true;

    cv::Mat depthMat;
    cv::Mat depthColorized;
    cv::Mat depthNormalized;

    float minDepth = 0.0f;
    float maxDepth = 1.0f;

    void updateDepth(const cv::Mat& frame);
    void normalizeDepth(const cv::Mat& depth, cv::Mat& output);
    void colorizeDepth(const cv::Mat& normalized, cv::Mat& output);
};
