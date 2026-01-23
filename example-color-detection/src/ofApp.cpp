#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    // Setup camera
    cam.setup(640, 480);

    // Default to red color detection
    currentPreset = RED;
    hueMin = 0;
    hueMax = 10;
    satMin = 100;
    satMax = 255;
    valMin = 100;
    valMax = 255;
    presetName = "Red";

    showMask = false;
    showContours = true;
}

void ofApp::update() {
    cam.update();

    if (cam.isFrameNew()) {
        frame = toCv(cam);

        if (!frame.empty()) {
            // Convert to HSV color space
            cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

            // Detect color
            detectColor();
        }
    }
}

void ofApp::detectColor() {
    // Create mask for color range
    cv::Scalar lower(hueMin, satMin, valMin);
    cv::Scalar upper(hueMax, satMax, valMax);
    cv::inRange(hsv, lower, upper, mask);

    // Special handling for red (wraps around hue 180)
    if (currentPreset == RED) {
        cv::Mat mask2;
        cv::Scalar lower2(170, satMin, valMin);
        cv::Scalar upper2(180, satMax, valMax);
        cv::inRange(hsv, lower2, upper2, mask2);
        cv::bitwise_or(mask, mask2, mask);
    }

    // Morphological operations to reduce noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // Find contours
    contours.clear();
    boundingBoxes.clear();

    cv::findContours(mask.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Filter contours by area and get bounding boxes
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > 500) {  // Minimum area threshold
            cv::Rect box = cv::boundingRect(contours[i]);
            boundingBoxes.push_back(box);
        }
    }

    // Convert mask to ofImage for display
    toOf(mask, resultImage);
    resultImage.update();
}

void ofApp::draw() {
    ofSetColor(255);

    if (showMask) {
        // Show binary mask
        resultImage.draw(0, 0);
    } else {
        // Show original camera feed
        cam.draw(0, 0);

        // Draw detected regions
        if (showContours) {
            ofPushStyle();
            ofNoFill();
            ofSetColor(0, 255, 0);
            ofSetLineWidth(2);

            for (const auto& box : boundingBoxes) {
                ofDrawRectangle(box.x, box.y, box.width, box.height);

                // Draw center point
                int cx = box.x + box.width / 2;
                int cy = box.y + box.height / 2;
                ofDrawCircle(cx, cy, 3);
            }

            ofPopStyle();
        }
    }

    // Draw UI
    ofPushStyle();
    ofSetColor(0, 0, 0, 200);
    ofDrawRectangle(0, 0, 250, 200);

    ofSetColor(255);
    stringstream info;
    info << "FPS: " << ofGetFrameRate() << endl;
    info << "Color Preset: " << presetName << endl;
    info << "Detected: " << boundingBoxes.size() << " objects" << endl << endl;
    info << "HSV Range:" << endl;
    info << "  Hue: " << hueMin << " - " << hueMax << endl;
    info << "  Sat: " << satMin << " - " << satMax << endl;
    info << "  Val: " << valMin << " - " << valMax << endl << endl;
    info << "Keys:" << endl;
    info << "  1-4: Color presets" << endl;
    info << "  m: Toggle mask view" << endl;
    info << "  c: Toggle contours" << endl;

    ofDrawBitmapString(info.str(), 10, 20);
    ofPopStyle();
}

void ofApp::updateColorRange() {
    switch (currentPreset) {
        case RED:
            hueMin = 0;
            hueMax = 10;
            satMin = 100;
            satMax = 255;
            valMin = 100;
            valMax = 255;
            presetName = "Red";
            break;

        case GREEN:
            hueMin = 40;
            hueMax = 80;
            satMin = 50;
            satMax = 255;
            valMin = 50;
            valMax = 255;
            presetName = "Green";
            break;

        case BLUE:
            hueMin = 100;
            hueMax = 130;
            satMin = 50;
            satMax = 255;
            valMin = 50;
            valMax = 255;
            presetName = "Blue";
            break;

        case YELLOW:
            hueMin = 20;
            hueMax = 40;
            satMin = 100;
            satMax = 255;
            valMin = 100;
            valMax = 255;
            presetName = "Yellow";
            break;

        case CUSTOM:
            presetName = "Custom";
            break;
    }
}

void ofApp::keyPressed(int key) {
    switch (key) {
        case '1':
            currentPreset = RED;
            updateColorRange();
            break;
        case '2':
            currentPreset = GREEN;
            updateColorRange();
            break;
        case '3':
            currentPreset = BLUE;
            updateColorRange();
            break;
        case '4':
            currentPreset = YELLOW;
            updateColorRange();
            break;
        case 'm':
            showMask = !showMask;
            break;
        case 'c':
            showContours = !showContours;
            break;
    }
}
