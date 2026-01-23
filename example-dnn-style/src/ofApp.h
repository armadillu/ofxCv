#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    ofVideoGrabber camera;
    cv::Mat frame;
    cv::Mat styledFrame;

    cv::dnn::Net net;
    bool modelLoaded;

    vector<string> modelNames;
    int currentModelIndex;

    bool loadModel(const string& modelPath);
    cv::Mat applyStyle(const cv::Mat& input);
};
