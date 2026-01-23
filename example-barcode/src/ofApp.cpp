#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Barcode Detection");

    // Initialize barcode detector
    try {
        barcodeDetector = cv::barcode::BarcodeDetector();
        statusMessage = "Barcode detector initialized";
    } catch (const cv::Exception& e) {
        statusMessage = "Error: " + std::string(e.what());
        ofLogError() << statusMessage;
    }

    // Try to initialize camera
    camera.setDesiredFrameRate(30);
    camera.setup(640, 480);

    useTestImage = !camera.isInitialized();

    if (useTestImage) {
        // Create test pattern with QR code simulation
        testImage.allocate(640, 480, OF_IMAGE_COLOR);
        testImage.getPixels().set(255);

        // Draw a simple barcode-like pattern
        for(int x = 200; x < 440; x += 10) {
            int width = (x % 20 == 0) ? 5 : 3;
            ofPixels& pix = testImage.getPixels();
            for(int y = 200; y < 280; y++) {
                for(int dx = 0; dx < width; dx++) {
                    if(x + dx < 440) {
                        pix.setColor(x + dx, y, ofColor(0));
                    }
                }
            }
        }
        testImage.update();

        statusMessage = "Using test pattern (no camera found)";
        ofLogWarning() << "Camera not available, using test pattern";
    } else {
        statusMessage = "Camera initialized - show barcode/QR code";
    }

    lastDetectionTime = 0;
}

void ofApp::update() {
    if (!useTestImage && camera.isInitialized()) {
        camera.update();

        if (camera.isFrameNew()) {
            cameraPixels = camera.getPixels();
            cv::Mat frame = toCv(cameraPixels);
            detectBarcodes(frame);
            cameraTexture.loadData(cameraPixels);
        }
    } else if (useTestImage) {
        // Process test image once per second
        if (ofGetElapsedTimef() - lastDetectionTime > 1.0f) {
            cv::Mat frame = toCv(testImage);
            detectBarcodes(frame);
            lastDetectionTime = ofGetElapsedTimef();
        }
    }
}

void ofApp::detectBarcodes(cv::Mat& frame) {
    try {
        // Convert to grayscale for better detection
        cv::Mat gray;
        if (frame.channels() == 3) {
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = frame.clone();
        }

        // Detect and decode barcodes
        cv::Mat detectedCorners;
        std::vector<std::string> detectedInfo;
        std::vector<std::string> detectedTypes;

        bool detected = barcodeDetector.detectAndDecodeWithType(
            gray,
            detectedInfo,
            detectedTypes,
            detectedCorners
        );

        if (detected && !detectedInfo.empty()) {
            // Convert corners format from Mat to vector
            corners.clear();
            if (!detectedCorners.empty()) {
                // Corners are in format [N][4] where N is number of barcodes
                for (int i = 0; i < detectedCorners.rows; i++) {
                    for (int j = 0; j < detectedCorners.cols; j++) {
                        cv::Point2f pt = detectedCorners.at<cv::Point2f>(i, j);
                        corners.push_back(pt);
                    }
                }
            }

            decodedInfo = detectedInfo;
            decodedTypes = detectedTypes;
            lastDetectionTime = ofGetElapsedTimef();

            statusMessage = "Detected " + ofToString(decodedInfo.size()) + " barcode(s)";
        } else {
            // Clear old detections after 2 seconds
            if (ofGetElapsedTimef() - lastDetectionTime > 2.0f) {
                corners.clear();
                decodedInfo.clear();
                decodedTypes.clear();
            }
        }

    } catch (const cv::Exception& e) {
        ofLogError() << "Detection error: " << e.what();
        statusMessage = "Detection error: " + std::string(e.what());
    }
}

void ofApp::draw() {
    ofBackground(50);

    // Draw camera or test image
    if (useTestImage) {
        ofSetColor(255);
        testImage.draw(0, 0);
    } else if (cameraTexture.isAllocated()) {
        ofSetColor(255);
        cameraTexture.draw(0, 0);
    }

    // Draw detected barcodes
    if (!corners.empty() && !decodedInfo.empty()) {
        int numBarcodes = decodedInfo.size();

        for (int i = 0; i < numBarcodes; i++) {
            // Draw polygon around barcode
            int cornersPerBarcode = corners.size() / numBarcodes;
            int startIdx = i * cornersPerBarcode;

            ofSetColor(0, 255, 0, 200);
            ofSetLineWidth(3);
            ofNoFill();
            ofBeginShape();
            for (int j = 0; j < cornersPerBarcode && startIdx + j < corners.size(); j++) {
                ofVertex(corners[startIdx + j].x, corners[startIdx + j].y);
            }
            ofEndShape(true);

            // Draw barcode info
            if (i < decodedInfo.size() && startIdx < corners.size()) {
                ofSetColor(255, 255, 0);
                std::string typeStr = (i < decodedTypes.size()) ?
                    decodedTypes[i] : "UNKNOWN";
                std::string info = typeStr + ": " + decodedInfo[i];
                ofDrawBitmapString(info, corners[startIdx].x, corners[startIdx].y - 10);
            }
        }
    }

    // Draw status message
    ofSetColor(255);
    ofDrawBitmapString("Barcode Detection - OpenCV BarcodeDetector", 10, 20);
    ofDrawBitmapString(statusMessage, 10, 40);
    ofDrawBitmapString("Press 'r' to reset detection", 10, 60);
    ofDrawBitmapString("Supports: EAN-8, EAN-13, UPC-A, UPC-E", 10, 80);

    if (!decodedInfo.empty()) {
        ofDrawBitmapString("Detected:", 10, 110);
        for (size_t i = 0; i < decodedInfo.size() && i < 5; i++) {
            std::string typeStr = (i < decodedTypes.size()) ?
                decodedTypes[i] : "UNKNOWN";
            ofDrawBitmapString("  " + typeStr + ": " + decodedInfo[i], 10, 130 + i * 20);
        }
    }

    // Draw FPS
    ofSetColor(255, 255, 0);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate(), 1),
                      ofGetWidth() - 100, 20);
}

void ofApp::keyPressed(int key) {
    if (key == 'r') {
        corners.clear();
        decodedInfo.clear();
        decodedTypes.clear();
        statusMessage = "Detection reset";
    }
}
