#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    ofVideoGrabber cam;

    // Stereo matching
    cv::Ptr<cv::StereoBM> stereoBM;
    cv::Mat leftImage, rightImage;
    cv::Mat disparity, disparityVis;

    // Settings
    int numDisparities;
    int blockSize;
    int preFilterCap;
    int uniquenessRatio;
    int speckleWindowSize;
    int speckleRange;

    // Display
    ofImage leftImg, rightImg, depthImg;

    // Mode
    bool useWebcam;
    int currentFrame;

    void processStereoPair();
    void generateSyntheticStereoPair();
    void visualizeDisparity();
};
