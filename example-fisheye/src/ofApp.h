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

    // Fisheye calibration parameters
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

    // Undistortion maps
    cv::Mat map1, map2;

    // Results
    ofImage original;
    ofImage undistorted;

    // UI parameters
    float k1, k2, k3, k4;  // Distortion coefficients
    float fx, fy;          // Focal lengths
    float cx, cy;          // Principal point
    bool applyUndistortion;

    void updateCalibration();
    void applyFisheyeUndistortion();
};
