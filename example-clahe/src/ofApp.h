#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber camera;
    cv::Mat inputMat, grayMat, claheMat;
    cv::Ptr<cv::CLAHE> clahe;
    ofImage inputImage, outputImage;

    // CLAHE parameters
    double clipLimit;
    int tileGridSize;
    bool showOriginal;
};
