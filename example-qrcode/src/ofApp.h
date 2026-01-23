#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/objdetect.hpp>

// OpenCV 4.x QR Code Detection Example
// This example demonstrates the built-in QR code detector in OpenCV 4.x

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    // OpenCV 4.x QR Code Detector
    cv::QRCodeDetector qrDecoder;

    // Detection results
    std::string decodedText;
    std::vector<cv::Point> qrPoints;
    bool qrDetected;

    // Display
    ofTrueTypeFont font;
    std::vector<std::string> history;
    static const int maxHistory = 5;
};
