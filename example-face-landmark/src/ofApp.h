#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

private:
    ofVideoGrabber cam;
    cv::CascadeClassifier faceCascade;
    cv::CascadeClassifier eyeCascade;
    cv::CascadeClassifier noseCascade;
    cv::CascadeClassifier mouthCascade;

    bool faceCascadeLoaded;
    bool eyeCascadeLoaded;
    bool noseCascadeLoaded;
    bool mouthCascadeLoaded;

    std::vector<cv::Rect> faces;
    std::vector<cv::Rect> eyes;
    std::vector<cv::Rect> noses;
    std::vector<cv::Rect> mouths;

    ofImage frame;
};
