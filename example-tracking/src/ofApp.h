#pragma once

#include "ofMain.h"
#include "ofxCv.h"

// OpenCV 4.x Object Tracking Example
// Uses CAMShift (Continuously Adaptive Mean Shift) tracking
// This algorithm is included in the core video module

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    ofVideoGrabber cam;

    // Tracking state
    cv::Rect trackingBox;
    cv::RotatedRect trackResult;
    bool isTracking;
    bool isSelecting;

    // ROI selection
    cv::Point selectionStart;
    cv::Rect selectionRect;

    // Histogram for CAMShift
    cv::Mat roiHist;
    int histSize;
    float histRange[2];
    const float* histRanges;

    // For visualization
    cv::Mat backProject;
    bool showBackProjection;

    // Performance
    float trackingTime;
};
