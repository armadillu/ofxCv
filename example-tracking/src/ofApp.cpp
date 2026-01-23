#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    isTracking = false;
    isSelecting = false;
    showBackProjection = false;
    trackingTime = 0;

    // Histogram parameters for Hue channel
    histSize = 180;  // Hue ranges 0-180 in OpenCV
    histRange[0] = 0;
    histRange[1] = 180;
    histRanges = histRange;

    ofSetWindowTitle("OpenCV 4.x CAMShift Tracking");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew()) {
        Mat frame = toCv(cam);

        // Convert to HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_RGB2HSV);

        if(isTracking) {
            float startTime = ofGetElapsedTimef();

            // Calculate back projection
            int channels[] = {0};  // Use Hue channel only
            calcBackProject(&hsv, 1, channels, roiHist, backProject, &histRanges);

            // Apply CAMShift
            trackResult = CamShift(backProject, trackingBox,
                TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

            trackingTime = (ofGetElapsedTimef() - startTime) * 1000;

            // Check if tracking is still valid
            if(trackingBox.width <= 0 || trackingBox.height <= 0 ||
               trackingBox.x < 0 || trackingBox.y < 0 ||
               trackingBox.x + trackingBox.width > frame.cols ||
               trackingBox.y + trackingBox.height > frame.rows) {
                // Reset if tracking goes out of bounds
                isTracking = false;
            }
        }
    }
}

void ofApp::draw() {
    // Draw camera or back projection
    if(showBackProjection && !backProject.empty()) {
        ofPixels pix;
        toOf(backProject, pix);
        ofImage img;
        img.setFromPixels(pix);
        ofSetColor(255);
        img.draw(0, 0, cam.getWidth(), cam.getHeight());
    } else {
        ofSetColor(255);
        cam.draw(0, 0);
    }

    // Draw selection rectangle
    if(isSelecting) {
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 255, 0);
        ofDrawRectangle(selectionRect.x, selectionRect.y,
                        selectionRect.width, selectionRect.height);
    }

    // Draw tracking result
    if(isTracking) {
        // Draw rotated rectangle from CAMShift
        Point2f vertices[4];
        trackResult.points(vertices);

        ofSetColor(0, 255, 0);
        ofSetLineWidth(3);
        ofNoFill();

        ofBeginShape();
        for(int i = 0; i < 4; i++) {
            ofVertex(vertices[i].x, vertices[i].y);
        }
        ofEndShape(true);

        // Draw center and orientation
        float cx = trackResult.center.x;
        float cy = trackResult.center.y;

        ofSetColor(255, 0, 0);
        ofFill();
        ofDrawCircle(cx, cy, 5);

        // Draw orientation line
        float angle = trackResult.angle * DEG_TO_RAD;
        float len = max(trackResult.size.width, trackResult.size.height) / 2;
        ofSetColor(255, 255, 0);
        ofSetLineWidth(2);
        ofDrawLine(cx, cy, cx + cos(angle) * len, cy + sin(angle) * len);

        // Draw info
        ofSetColor(0, 200);
        ofFill();
        ofDrawRectangle(trackResult.boundingRect().x, trackResult.boundingRect().y - 25, 150, 22);
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Tracking (" + ofToString(trackingTime, 1) + "ms)",
                           trackResult.boundingRect().x + 5, trackResult.boundingRect().y - 8);
    }

    // Draw info panel
    ofSetColor(0, 180);
    ofFill();
    ofDrawRectangle(10, 10, 400, 200);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x CAMShift Tracking", 20, y); y += 20;
    ofDrawBitmapString("============================", 20, y); y += 25;

    ofSetColor(255, 255, 0);
    ofDrawBitmapString("Algorithm: CAMShift (Mean Shift)", 20, y); y += 20;

    if(isTracking) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Status: Tracking", 20, y); y += 20;
        ofSetColor(200);
        ofDrawBitmapString("Center: (" + ofToString((int)trackResult.center.x) + ", " +
                           ofToString((int)trackResult.center.y) + ")", 20, y); y += 18;
        ofDrawBitmapString("Size: " + ofToString((int)trackResult.size.width) + " x " +
                           ofToString((int)trackResult.size.height), 20, y); y += 18;
        ofDrawBitmapString("Angle: " + ofToString(trackResult.angle, 1) + " deg", 20, y); y += 18;
        ofDrawBitmapString("Tracking time: " + ofToString(trackingTime, 2) + " ms", 20, y); y += 20;
    } else {
        ofSetColor(200);
        ofDrawBitmapString("Status: Not tracking", 20, y); y += 20;
        ofDrawBitmapString("Draw a rectangle on a colored object", 20, y); y += 20;
    }

    y += 10;
    ofSetColor(150);
    ofDrawBitmapString("Mouse drag: Select object to track", 20, y); y += 18;
    ofDrawBitmapString("[b] Toggle back projection view", 20, y); y += 18;
    ofDrawBitmapString("[r] Reset tracking", 20, y);
}

void ofApp::mousePressed(int x, int y, int button) {
    if(button == 0) {
        isSelecting = true;
        selectionStart = Point(x, y);
        selectionRect = Rect(x, y, 0, 0);
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if(isSelecting && button == 0) {
        int minX = min(selectionStart.x, x);
        int minY = min(selectionStart.y, y);
        int maxX = max(selectionStart.x, x);
        int maxY = max(selectionStart.y, y);
        selectionRect = Rect(minX, minY, maxX - minX, maxY - minY);
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    if(isSelecting && button == 0) {
        isSelecting = false;

        if(selectionRect.width > 10 && selectionRect.height > 10) {
            Mat frame = toCv(cam);

            // Clamp selection to frame bounds
            selectionRect &= Rect(0, 0, frame.cols, frame.rows);

            if(selectionRect.width > 0 && selectionRect.height > 0) {
                // Convert to HSV
                Mat hsv;
                cvtColor(frame, hsv, COLOR_RGB2HSV);

                // Extract ROI
                Mat roi(hsv, selectionRect);
                Mat mask;

                // Create mask for skin-like colors (optional, can be removed)
                // inRange(roi, Scalar(0, 60, 32), Scalar(180, 255, 255), mask);

                // Calculate histogram of Hue channel
                int channels[] = {0};
                int histSizes[] = {histSize};
                calcHist(&roi, 1, channels, Mat(), roiHist, 1, histSizes, &histRanges);

                // Normalize histogram
                normalize(roiHist, roiHist, 0, 255, NORM_MINMAX);

                trackingBox = selectionRect;
                isTracking = true;

                ofLog() << "Started tracking at (" << selectionRect.x << ", " << selectionRect.y
                        << ") size: " << selectionRect.width << "x" << selectionRect.height;
            }
        }
    }
}

void ofApp::keyPressed(int key) {
    if(key == 'b' || key == 'B') {
        showBackProjection = !showBackProjection;
    }
    else if(key == 'r' || key == 'R') {
        isTracking = false;
        ofLog() << "Tracking reset";
    }
}
