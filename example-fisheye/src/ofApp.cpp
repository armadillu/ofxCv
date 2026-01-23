#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Fisheye Lens Correction - ofxCv");

    // Initialize camera
    cam.setup(640, 480);

    // Initialize fisheye distortion parameters
    // These simulate a fisheye lens or can be adjusted to real calibration values
    k1 = -0.3;   // Radial distortion k1
    k2 = 0.1;    // Radial distortion k2
    k3 = -0.05;  // Radial distortion k3
    k4 = 0.01;   // Radial distortion k4

    // Camera matrix (intrinsics)
    fx = cam.getWidth() * 0.8;  // Focal length X
    fy = cam.getWidth() * 0.8;  // Focal length Y
    cx = cam.getWidth() / 2.0;   // Principal point X
    cy = cam.getHeight() / 2.0;  // Principal point Y

    applyUndistortion = true;

    // Allocate images
    original.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
    undistorted.allocate(cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);

    updateCalibration();

    ofLog() << "Fisheye Correction Sample";
    ofLog() << "Controls:";
    ofLog() << "  SPACE: Toggle undistortion on/off";
    ofLog() << "  1/2: Decrease/Increase k1";
    ofLog() << "  3/4: Decrease/Increase k2";
    ofLog() << "  5/6: Decrease/Increase k3";
    ofLog() << "  7/8: Decrease/Increase k4";
    ofLog() << "  q/w: Decrease/Increase focal length";
}

void ofApp::updateCalibration() {
    // Create camera matrix (intrinsic parameters)
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    cameraMatrix.at<double>(0, 0) = fx;  // fx
    cameraMatrix.at<double>(1, 1) = fy;  // fy
    cameraMatrix.at<double>(0, 2) = cx;  // cx
    cameraMatrix.at<double>(1, 2) = cy;  // cy

    // Create distortion coefficients for fisheye model
    distCoeffs = Mat::zeros(4, 1, CV_64F);
    distCoeffs.at<double>(0) = k1;
    distCoeffs.at<double>(1) = k2;
    distCoeffs.at<double>(2) = k3;
    distCoeffs.at<double>(3) = k4;

    // Calculate undistortion maps using fisheye model
    Size imageSize(cam.getWidth(), cam.getHeight());
    Mat R = Mat::eye(3, 3, CV_64F);
    Mat newCameraMatrix = cameraMatrix.clone();

    try {
        cv::fisheye::initUndistortRectifyMap(
            cameraMatrix, distCoeffs, R, newCameraMatrix,
            imageSize, CV_16SC2, map1, map2
        );
    } catch (cv::Exception& e) {
        ofLogWarning() << "Fisheye calibration error: " << e.what();
    }
}

void ofApp::applyFisheyeUndistortion() {
    if (!applyUndistortion || map1.empty() || map2.empty()) {
        undistorted = original;
        return;
    }

    Mat src = toCv(original);
    Mat dst = toCv(undistorted);

    try {
        cv::remap(src, dst, map1, map2, cv::INTER_LINEAR);
    } catch (cv::Exception& e) {
        ofLogWarning() << "Remap error: " << e.what();
        dst = src.clone();
    }

    undistorted.update();
}

void ofApp::update() {
    cam.update();

    if (cam.isFrameNew()) {
        // Copy camera frame to original image
        original.setFromPixels(cam.getPixels());

        // Apply fisheye undistortion
        applyFisheyeUndistortion();
    }
}

void ofApp::draw() {
    ofBackground(40);

    // Draw original (distorted) image
    ofSetColor(255);
    ofDrawBitmapStringHighlight("Original (Simulated Fisheye)", 10, 15);
    original.draw(0, 30, 640, 480);

    // Draw undistorted image
    ofDrawBitmapStringHighlight("Undistorted", 660, 15);
    undistorted.draw(650, 30, 640, 480);

    // Draw parameters
    ofDrawBitmapStringHighlight(
        "Parameters:\n" +
        ofToString("k1: ") + ofToString(k1, 3) + " (keys 1/2)\n" +
        ofToString("k2: ") + ofToString(k2, 3) + " (keys 3/4)\n" +
        ofToString("k3: ") + ofToString(k3, 3) + " (keys 5/6)\n" +
        ofToString("k4: ") + ofToString(k4, 3) + " (keys 7/8)\n" +
        ofToString("Focal: ") + ofToString(fx, 0) + " (keys q/w)\n" +
        ofToString("Undistortion: ") + (applyUndistortion ? "ON" : "OFF") + " (SPACE)",
        10, 530
    );

    // Draw info
    ofDrawBitmapStringHighlight(
        "Fisheye Model: cv::fisheye\n"
        "Note: Adjust k1-k4 to simulate fisheye distortion\n"
        "For real fisheye correction, use calibrated values",
        660, 530
    );
}

void ofApp::keyPressed(int key) {
    bool needsUpdate = false;

    if (key == ' ') {
        applyUndistortion = !applyUndistortion;
    }
    // k1 control
    else if (key == '1') {
        k1 -= 0.05;
        needsUpdate = true;
    }
    else if (key == '2') {
        k1 += 0.05;
        needsUpdate = true;
    }
    // k2 control
    else if (key == '3') {
        k2 -= 0.05;
        needsUpdate = true;
    }
    else if (key == '4') {
        k2 += 0.05;
        needsUpdate = true;
    }
    // k3 control
    else if (key == '5') {
        k3 -= 0.05;
        needsUpdate = true;
    }
    else if (key == '6') {
        k3 += 0.05;
        needsUpdate = true;
    }
    // k4 control
    else if (key == '7') {
        k4 -= 0.05;
        needsUpdate = true;
    }
    else if (key == '8') {
        k4 += 0.05;
        needsUpdate = true;
    }
    // Focal length control
    else if (key == 'q') {
        fx -= 20;
        fy -= 20;
        needsUpdate = true;
    }
    else if (key == 'w') {
        fx += 20;
        fy += 20;
        needsUpdate = true;
    }

    if (needsUpdate) {
        updateCalibration();
    }
}
