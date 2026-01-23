#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    // Load font for displaying decoded text
    font.load(OF_TTF_MONO, 14);

    qrDetected = false;
    decodedText = "";

    ofSetWindowTitle("OpenCV 4.x QR Code Detection");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew()) {
        // Convert to OpenCV Mat
        Mat frame = toCv(cam);

        // Detect and decode QR code
        qrPoints.clear();
        std::string decoded = qrDecoder.detectAndDecode(frame, qrPoints);

        if(!decoded.empty()) {
            decodedText = decoded;
            qrDetected = true;

            // Add to history if it's a new code
            if(history.empty() || history.back() != decoded) {
                history.push_back(decoded);
                if(history.size() > maxHistory) {
                    history.erase(history.begin());
                }
                ofLog() << "QR Code detected: " << decoded;
            }
        } else {
            qrDetected = false;
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    // Draw QR code bounding box if detected
    if(qrDetected && qrPoints.size() == 4) {
        ofSetColor(0, 255, 0);
        ofSetLineWidth(3);
        ofNoFill();

        ofBeginShape();
        for(int i = 0; i < 4; i++) {
            ofVertex(qrPoints[i].x, qrPoints[i].y);
        }
        ofEndShape(true);

        // Draw corner points
        ofFill();
        ofSetColor(255, 0, 0);
        for(int i = 0; i < 4; i++) {
            ofDrawCircle(qrPoints[i].x, qrPoints[i].y, 8);
        }

        // Draw decoded text near QR code
        if(!decodedText.empty()) {
            float centerX = (qrPoints[0].x + qrPoints[2].x) / 2;
            float centerY = (qrPoints[0].y + qrPoints[2].y) / 2;

            ofSetColor(0, 200);
            ofDrawRectangle(centerX - 100, centerY - 20, 200, 30);
            ofSetColor(0, 255, 0);
            ofDrawBitmapString(decodedText.substr(0, 30), centerX - 90, centerY);
        }
    }

    // Draw info panel
    ofSetColor(0, 180);
    ofFill();
    ofDrawRectangle(10, 10, 400, 180);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x QR Code Detection", 20, y); y += 20;
    ofDrawBitmapString("================================", 20, y); y += 25;

    if(qrDetected) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("QR Code DETECTED!", 20, y); y += 20;
        ofSetColor(255, 255, 0);
        // Truncate long URLs
        std::string displayText = decodedText;
        if(displayText.length() > 45) {
            displayText = displayText.substr(0, 42) + "...";
        }
        ofDrawBitmapString("Content: " + displayText, 20, y); y += 25;
    } else {
        ofSetColor(255, 100, 100);
        ofDrawBitmapString("No QR code detected", 20, y); y += 25;
    }

    ofSetColor(200);
    ofDrawBitmapString("History (last 5):", 20, y); y += 18;
    for(int i = history.size() - 1; i >= 0; i--) {
        std::string h = history[i];
        if(h.length() > 40) h = h.substr(0, 37) + "...";
        ofDrawBitmapString("  " + h, 20, y); y += 15;
    }

    // Instructions
    ofSetColor(150);
    ofDrawBitmapString("Show a QR code to the camera", 20, cam.getHeight() - 30);
    ofDrawBitmapString("Press 'c' to clear history", 20, cam.getHeight() - 10);
}

void ofApp::keyPressed(int key) {
    if(key == 'c' || key == 'C') {
        history.clear();
        ofLog() << "History cleared";
    }
}
