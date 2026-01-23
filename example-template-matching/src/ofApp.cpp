#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Template Matching - ofxCv");

    camera.setup(640, 480);
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew()) {
        frame = toCv(camera);

        if (hasTemplate && !templ.empty()) {
            performMatching();
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    camera.draw(0, 0);

    // Draw capture rectangle
    if (capturing) {
        ofNoFill();
        ofSetColor(0, 255, 0);
        ofDrawRectangle(captureStart.x, captureStart.y,
                       captureEnd.x - captureStart.x,
                       captureEnd.y - captureStart.y);
    }

    // Draw match result
    if (hasTemplate && matchVal >= threshold) {
        ofNoFill();
        ofSetColor(255, 0, 0, 180);
        ofSetLineWidth(3);
        ofDrawRectangle(matchLoc.x, matchLoc.y, templ.cols, templ.rows);

        // Draw crosshair at center
        int cx = matchLoc.x + templ.cols / 2;
        int cy = matchLoc.y + templ.rows / 2;
        ofDrawLine(cx - 10, cy, cx + 10, cy);
        ofDrawLine(cx, cy - 10, cx, cy + 10);
    }

    // Draw template thumbnail
    if (hasTemplate && !templ.empty()) {
        ofPushMatrix();
        ofTranslate(10, 500);

        // Draw background
        ofFill();
        ofSetColor(0, 180);
        ofDrawRectangle(-5, -5, templ.cols + 10, templ.rows + 10);

        ofSetColor(255);
        drawMat(templ, 0, 0);

        ofPopMatrix();

        // Template info
        ofSetColor(255);
        ofDrawBitmapString("Template: " + ofToString(templ.cols) + "x" + ofToString(templ.rows),
                          10, 500 + templ.rows + 15);
    }

    drawUI();
}

void ofApp::performMatching() {
    if (frame.empty() || templ.empty()) return;
    if (frame.cols < templ.cols || frame.rows < templ.rows) return;

    // Create result matrix
    int resultCols = frame.cols - templ.cols + 1;
    int resultRows = frame.rows - templ.rows + 1;
    result.create(resultRows, resultCols, CV_32FC1);

    // Perform template matching
    cv::matchTemplate(frame, templ, result, matchMethod);

    // Find best match location
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // For SQDIFF methods, minimum is best match
    if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
        matchLoc = minLoc;
        matchVal = 1.0f - minVal;
    } else {
        matchLoc = maxLoc;
        matchVal = maxVal;
    }
}

void ofApp::captureTemplate(cv::Point start, cv::Point end) {
    if (frame.empty()) return;

    // Ensure valid rectangle
    int x = std::min(start.x, end.x);
    int y = std::min(start.y, end.y);
    int w = std::abs(end.x - start.x);
    int h = std::abs(end.y - start.y);

    if (w < 10 || h < 10) return;

    // Clamp to frame bounds
    x = std::max(0, std::min(x, frame.cols - 1));
    y = std::max(0, std::min(y, frame.rows - 1));
    w = std::min(w, frame.cols - x);
    h = std::min(h, frame.rows - y);

    if (w > 0 && h > 0) {
        cv::Rect roi(x, y, w, h);
        templ = frame(roi).clone();
        hasTemplate = true;

        ofLogNotice() << "Template captured: " << w << "x" << h;
    }
}

void ofApp::drawUI() {
    ofSetColor(0, 200);
    ofDrawRectangle(640, 0, 240, 480);

    ofSetColor(255);
    int yPos = 20;
    int yStep = 20;

    ofDrawBitmapString("Template Matching", 650, yPos);
    yPos += yStep * 2;

    ofDrawBitmapString("CONTROLS:", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("Click & drag: Capture template", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("SPACE: Clear template", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("1-6: Change method", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("T/t: Adjust threshold", 650, yPos);
    yPos += yStep * 2;

    ofDrawBitmapString("STATUS:", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("Template: " + string(hasTemplate ? "YES" : "NO"), 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("Match value: " + ofToString(matchVal, 3), 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("Threshold: " + ofToString(threshold, 2), 650, yPos);
    yPos += yStep * 2;

    ofDrawBitmapString("METHOD:", 650, yPos);
    yPos += yStep;

    string methodName;
    switch(matchMethod) {
        case cv::TM_SQDIFF: methodName = "SQDIFF"; break;
        case cv::TM_SQDIFF_NORMED: methodName = "SQDIFF_NORMED"; break;
        case cv::TM_CCORR: methodName = "CCORR"; break;
        case cv::TM_CCORR_NORMED: methodName = "CCORR_NORMED"; break;
        case cv::TM_CCOEFF: methodName = "CCOEFF"; break;
        case cv::TM_CCOEFF_NORMED: methodName = "CCOEFF_NORMED"; break;
        default: methodName = "UNKNOWN"; break;
    }
    ofDrawBitmapString("> " + methodName, 650, yPos);
    yPos += yStep;

    ofDrawBitmapString("[1] SQDIFF", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("[2] SQDIFF_NORMED", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("[3] CCORR", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("[4] CCORR_NORMED", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("[5] CCOEFF", 650, yPos);
    yPos += yStep;
    ofDrawBitmapString("[6] CCOEFF_NORMED", 650, yPos);
    yPos += yStep * 2;

    if (hasTemplate && matchVal >= threshold) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("MATCH FOUND!", 650, yPos);
    } else if (hasTemplate) {
        ofSetColor(255, 150, 0);
        ofDrawBitmapString("NO MATCH", 650, yPos);
    }
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        hasTemplate = false;
        templ.release();
        ofLogNotice() << "Template cleared";
    } else if (key == '1') {
        matchMethod = cv::TM_SQDIFF;
    } else if (key == '2') {
        matchMethod = cv::TM_SQDIFF_NORMED;
    } else if (key == '3') {
        matchMethod = cv::TM_CCORR;
    } else if (key == '4') {
        matchMethod = cv::TM_CCORR_NORMED;
    } else if (key == '5') {
        matchMethod = cv::TM_CCOEFF;
    } else if (key == '6') {
        matchMethod = cv::TM_CCOEFF_NORMED;
    } else if (key == 'T') {
        threshold = std::min(1.0f, threshold + 0.05f);
    } else if (key == 't') {
        threshold = std::max(0.0f, threshold - 0.05f);
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (x < 640) {
        capturing = true;
        captureStart = cv::Point(x, y);
        captureEnd = captureStart;
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if (capturing && x < 640) {
        captureEnd = cv::Point(x, y);
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    if (capturing) {
        capturing = false;
        captureTemplate(captureStart, captureEnd);
    }
}
