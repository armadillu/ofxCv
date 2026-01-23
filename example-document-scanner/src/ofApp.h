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
    cv::Mat frame, gray, edges, morphed;

    // Document corners (clockwise from top-left)
    vector<cv::Point2f> corners;

    // Warped document image
    cv::Mat warped;
    ofImage warpedImg;

    // UI state
    int selectedCorner;
    bool manualMode;

    // Auto-detect document
    bool detectDocument();

    // Get largest quadrilateral contour
    vector<cv::Point2f> getQuadrilateral(const vector<vector<cv::Point>>& contours);

    // Order points: top-left, top-right, bottom-right, bottom-left
    vector<cv::Point2f> orderPoints(const vector<cv::Point2f>& pts);

    // Apply perspective transform
    void warpPerspective();

    // Distance helper
    float pointDistance(const cv::Point2f& a, const cv::Point2f& b);
};
