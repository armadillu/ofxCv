#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/features2d.hpp>

// OpenCV 4.x SIFT Feature Matching Example
// SIFT (Scale-Invariant Feature Transform) is now free in OpenCV 4.x!
// This example demonstrates feature detection and matching

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    // Feature detector
    cv::Ptr<cv::SIFT> sift;

    // Reference image (captured with 'c')
    cv::Mat referenceImg;
    std::vector<cv::KeyPoint> referenceKeypoints;
    cv::Mat referenceDescriptors;
    bool hasReference;

    // Current frame features
    std::vector<cv::KeyPoint> currentKeypoints;
    cv::Mat currentDescriptors;

    // Matches
    cv::Ptr<cv::BFMatcher> matcher;
    std::vector<cv::DMatch> goodMatches;

    // Homography
    cv::Mat homography;
    std::vector<cv::Point2f> refCorners;
    std::vector<cv::Point2f> sceneCorners;
    bool homographyFound;

    // Settings
    int maxFeatures;
    float ratioThreshold;
    bool showAllKeypoints;

    // Performance
    float detectionTime;
    float matchingTime;
};
