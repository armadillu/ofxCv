#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

// OpenCV 4.x Semantic Segmentation Example
// Uses a DeepLabV3-style ONNX model for pixel-wise labeling

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofVideoGrabber cam;

    cv::dnn::Net net;
    bool modelLoaded = false;

    int inputWidth = 513;
    int inputHeight = 513;
    float inferenceTime = 0.0f;
    float overlayAlpha = 0.55f;

    bool showOverlay = true;
    bool showMaskThumbnail = true;
    bool showLegend = false;

    cv::Mat overlayMat;
    cv::Mat maskMat;
    cv::Mat classIdMapResized;

    std::vector<std::string> classNames;
    std::vector<cv::Vec3b> classColors;

    void loadClassNames();
    void buildClassColors();
    void updateSegmentation(const cv::Mat& frame);
    int getClassIdAt(int x, int y) const;
};
