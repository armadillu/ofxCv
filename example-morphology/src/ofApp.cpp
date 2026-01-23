#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Morphological Operations - ofxCv");

    // Initialize camera
    cam.setup(640, 480);

    // Initialize parameters
    morphOp = cv::MORPH_ERODE;
    kernelSize = 5;
    kernelShape = cv::MORPH_RECT;
    iterations = 1;

    // Allocate images
    original.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
    binary.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_GRAYSCALE);
    result.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_GRAYSCALE);

    ofLog() << "Morphological Operations Sample";
    ofLog() << "Controls:";
    ofLog() << "  1-7: Select operation (Erode/Dilate/Open/Close/Gradient/TopHat/BlackHat)";
    ofLog() << "  q/w: Decrease/Increase kernel size";
    ofLog() << "  a/s: Decrease/Increase iterations";
    ofLog() << "  z/x: Cycle kernel shape (Rect/Ellipse/Cross)";
}

string ofApp::getOperationName() {
    switch (morphOp) {
        case cv::MORPH_ERODE:    return "Erode";
        case cv::MORPH_DILATE:   return "Dilate";
        case cv::MORPH_OPEN:     return "Open";
        case cv::MORPH_CLOSE:    return "Close";
        case cv::MORPH_GRADIENT: return "Gradient";
        case cv::MORPH_TOPHAT:   return "Top Hat";
        case cv::MORPH_BLACKHAT: return "Black Hat";
        default:                 return "Unknown";
    }
}

string ofApp::getShapeName() {
    switch (kernelShape) {
        case cv::MORPH_RECT:    return "Rectangle";
        case cv::MORPH_ELLIPSE: return "Ellipse";
        case cv::MORPH_CROSS:   return "Cross";
        default:                return "Unknown";
    }
}

void ofApp::applyMorphology() {
    // Convert to grayscale
    Mat gray = toCv(original);
    cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

    // Apply threshold to create binary image
    Mat binaryMat;
    threshold(gray, binaryMat, 128, 255, cv::THRESH_BINARY);
    toOf(binaryMat, binary.getPixels());
    binary.update();

    // Create structuring element
    Mat element = getStructuringElement(
        kernelShape,
        Size(kernelSize, kernelSize)
    );

    // Apply morphological operation
    Mat resultMat = toCv(result);
    try {
        cv::morphologyEx(binaryMat, resultMat, morphOp, element, Point(-1, -1), iterations);
        result.update();
    } catch (cv::Exception& e) {
        ofLogWarning() << "Morphology error: " << e.what();
    }
}

void ofApp::update() {
    cam.update();

    if (cam.isFrameNew()) {
        // Copy camera frame to original image
        original.setFromPixels(cam.getPixels());

        // Apply morphological operations
        applyMorphology();
    }
}

void ofApp::draw() {
    ofBackground(40);

    // Draw original
    ofSetColor(255);
    ofDrawBitmapStringHighlight("Original", 10, 15);
    original.draw(0, 30, 426, 320);

    // Draw binary (thresholded)
    ofDrawBitmapStringHighlight("Binary (Threshold)", 436, 15);
    binary.draw(426, 30, 426, 320);

    // Draw morphology result
    ofDrawBitmapStringHighlight("Morphology Result", 862, 15);
    result.draw(852, 30, 426, 320);

    // Draw parameters
    ofDrawBitmapStringHighlight(
        "Parameters:\n" +
        ofToString("Operation: ") + getOperationName() + " (keys 1-7)\n" +
        ofToString("Kernel Size: ") + ofToString(kernelSize) + "x" + ofToString(kernelSize) + " (keys q/w)\n" +
        ofToString("Kernel Shape: ") + getShapeName() + " (keys z/x)\n" +
        ofToString("Iterations: ") + ofToString(iterations) + " (keys a/s)",
        10, 370
    );

    // Draw operation descriptions
    ofDrawBitmapStringHighlight(
        "Operations:\n"
        "1. Erode - Shrinks bright regions\n"
        "2. Dilate - Expands bright regions\n"
        "3. Open - Erode then Dilate (removes noise)\n"
        "4. Close - Dilate then Erode (fills holes)\n"
        "5. Gradient - Dilate minus Erode (edge detection)\n"
        "6. Top Hat - Original minus Open (bright details)\n"
        "7. Black Hat - Close minus Original (dark details)",
        10, 490
    );
}

void ofApp::keyPressed(int key) {
    // Select operation
    if (key == '1') {
        morphOp = cv::MORPH_ERODE;
        ofLog() << "Operation: Erode";
    }
    else if (key == '2') {
        morphOp = cv::MORPH_DILATE;
        ofLog() << "Operation: Dilate";
    }
    else if (key == '3') {
        morphOp = cv::MORPH_OPEN;
        ofLog() << "Operation: Open";
    }
    else if (key == '4') {
        morphOp = cv::MORPH_CLOSE;
        ofLog() << "Operation: Close";
    }
    else if (key == '5') {
        morphOp = cv::MORPH_GRADIENT;
        ofLog() << "Operation: Gradient";
    }
    else if (key == '6') {
        morphOp = cv::MORPH_TOPHAT;
        ofLog() << "Operation: Top Hat";
    }
    else if (key == '7') {
        morphOp = cv::MORPH_BLACKHAT;
        ofLog() << "Operation: Black Hat";
    }
    // Kernel size
    else if (key == 'q') {
        kernelSize = max(3, kernelSize - 2);
        ofLog() << "Kernel size: " << kernelSize;
    }
    else if (key == 'w') {
        kernelSize = min(21, kernelSize + 2);
        ofLog() << "Kernel size: " << kernelSize;
    }
    // Iterations
    else if (key == 'a') {
        iterations = max(1, iterations - 1);
        ofLog() << "Iterations: " << iterations;
    }
    else if (key == 's') {
        iterations = min(10, iterations + 1);
        ofLog() << "Iterations: " << iterations;
    }
    // Kernel shape
    else if (key == 'z') {
        kernelShape = (kernelShape == cv::MORPH_RECT) ? cv::MORPH_CROSS :
                      (kernelShape == cv::MORPH_CROSS) ? cv::MORPH_ELLIPSE : cv::MORPH_RECT;
        ofLog() << "Kernel shape: " << getShapeName();
    }
    else if (key == 'x') {
        kernelShape = (kernelShape == cv::MORPH_RECT) ? cv::MORPH_ELLIPSE :
                      (kernelShape == cv::MORPH_ELLIPSE) ? cv::MORPH_CROSS : cv::MORPH_RECT;
        ofLog() << "Kernel shape: " << getShapeName();
    }
}
