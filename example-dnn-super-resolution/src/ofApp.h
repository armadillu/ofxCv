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
    cv::Mat superResFrame;

    cv::dnn::Net net;
    bool modelLoaded;

    vector<string> modelNames;
    vector<string> algorithmNames;
    vector<int> scaleFactors;
    int currentModelIndex;
    int inputWidth, inputHeight;

    bool loadModel(const string& modelPath, const string& algorithm, int scale);
    cv::Mat applySuperResolution(const cv::Mat& input);
};
