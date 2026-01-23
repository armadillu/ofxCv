#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

// OpenCV 4.x DNN Face Detection Example
// This example demonstrates deep neural network-based face detection
// which is more accurate than traditional Haar cascades

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    // OpenCV 4.x DNN module
    cv::dnn::Net faceNet;
    bool modelLoaded;

    // Detection results
    struct FaceDetection {
        cv::Rect box;
        float confidence;
    };
    std::vector<FaceDetection> faces;

    // Settings
    float confidenceThreshold;
    bool showConfidence;

    // Performance
    float inferenceTime;
};
