#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

void ofApp::setup() {
    ofSetWindowTitle("Background Subtraction - MOG2");
    ofSetFrameRate(60);

    // Initialize camera
    cam.setDeviceID(0);
    cam.setDesiredFrameRate(30);
    cam.initGrabber(640, 480);

    // Create background subtractor (OpenCV 4.x style)
    history = 500;
    varThreshold = 16;
    detectShadows = true;
    bgSubtractor = createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);

    // Morphology parameters
    useMorphology = true;
    morphKernelSize = 3;

    // UI state
    showMask = true;
    showContours = true;
    learningMode = true;
    learningRate = -1;  // -1 = automatic learning rate

    maskImage.allocate(640, 480, OF_IMAGE_GRAYSCALE);
    resultImage.allocate(640, 480, OF_IMAGE_COLOR);

    ofLog() << "Background Subtraction Example";
    ofLog() << "Press SPACE: Toggle learning mode";
    ofLog() << "Press 'm': Toggle mask view";
    ofLog() << "Press 'c': Toggle contours";
    ofLog() << "Press 'n': Toggle morphology";
    ofLog() << "Press '+/-': Adjust threshold";
    ofLog() << "Press 'r': Reset background model";
}

void ofApp::update() {
    cam.update();
    if (!cam.isFrameNew()) {
        return;
    }

    // Convert to cv::Mat
    frame = toCv(cam);

    processBackgroundSubtraction();
}

void ofApp::processBackgroundSubtraction() {
    // Apply background subtraction (OpenCV 4.x style)
    bgSubtractor->apply(frame, fgMask, learningRate);

    // Optional: Remove shadows (set shadow pixels to 0)
    if (detectShadows) {
        // Shadow pixels have value 127, foreground is 255
        threshold(fgMask, fgMask, 200, 255, THRESH_BINARY);
    }

    // Optional: Morphological operations to reduce noise
    if (useMorphology) {
        Mat kernel = getStructuringElement(MORPH_ELLIPSE,
                                           Size(morphKernelSize, morphKernelSize));
        morphologyEx(fgMask, fgMask, MORPH_OPEN, kernel);   // Remove small noise
        morphologyEx(fgMask, fgMask, MORPH_CLOSE, kernel);  // Fill small holes
    }

    // Find contours of moving objects
    contours.clear();
    boundingBoxes.clear();

    findContours(fgMask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Filter contours by area and create bounding boxes
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > 500) {  // Minimum area threshold
            Rect box = boundingRect(contours[i]);
            boundingBoxes.push_back(box);
        }
    }

    // Convert mask to ofImage for display
    toOf(fgMask, maskImage);
    maskImage.update();

    // Create result image with contours
    Mat result = frame.clone();
    if (showContours) {
        for (size_t i = 0; i < boundingBoxes.size(); i++) {
            rectangle(result, boundingBoxes[i], Scalar(0, 255, 0), 2);

            // Draw motion label
            Point pt(boundingBoxes[i].x, boundingBoxes[i].y - 5);
            putText(result, "Motion", pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
        }
    }

    toOf(result, resultImage);
    resultImage.update();
}

void ofApp::draw() {
    ofSetColor(255);

    if (showMask) {
        // Draw mask on left
        maskImage.draw(0, 0);
        ofDrawBitmapStringHighlight("Foreground Mask", 10, 20);
    } else {
        // Draw camera on left
        cam.draw(0, 0);
        ofDrawBitmapStringHighlight("Camera Input", 10, 20);
    }

    // Draw result with contours on right
    resultImage.draw(640, 0);
    ofDrawBitmapStringHighlight("Motion Detection", 650, 20);

    // Draw UI info
    int y = 500;
    ofDrawBitmapStringHighlight("Controls:", 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("SPACE: Learning " + string(learningMode ? "ON" : "OFF"), 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("m: Toggle mask view", 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("c: Contours " + string(showContours ? "ON" : "OFF"), 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("n: Morphology " + string(useMorphology ? "ON" : "OFF"), 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("+/-: Threshold " + ofToString((int)varThreshold), 10, y);
    y += 20;
    ofDrawBitmapStringHighlight("r: Reset model", 10, y);

    y = 500;
    ofDrawBitmapStringHighlight("Stats:", 650, y);
    y += 20;
    ofDrawBitmapStringHighlight("Objects: " + ofToString(boundingBoxes.size()), 650, y);
    y += 20;
    ofDrawBitmapStringHighlight("History: " + ofToString(history), 650, y);
    y += 20;
    ofDrawBitmapStringHighlight("Threshold: " + ofToString((int)varThreshold), 650, y);
    y += 20;
    ofDrawBitmapStringHighlight("Shadows: " + string(detectShadows ? "ON" : "OFF"), 650, y);
    y += 20;
    ofDrawBitmapStringHighlight("FPS: " + ofToString((int)ofGetFrameRate()), 650, y);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        // Toggle learning mode
        learningMode = !learningMode;
        learningRate = learningMode ? -1 : 0;
        ofLog() << "Learning mode: " << (learningMode ? "ON" : "OFF");
    }
    else if (key == 'm') {
        // Toggle mask view
        showMask = !showMask;
    }
    else if (key == 'c') {
        // Toggle contours
        showContours = !showContours;
    }
    else if (key == 'n') {
        // Toggle morphology
        useMorphology = !useMorphology;
    }
    else if (key == '+' || key == '=') {
        // Increase threshold
        varThreshold = min(varThreshold + 2, 100.0);
        bgSubtractor->setVarThreshold(varThreshold);
        ofLog() << "Threshold: " << varThreshold;
    }
    else if (key == '-') {
        // Decrease threshold
        varThreshold = max(varThreshold - 2, 2.0);
        bgSubtractor->setVarThreshold(varThreshold);
        ofLog() << "Threshold: " << varThreshold;
    }
    else if (key == 'r') {
        // Reset background model
        bgSubtractor = createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);
        ofLog() << "Background model reset";
    }
    else if (key == 's') {
        // Toggle shadow detection
        detectShadows = !detectShadows;
        bgSubtractor->setDetectShadows(detectShadows);
        ofLog() << "Shadow detection: " << (detectShadows ? "ON" : "OFF");
    }
}
