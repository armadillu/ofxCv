#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetWindowTitle("example-clahe - Adaptive Histogram Equalization");

    // Try to initialize camera
    camera.setup(640, 480);

    // Initialize CLAHE with default parameters
    clipLimit = 2.0;
    tileGridSize = 8;
    clahe = cv::createCLAHE(clipLimit, cv::Size(tileGridSize, tileGridSize));

    showOriginal = false;

    ofLogNotice() << "CLAHE example initialized";
    ofLogNotice() << "Controls:";
    ofLogNotice() << "  SPACE: Toggle original/enhanced view";
    ofLogNotice() << "  UP/DOWN: Adjust clip limit (contrast)";
    ofLogNotice() << "  LEFT/RIGHT: Adjust tile grid size";
}

void ofApp::update() {
    camera.update();

    if (camera.isInitialized() && camera.isFrameNew()) {
        // Convert to cv::Mat
        inputMat = toCv(camera);

        // Convert to grayscale
        if (inputMat.channels() == 3) {
            cv::cvtColor(inputMat, grayMat, cv::COLOR_BGR2GRAY);
        } else {
            grayMat = inputMat.clone();
        }

        // Apply CLAHE (Contrast Limited Adaptive Histogram Equalization)
        clahe->apply(grayMat, claheMat);

        // Convert back to ofImage for display
        toOf(grayMat, inputImage);
        toOf(claheMat, outputImage);
        inputImage.update();
        outputImage.update();
    }
}

void ofApp::draw() {
    ofBackground(0);

    if (camera.isInitialized()) {
        // Draw either original or enhanced image
        if (showOriginal) {
            inputImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        } else {
            outputImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        }

        // Draw info overlay
        ofSetColor(255);
        stringstream info;
        info << "CLAHE - Contrast Limited Adaptive Histogram Equalization" << endl;
        info << "Mode: " << (showOriginal ? "ORIGINAL" : "ENHANCED") << endl;
        info << "Clip Limit: " << clipLimit << " (UP/DOWN to adjust)" << endl;
        info << "Tile Grid Size: " << tileGridSize << "x" << tileGridSize << " (LEFT/RIGHT to adjust)" << endl;
        info << "Press SPACE to toggle view" << endl;
        info << "FPS: " << ofToString(ofGetFrameRate(), 1);

        ofDrawBitmapStringHighlight(info.str(), 20, 20);
    } else {
        ofSetColor(255);
        ofDrawBitmapString("Camera not initialized", 20, 20);
    }
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        showOriginal = !showOriginal;
    }
    else if (key == OF_KEY_UP) {
        clipLimit = std::min(clipLimit + 0.5, 10.0);
        clahe->setClipLimit(clipLimit);
        ofLogNotice() << "Clip Limit: " << clipLimit;
    }
    else if (key == OF_KEY_DOWN) {
        clipLimit = std::max(clipLimit - 0.5, 0.5);
        clahe->setClipLimit(clipLimit);
        ofLogNotice() << "Clip Limit: " << clipLimit;
    }
    else if (key == OF_KEY_RIGHT) {
        tileGridSize = std::min(tileGridSize + 1, 32);
        clahe->setTilesGridSize(cv::Size(tileGridSize, tileGridSize));
        ofLogNotice() << "Tile Grid Size: " << tileGridSize << "x" << tileGridSize;
    }
    else if (key == OF_KEY_LEFT) {
        tileGridSize = std::max(tileGridSize - 1, 2);
        clahe->setTilesGridSize(cv::Size(tileGridSize, tileGridSize));
        ofLogNotice() << "Tile Grid Size: " << tileGridSize << "x" << tileGridSize;
    }
}
