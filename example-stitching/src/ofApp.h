#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    vector<ofImage> inputImages;
    ofImage panorama;
    cv::Mat panoramaMat;

    bool stitchingDone;
    bool stitchingFailed;
    string statusMessage;

    void loadImages();
    void performStitching();

    // Manual stitching helper functions
    bool findHomography(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& H);
    cv::Mat warpAndBlend(const cv::Mat& img1, const cv::Mat& img2, const cv::Mat& H);
};
