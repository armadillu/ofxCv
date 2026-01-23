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
    ofVideoGrabber cam;

    // Morphological operation parameters
    int morphOp;          // Current operation (MORPH_ERODE, MORPH_DILATE, etc.)
    int kernelSize;       // Size of structuring element
    int kernelShape;      // Shape: MORPH_RECT, MORPH_ELLIPSE, MORPH_CROSS
    int iterations;       // Number of iterations

    // Results
    ofImage original;
    ofImage binary;
    ofImage result;

    void applyMorphology();
    string getOperationName();
    string getShapeName();
};
