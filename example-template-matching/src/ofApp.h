#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

private:
    ofVideoGrabber camera;
    cv::Mat frame, templ, result;
    ofImage templateImg;

    // Template capture state
    bool capturing = false;
    bool hasTemplate = false;
    cv::Point captureStart;
    cv::Point captureEnd;

    // Matching results
    cv::Point matchLoc;
    float matchVal = 0.0f;

    // Settings
    int matchMethod = cv::TM_CCOEFF_NORMED;
    float threshold = 0.8f;

    void captureTemplate(cv::Point start, cv::Point end);
    void performMatching();
    void drawUI();
};
