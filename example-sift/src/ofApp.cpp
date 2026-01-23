#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    maxFeatures = 500;
    ratioThreshold = 0.7f;
    showAllKeypoints = true;
    hasReference = false;
    homographyFound = false;
    detectionTime = 0;
    matchingTime = 0;

    // Create SIFT detector (now free in OpenCV 4.x!)
    sift = SIFT::create(maxFeatures);

    // Create brute-force matcher with L2 norm (for SIFT)
    matcher = BFMatcher::create(NORM_L2, false);

    ofSetWindowTitle("OpenCV 4.x SIFT Feature Matching");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew()) {
        Mat frame = toCv(cam);
        Mat gray;
        cvtColor(frame, gray, COLOR_RGB2GRAY);

        // Detect keypoints and compute descriptors
        float startTime = ofGetElapsedTimef();
        currentKeypoints.clear();
        sift->detectAndCompute(gray, noArray(), currentKeypoints, currentDescriptors);
        detectionTime = (ofGetElapsedTimef() - startTime) * 1000;

        // Match with reference if available
        if(hasReference && !referenceDescriptors.empty() && !currentDescriptors.empty()) {
            startTime = ofGetElapsedTimef();

            // Find matches using kNN
            std::vector<std::vector<DMatch>> knnMatches;
            matcher->knnMatch(referenceDescriptors, currentDescriptors, knnMatches, 2);

            // Apply Lowe's ratio test
            goodMatches.clear();
            for(size_t i = 0; i < knnMatches.size(); i++) {
                if(knnMatches[i].size() >= 2 &&
                   knnMatches[i][0].distance < ratioThreshold * knnMatches[i][1].distance) {
                    goodMatches.push_back(knnMatches[i][0]);
                }
            }

            matchingTime = (ofGetElapsedTimef() - startTime) * 1000;

            // Find homography if enough matches
            homographyFound = false;
            if(goodMatches.size() >= 4) {
                std::vector<Point2f> refPts, scenePts;
                for(size_t i = 0; i < goodMatches.size(); i++) {
                    refPts.push_back(referenceKeypoints[goodMatches[i].queryIdx].pt);
                    scenePts.push_back(currentKeypoints[goodMatches[i].trainIdx].pt);
                }

                homography = findHomography(refPts, scenePts, RANSAC, 5.0);

                if(!homography.empty()) {
                    // Transform reference corners to scene
                    refCorners.clear();
                    refCorners.push_back(Point2f(0, 0));
                    refCorners.push_back(Point2f(referenceImg.cols, 0));
                    refCorners.push_back(Point2f(referenceImg.cols, referenceImg.rows));
                    refCorners.push_back(Point2f(0, referenceImg.rows));

                    sceneCorners.clear();
                    perspectiveTransform(refCorners, sceneCorners, homography);
                    homographyFound = true;
                }
            }
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    // Draw all keypoints
    if(showAllKeypoints) {
        ofSetColor(0, 255, 255, 100);
        for(const auto& kp : currentKeypoints) {
            ofDrawCircle(kp.pt.x, kp.pt.y, kp.size / 4);
        }
    }

    // Draw matches and homography if reference exists
    if(hasReference) {
        // Draw matched keypoints
        ofSetColor(0, 255, 0);
        for(const auto& match : goodMatches) {
            Point2f pt = currentKeypoints[match.trainIdx].pt;
            ofDrawCircle(pt.x, pt.y, 5);
        }

        // Draw homography outline
        if(homographyFound && sceneCorners.size() == 4) {
            ofSetColor(255, 0, 255);
            ofSetLineWidth(4);
            ofNoFill();
            ofBeginShape();
            for(int i = 0; i < 4; i++) {
                ofVertex(sceneCorners[i].x, sceneCorners[i].y);
            }
            ofEndShape(true);

            // Draw corner labels
            ofFill();
            std::string labels[] = {"TL", "TR", "BR", "BL"};
            for(int i = 0; i < 4; i++) {
                ofSetColor(255, 0, 255);
                ofDrawCircle(sceneCorners[i].x, sceneCorners[i].y, 8);
                ofSetColor(255);
                ofDrawBitmapString(labels[i], sceneCorners[i].x + 10, sceneCorners[i].y);
            }
        }

        // Draw reference image thumbnail
        ofSetColor(255);
        float thumbScale = 0.2;
        float thumbW = referenceImg.cols * thumbScale;
        float thumbH = referenceImg.rows * thumbScale;
        float thumbX = cam.getWidth() - thumbW - 10;
        float thumbY = cam.getHeight() - thumbH - 10;

        ofSetColor(0);
        ofDrawRectangle(thumbX - 2, thumbY - 20, thumbW + 4, thumbH + 22);
        ofSetColor(255);
        ofDrawBitmapString("Reference", thumbX, thumbY - 5);

        ofPixels pix;
        toOf(referenceImg, pix);
        ofImage refImg;
        refImg.setFromPixels(pix);
        refImg.draw(thumbX, thumbY, thumbW, thumbH);
    }

    // Draw info panel
    ofSetColor(0, 180);
    ofFill();
    ofDrawRectangle(10, 10, 400, 220);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x SIFT Feature Matching", 20, y); y += 20;
    ofDrawBitmapString("=================================", 20, y); y += 25;

    ofSetColor(0, 255, 255);
    ofDrawBitmapString("Keypoints detected: " + ofToString(currentKeypoints.size()), 20, y); y += 20;

    if(hasReference) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Good matches: " + ofToString(goodMatches.size()), 20, y); y += 20;

        if(homographyFound) {
            ofSetColor(255, 0, 255);
            ofDrawBitmapString("Homography: FOUND", 20, y); y += 20;
        } else {
            ofSetColor(255, 100, 100);
            ofDrawBitmapString("Homography: Not found (need more matches)", 20, y); y += 20;
        }

        ofSetColor(200);
        ofDrawBitmapString("Reference keypoints: " + ofToString(referenceKeypoints.size()), 20, y); y += 20;
    } else {
        ofSetColor(255, 255, 0);
        ofDrawBitmapString("No reference - Press 'c' to capture", 20, y); y += 20;
    }

    ofSetColor(200);
    ofDrawBitmapString("Detection time: " + ofToString(detectionTime, 1) + " ms", 20, y); y += 18;
    if(hasReference) {
        ofDrawBitmapString("Matching time: " + ofToString(matchingTime, 1) + " ms", 20, y); y += 18;
    }

    y += 10;
    ofSetColor(150);
    ofDrawBitmapString("[c] Capture reference  [r] Reset", 20, y); y += 18;
    ofDrawBitmapString("[k] Toggle keypoint display", 20, y); y += 18;
    ofDrawBitmapString("[+/-] Adjust ratio threshold: " + ofToString(ratioThreshold, 2), 20, y);
}

void ofApp::keyPressed(int key) {
    if(key == 'c' || key == 'C') {
        // Capture current frame as reference
        Mat frame = toCv(cam);
        cvtColor(frame, referenceImg, COLOR_RGB2GRAY);

        referenceKeypoints.clear();
        sift->detectAndCompute(referenceImg, noArray(), referenceKeypoints, referenceDescriptors);
        hasReference = true;

        ofLog() << "Captured reference with " << referenceKeypoints.size() << " keypoints";
    }
    else if(key == 'r' || key == 'R') {
        hasReference = false;
        homographyFound = false;
        goodMatches.clear();
        ofLog() << "Reference cleared";
    }
    else if(key == 'k' || key == 'K') {
        showAllKeypoints = !showAllKeypoints;
    }
    else if(key == '+' || key == '=') {
        ratioThreshold = std::min(0.95f, ratioThreshold + 0.05f);
    }
    else if(key == '-' || key == '_') {
        ratioThreshold = std::max(0.3f, ratioThreshold - 0.05f);
    }
}
