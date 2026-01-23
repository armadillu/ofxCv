#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <opencv2/dnn.hpp>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

private:
    ofVideoGrabber camera;
    cv::Mat frame;

    cv::dnn::Net net;
    bool modelLoaded;

    // EAST model parameters
    int inputWidth = 320;
    int inputHeight = 320;
    float confThreshold = 0.5;
    float nmsThreshold = 0.4;

    struct TextBox {
        cv::RotatedRect box;
        float confidence;
    };

    vector<TextBox> detectedBoxes;

    bool loadModel(const string& modelPath);
    vector<TextBox> detectText(const cv::Mat& input);
    void decode(const cv::Mat& scores, const cv::Mat& geometry,
                float scoreThresh, vector<cv::RotatedRect>& detections,
                vector<float>& confidences);
};
