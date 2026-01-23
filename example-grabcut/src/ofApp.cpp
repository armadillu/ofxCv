#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetWindowTitle("example-grabcut - GrabCut Foreground Extraction");
    ofSetFrameRate(60);

    drawMode = MODE_RECT;
    bDrawingRect = false;
    bProcessed = false;
    brushSize = 10;

    // Create a test image with distinct foreground and background
    createTestImage();

    // Initialize
    resetMask();

    statusText = "Draw rectangle around object, then press SPACE to extract";
}

void ofApp::createTestImage() {
    // Create a simple test image: colored circle on gradient background
    int w = 640;
    int h = 480;

    cv::Mat testImg(h, w, CV_8UC3);

    // Create gradient background
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float gradR = ofMap(x, 0, w, 50, 100);
            float gradG = ofMap(y, 0, h, 50, 120);
            float gradB = 80;
            testImg.at<cv::Vec3b>(y, x) = cv::Vec3b(gradB, gradG, gradR);
        }
    }

    // Draw colored circles (foreground objects)
    cv::circle(testImg, cv::Point(w/2, h/2), 100, cv::Scalar(200, 100, 50), -1);
    cv::circle(testImg, cv::Point(w/2 + 30, h/2 - 20), 40, cv::Scalar(100, 180, 220), -1);

    image = testImg.clone();

    // Convert to ofImage for display
    img.allocate(w, h, OF_IMAGE_COLOR);
    ofxCv::toOf(image, img);
    img.update();
}

void ofApp::resetMask() {
    if (image.empty()) return;

    mask = cv::Mat(image.size(), CV_8UC1, cv::GC_BGD);
    bgModel.release();
    fgModel.release();
    selection = cv::Rect();
    bProcessed = false;
}

void ofApp::runGrabCut() {
    if (image.empty() || selection.area() == 0) {
        statusText = "Please draw a rectangle around the object first";
        return;
    }

    try {
        // Initialize mask with rectangle
        mask.setTo(cv::GC_BGD);

        // Set rectangle area as probable foreground
        cv::Rect roi = selection & cv::Rect(0, 0, image.cols, image.rows);
        if (roi.area() > 0) {
            mask(roi).setTo(cv::GC_PR_FGD);
        }

        // Run GrabCut algorithm
        // cv::GC_INIT_WITH_RECT: Initialize with rectangle
        // 5 iterations for segmentation refinement
        cv::grabCut(image, mask, selection, bgModel, fgModel, 5, cv::GC_INIT_WITH_RECT);

        // Create result: extract foreground pixels
        result = cv::Mat::zeros(image.size(), CV_8UC3);

        for (int y = 0; y < mask.rows; y++) {
            for (int x = 0; x < mask.cols; x++) {
                uchar val = mask.at<uchar>(y, x);

                // GC_FGD or GC_PR_FGD are foreground
                if (val == cv::GC_FGD || val == cv::GC_PR_FGD) {
                    result.at<cv::Vec3b>(y, x) = image.at<cv::Vec3b>(y, x);
                }
            }
        }

        // Convert to ofTexture
        ofxCv::toOf(result, img);
        img.update();
        resultTexture.allocate(img.getPixels());

        bProcessed = true;
        statusText = "Extraction complete! Press 'r' to reset, 'f'/'b' to refine";

    } catch (cv::Exception& e) {
        statusText = "GrabCut failed: " + string(e.what());
        ofLogError() << statusText;
    }
}

void ofApp::update() {
    // Nothing to update continuously
}

