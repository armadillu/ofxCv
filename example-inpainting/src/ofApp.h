#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

private:
    ofImage originalImg;
    ofImage maskImg;
    ofImage inpaintedImg;
    cv::Mat originalMat;
    cv::Mat maskMat;
    cv::Mat resultMat;

    bool drawing;
    int brushSize;
    bool needsInpaint;
    int inpaintRadius;

    void performInpainting();
};
