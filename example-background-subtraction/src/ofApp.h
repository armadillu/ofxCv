#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    void processBackgroundSubtraction();

    ofVideoGrabber cam;
    cv::Mat frame, fgMask;
    ofImage maskImage, resultImage;

    // Background subtractor (OpenCV 4.x style)
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;

    // Parameters (adjustable with keys)
    int history;          // Number of frames in history
    double varThreshold;  // Threshold for pixel classification
    bool detectShadows;   // Enable shadow detection

    // Morphological operations for noise reduction
    bool useMorphology;
    int morphKernelSize;

    // Detected motion contours
    vector<vector<cv::Point>> contours;
    vector<cv::Rect> boundingBoxes;

    // UI
    bool showMask;
    bool showContours;
    bool learningMode;
    double learningRate;
};