void ofApp::draw() {
    ofBackground(40);

    // Draw original or result image
    ofSetColor(255);
    if (bProcessed && resultTexture.isAllocated()) {
        resultTexture.draw(20, 20);
    } else {
        img.draw(20, 20);

        // Draw selection rectangle
        if (selection.area() > 0) {
            ofPushStyle();
            ofNoFill();
            ofSetColor(0, 255, 0);
            ofSetLineWidth(2);
            ofDrawRectangle(20 + selection.x, 20 + selection.y,
                          selection.width, selection.height);
            ofPopStyle();
        }

        // Draw current rectangle while dragging
        if (bDrawingRect) {
            ofPushStyle();
            ofNoFill();
            ofSetColor(255, 255, 0);
            ofSetLineWidth(1);
            int x1 = 20 + rectStart.x;
            int y1 = 20 + rectStart.y;
            int x2 = ofGetMouseX();
            int y2 = ofGetMouseY();
            ofDrawRectangle(x1, y1, x2 - x1, y2 - y1);
            ofPopStyle();
        }
    }

    // Draw mask visualization (if processed)
    if (bProcessed && !mask.empty()) {
        ofPushMatrix();
        ofTranslate(680, 20);

        // Create colored mask visualization
        cv::Mat maskVis = cv::Mat::zeros(mask.size(), CV_8UC3);
        for (int y = 0; y < mask.rows; y++) {
            for (int x = 0; x < mask.cols; x++) {
                uchar val = mask.at<uchar>(y, x);
                switch (val) {
                    case cv::GC_BGD:    // Definite background (black)
                        maskVis.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                        break;
                    case cv::GC_FGD:    // Definite foreground (white)
                        maskVis.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
                        break;
                    case cv::GC_PR_BGD: // Probable background (dark gray)
                        maskVis.at<cv::Vec3b>(y, x) = cv::Vec3b(64, 64, 64);
                        break;
                    case cv::GC_PR_FGD: // Probable foreground (light gray)
                        maskVis.at<cv::Vec3b>(y, x) = cv::Vec3b(192, 192, 192);
                        break;
                }
            }
        }

        ofImage maskImg;
        ofxCv::toOf(maskVis, maskImg);
        maskImg.update();

        ofSetColor(255);
        maskImg.draw(0, 0);

        ofPopMatrix();

        // Label
        ofSetColor(200);
        ofDrawBitmapString("Mask Visualization", 680, 510);
    }

    // Draw instructions
    ofSetColor(220);
    ofDrawBitmapString(statusText, 20, img.getHeight() + 50);
    ofDrawBitmapString("Controls:", 20, img.getHeight() + 80);
    ofDrawBitmapString("  SPACE: Run GrabCut", 20, img.getHeight() + 100);
    ofDrawBitmapString("  r: Reset", 20, img.getHeight() + 120);
    ofDrawBitmapString("  t: New test image", 20, img.getHeight() + 140);
    ofDrawBitmapString("  Drag mouse: Draw rectangle around object", 20, img.getHeight() + 160);

    // Draw mode info
    string modeStr = "Mode: Rectangle Selection";
    if (drawMode == MODE_FG_DRAW) modeStr = "Mode: Foreground Draw";
    if (drawMode == MODE_BG_DRAW) modeStr = "Mode: Background Draw";
    ofDrawBitmapString(modeStr, 20, img.getHeight() + 190);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        runGrabCut();
    } else if (key == 'r') {
        resetMask();
        createTestImage();
        statusText = "Reset complete. Draw rectangle around object";
    } else if (key == 't') {
        createTestImage();
        resetMask();
        statusText = "New test image created";
    } else if (key == 'f') {
        drawMode = MODE_FG_DRAW;
        statusText = "Mode: Draw foreground (not implemented in basic version)";
    } else if (key == 'b') {
        drawMode = MODE_BG_DRAW;
        statusText = "Mode: Draw background (not implemented in basic version)";
    } else if (key == 'm') {
        drawMode = MODE_RECT;
        statusText = "Mode: Rectangle selection";
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (drawMode == MODE_RECT && !bProcessed) {
        // Start drawing rectangle
        int imgX = x - 20;
        int imgY = y - 20;

        if (imgX >= 0 && imgX < image.cols && imgY >= 0 && imgY < image.rows) {
            bDrawingRect = true;
            rectStart = cv::Point(imgX, imgY);
        }
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    // Rectangle drawing is handled in draw() for visual feedback
}

void ofApp::mouseReleased(int x, int y, int button) {
    if (bDrawingRect) {
        int imgX = x - 20;
        int imgY = y - 20;

        // Clamp to image bounds
        imgX = ofClamp(imgX, 0, image.cols - 1);
        imgY = ofClamp(imgY, 0, image.rows - 1);

        cv::Point rectEnd(imgX, imgY);

        // Create selection rectangle
        int x1 = std::min(rectStart.x, rectEnd.x);
        int y1 = std::min(rectStart.y, rectEnd.y);
        int x2 = std::max(rectStart.x, rectEnd.x);
        int y2 = std::max(rectStart.y, rectEnd.y);

        selection = cv::Rect(x1, y1, x2 - x1, y2 - y1);

        bDrawingRect = false;

        if (selection.area() > 100) {
            statusText = "Rectangle selected. Press SPACE to run GrabCut";
        } else {
            statusText = "Rectangle too small. Try again";
            selection = cv::Rect();
        }
    }
}
