#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    void updateColorRange();
    void detectColor();

    ofVideoGrabber cam;
    cv::Mat frame, hsv, mask;
    ofImage resultImage;

    // HSV color range (adjustable)
    int hueMin, hueMax;
    int satMin, satMax;
    int valMin, valMax;

    // Detected contours
    vector<vector<cv::Point>> contours;
    vector<cv::Rect> boundingBoxes;

    // Predefined color presets
    enum ColorPreset {
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CUSTOM
    };
    ColorPreset currentPreset;

    // UI
    bool showMask;
    bool showContours;
    string presetName;
};
