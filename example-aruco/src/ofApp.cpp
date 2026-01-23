#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    // Available ArUco dictionary types
    dictNames = {
        "DICT_4X4_50",
        "DICT_4X4_100",
        "DICT_4X4_250",
        "DICT_5X5_50",
        "DICT_5X5_100",
        "DICT_6X6_50",
        "DICT_6X6_100",
        "APRILTAG_36h11"
    };
    currentDict = 0;

    // Initialize ArUco detector with DICT_4X4_50
    dictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_50);
    detectorParams = aruco::DetectorParameters();
    detector = aruco::ArucoDetector(dictionary, detectorParams);

    showMarkerPreview = true;

    // Generate a preview marker (ID 0) for testing
    Mat markerImage;
    aruco::generateImageMarker(dictionary, 0, 200, markerImage, 1);
    ofPixels pix;
    toOf(markerImage, pix);
    markerPreview.setFromPixels(pix);

    ofSetWindowTitle("OpenCV 4.x ArUco Marker Detection");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew()) {
        Mat frame = toCv(cam);
        Mat gray;
        cvtColor(frame, gray, COLOR_RGB2GRAY);

        // Detect markers
        markerIds.clear();
        markerCorners.clear();
        rejectedCandidates.clear();

        detector.detectMarkers(gray, markerCorners, markerIds, rejectedCandidates);
    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    // Draw detected markers
    for(size_t i = 0; i < markerIds.size(); i++) {
        // Draw marker outline
        ofSetColor(0, 255, 0);
        ofSetLineWidth(3);
        ofNoFill();

        ofBeginShape();
        for(int j = 0; j < 4; j++) {
            ofVertex(markerCorners[i][j].x, markerCorners[i][j].y);
        }
        ofEndShape(true);

        // Draw corner points with different colors
        ofFill();
        ofColor colors[] = {ofColor::red, ofColor::green, ofColor::blue, ofColor::yellow};
        for(int j = 0; j < 4; j++) {
            ofSetColor(colors[j]);
            ofDrawCircle(markerCorners[i][j].x, markerCorners[i][j].y, 8);
        }

        // Draw marker ID at center
        float cx = 0, cy = 0;
        for(int j = 0; j < 4; j++) {
            cx += markerCorners[i][j].x;
            cy += markerCorners[i][j].y;
        }
        cx /= 4; cy /= 4;

        ofSetColor(0, 200);
        ofDrawRectangle(cx - 25, cy - 15, 50, 30);
        ofSetColor(255, 255, 0);
        ofDrawBitmapString("ID:" + ofToString(markerIds[i]), cx - 20, cy + 5);

        // Draw axes to show orientation
        ofSetLineWidth(2);
        Point2f dir1 = markerCorners[i][1] - markerCorners[i][0];
        Point2f dir2 = markerCorners[i][3] - markerCorners[i][0];
        float scale = 0.5;

        ofSetColor(255, 0, 0); // X axis - red
        ofDrawLine(cx, cy, cx + dir1.x * scale, cy + dir1.y * scale);
        ofSetColor(0, 255, 0); // Y axis - green
        ofDrawLine(cx, cy, cx + dir2.x * scale, cy + dir2.y * scale);
    }

    // Draw rejected candidates (faint)
    ofSetColor(255, 0, 0, 100);
    ofSetLineWidth(1);
    ofNoFill();
    for(size_t i = 0; i < rejectedCandidates.size(); i++) {
        ofBeginShape();
        for(int j = 0; j < 4; j++) {
            ofVertex(rejectedCandidates[i][j].x, rejectedCandidates[i][j].y);
        }
        ofEndShape(true);
    }

    // Draw marker preview
    if(showMarkerPreview && markerPreview.isAllocated()) {
        ofSetColor(255);
        float previewSize = 150;
        float px = cam.getWidth() - previewSize - 20;
        float py = cam.getHeight() - previewSize - 20;

        ofSetColor(255);
        ofFill();
        ofDrawRectangle(px - 10, py - 30, previewSize + 20, previewSize + 40);
        ofSetColor(0);
        ofDrawBitmapString("Print this marker!", px, py - 10);
        markerPreview.draw(px, py, previewSize, previewSize);
    }

    // Draw info panel
    ofSetColor(0, 180);
    ofFill();
    ofDrawRectangle(10, 10, 380, 180);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x ArUco Marker Detection", 20, y); y += 20;
    ofDrawBitmapString("==================================", 20, y); y += 25;

    ofSetColor(0, 255, 0);
    ofDrawBitmapString("Markers detected: " + ofToString(markerIds.size()), 20, y); y += 20;

    ofSetColor(255, 255, 0);
    std::string ids = "IDs: ";
    for(size_t i = 0; i < markerIds.size(); i++) {
        ids += ofToString(markerIds[i]);
        if(i < markerIds.size() - 1) ids += ", ";
    }
    ofDrawBitmapString(ids, 20, y); y += 20;

    ofSetColor(200);
    ofDrawBitmapString("Dictionary: " + dictNames[currentDict], 20, y); y += 20;
    ofDrawBitmapString("Rejected candidates: " + ofToString(rejectedCandidates.size()), 20, y); y += 25;

    ofSetColor(150);
    ofDrawBitmapString("[d] Change dictionary  [p] Toggle preview", 20, y); y += 18;
    ofDrawBitmapString("[s] Save marker image", 20, y);
}

void ofApp::keyPressed(int key) {
    if(key == 'd' || key == 'D') {
        // Cycle through dictionaries
        currentDict = (currentDict + 1) % 8;
        cv::aruco::PredefinedDictionaryType dictType;
        switch(currentDict) {
            case 0: dictType = aruco::DICT_4X4_50; break;
            case 1: dictType = aruco::DICT_4X4_100; break;
            case 2: dictType = aruco::DICT_4X4_250; break;
            case 3: dictType = aruco::DICT_5X5_50; break;
            case 4: dictType = aruco::DICT_5X5_100; break;
            case 5: dictType = aruco::DICT_6X6_50; break;
            case 6: dictType = aruco::DICT_6X6_100; break;
            case 7: dictType = aruco::DICT_APRILTAG_36h11; break;
            default: dictType = aruco::DICT_4X4_50; break;
        }
        dictionary = aruco::getPredefinedDictionary(dictType);
        detector = aruco::ArucoDetector(dictionary, detectorParams);

        // Update preview marker
        Mat markerImage;
        aruco::generateImageMarker(dictionary, 0, 200, markerImage, 1);
        ofPixels pix;
        toOf(markerImage, pix);
        markerPreview.setFromPixels(pix);

        ofLog() << "Switched to dictionary: " << dictNames[currentDict];
    }
    else if(key == 'p' || key == 'P') {
        showMarkerPreview = !showMarkerPreview;
    }
    else if(key == 's' || key == 'S') {
        // Save multiple markers
        for(int id = 0; id < 10; id++) {
            Mat markerImage;
            aruco::generateImageMarker(dictionary, id, 400, markerImage, 1);
            ofPixels pix;
            toOf(markerImage, pix);
            ofImage img;
            img.setFromPixels(pix);
            img.save("marker_" + ofToString(id) + ".png");
        }
        ofLog() << "Saved 10 marker images (marker_0.png to marker_9.png)";
    }
}
