#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/aruco.hpp>

// OpenCV 4.x ArUco Marker Detection Example
// ArUco markers are used for augmented reality and camera pose estimation

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    // ArUco detection
    cv::aruco::Dictionary dictionary;
    cv::aruco::DetectorParameters detectorParams;
    cv::aruco::ArucoDetector detector;

    // Detection results
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;

    // Dictionary type
    int currentDict;
    std::vector<std::string> dictNames;

    // For generating marker images
    bool showMarkerPreview;
    ofImage markerPreview;
};
