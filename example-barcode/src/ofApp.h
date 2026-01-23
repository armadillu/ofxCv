#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/objdetect.hpp>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    ofVideoGrabber camera;
    cv::barcode::BarcodeDetector barcodeDetector;

    // Detection results
    std::vector<cv::Point2f> corners;
    std::vector<std::string> decodedInfo;
    std::vector<std::string> decodedTypes;

    // Display
    ofPixels cameraPixels;
    ofTexture cameraTexture;
    std::string statusMessage;
    float lastDetectionTime;

    // Test mode
    bool useTestImage;
    ofImage testImage;

    void detectBarcodes(cv::Mat& frame);
};
