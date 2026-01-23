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
    void generateTestImages();
    void computeHDR();
    void applyTonemap();

    vector<cv::Mat> images;
    vector<float> exposureTimes;
    cv::Mat hdrImage;
    cv::Mat fusedImage;

    ofImage displayImage;

    bool hdrComputed;
    int displayMode; // 0: underexposed, 1: normal, 2: overexposed, 3: HDR fused

    float gamma;
    float exposure;
};
