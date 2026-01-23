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
    void runGrabCut();
    void resetMask();
    void createTestImage();

    ofImage img;
    cv::Mat image;
    cv::Mat mask;
    cv::Mat bgModel, fgModel;
    cv::Mat result;

    ofTexture resultTexture;

    // Rectangle selection for GrabCut
    cv::Rect selection;
    bool bDrawingRect;
    cv::Point rectStart;

    // Drawing mode
    enum DrawMode {
        MODE_RECT,
        MODE_FG_DRAW,
        MODE_BG_DRAW
    };
    DrawMode drawMode;
    int brushSize;

    bool bProcessed;
    string statusText;
};
