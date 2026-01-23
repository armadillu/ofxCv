#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(30);
    ofSetWindowTitle("Stereo Depth Estimation - StereoBM");

    // Initialize stereo matcher
    stereoBM = cv::StereoBM::create();

    // Settings (adjust for better results)
    numDisparities = 64;  // Must be divisible by 16
    blockSize = 15;       // Must be odd, typically 5-21
    preFilterCap = 31;
    uniquenessRatio = 15;
    speckleWindowSize = 100;
    speckleRange = 32;

    stereoBM->setNumDisparities(numDisparities);
    stereoBM->setBlockSize(blockSize);
    stereoBM->setPreFilterCap(preFilterCap);
    stereoBM->setUniquenessRatio(uniquenessRatio);
    stereoBM->setSpeckleWindowSize(speckleWindowSize);
    stereoBM->setSpeckleRange(speckleRange);

    // Try to initialize webcam
    cam.setDesiredFrameRate(30);
    cam.setup(640, 480);
    useWebcam = cam.isInitialized();

    if (!useWebcam) {
        ofLogNotice() << "Webcam not available, using synthetic stereo pair";
        generateSyntheticStereoPair();
    }

    currentFrame = 0;

    ofLogNotice() << "Controls:";
    ofLogNotice() << "  1/2: Adjust numDisparities (-/+)";
    ofLogNotice() << "  3/4: Adjust blockSize (-/+)";
    ofLogNotice() << "  5/6: Adjust preFilterCap (-/+)";
    ofLogNotice() << "  7/8: Adjust uniquenessRatio (-/+)";
    ofLogNotice() << "  SPACE: Generate new synthetic pair";
}

void ofApp::update() {
    if (useWebcam && cam.isInitialized()) {
        cam.update();

        if (cam.isFrameNew()) {
            // Generate synthetic stereo pair from webcam feed
            Mat frame = toCv(cam);
            cvtColor(frame, leftImage, cv::COLOR_RGB2GRAY);

            // Shift right image by a few pixels to simulate stereo
            int shift = 20;
            rightImage = Mat::zeros(leftImage.size(), leftImage.type());
            leftImage(Rect(shift, 0, leftImage.cols - shift, leftImage.rows))
                .copyTo(rightImage(Rect(0, 0, leftImage.cols - shift, leftImage.rows)));

            processStereoPair();
        }
    } else {
        // Animate synthetic stereo pair
        currentFrame++;
        if (currentFrame % 120 == 0) {
            generateSyntheticStereoPair();
            processStereoPair();
        }
    }
}

void ofApp::draw() {
    ofBackground(30);

    int x = 20, y = 20, w = 320, h = 240;

    // Draw left image
    if (!leftImage.empty()) {
        ofxCv::toOf(leftImage, leftImg);
        leftImg.update();
        leftImg.draw(x, y, w, h);
        ofSetColor(255);
        ofDrawBitmapString("Left Image", x, y - 5);
    }

    // Draw right image
    if (!rightImage.empty()) {
        ofxCv::toOf(rightImage, rightImg);
        rightImg.update();
        rightImg.draw(x + w + 20, y, w, h);
        ofSetColor(255);
        ofDrawBitmapString("Right Image", x + w + 20, y - 5);
    }

    // Draw disparity map
    if (!disparityVis.empty()) {
        ofxCv::toOf(disparityVis, depthImg);
        depthImg.update();
        depthImg.draw(x, y + h + 20, w * 2 + 20, h);
        ofSetColor(255);
        ofDrawBitmapString("Disparity Map (Depth)", x, y + h + 15);
    }

    // Draw settings
    ofSetColor(255);
    int ty = y + h * 2 + 60;
    ofDrawBitmapString("Settings:", 20, ty);
    ty += 20;
    ofDrawBitmapString("  numDisparities: " + ofToString(numDisparities) + " (keys 1/2)", 20, ty);
    ty += 15;
    ofDrawBitmapString("  blockSize: " + ofToString(blockSize) + " (keys 3/4)", 20, ty);
    ty += 15;
    ofDrawBitmapString("  preFilterCap: " + ofToString(preFilterCap) + " (keys 5/6)", 20, ty);
    ty += 15;
    ofDrawBitmapString("  uniquenessRatio: " + ofToString(uniquenessRatio) + " (keys 7/8)", 20, ty);
    ty += 15;
    ofDrawBitmapString("  SPACE: Generate new synthetic pair", 20, ty);

    // Draw info
    ty += 30;
    ofDrawBitmapString("Mode: " + string(useWebcam ? "Webcam (simulated stereo)" : "Synthetic"), 20, ty);
    ty += 15;
    ofDrawBitmapString("Stereo matching estimates depth from two views", 20, ty);
    ty += 15;
    ofDrawBitmapString("Brighter = closer, Darker = farther", 20, ty);
}

void ofApp::keyPressed(int key) {
    bool needsUpdate = false;

    switch(key) {
        case '1':
            numDisparities = max(16, numDisparities - 16);
            stereoBM->setNumDisparities(numDisparities);
            needsUpdate = true;
            break;
        case '2':
            numDisparities = min(256, numDisparities + 16);
            stereoBM->setNumDisparities(numDisparities);
            needsUpdate = true;
            break;
        case '3':
            blockSize = max(5, blockSize - 2);
            if (blockSize % 2 == 0) blockSize--;
            stereoBM->setBlockSize(blockSize);
            needsUpdate = true;
            break;
        case '4':
            blockSize = min(21, blockSize + 2);
            if (blockSize % 2 == 0) blockSize++;
            stereoBM->setBlockSize(blockSize);
            needsUpdate = true;
            break;
        case '5':
            preFilterCap = max(1, preFilterCap - 5);
            stereoBM->setPreFilterCap(preFilterCap);
            needsUpdate = true;
            break;
        case '6':
            preFilterCap = min(63, preFilterCap + 5);
            stereoBM->setPreFilterCap(preFilterCap);
            needsUpdate = true;
            break;
        case '7':
            uniquenessRatio = max(5, uniquenessRatio - 5);
            stereoBM->setUniquenessRatio(uniquenessRatio);
            needsUpdate = true;
            break;
        case '8':
            uniquenessRatio = min(100, uniquenessRatio + 5);
            stereoBM->setUniquenessRatio(uniquenessRatio);
            needsUpdate = true;
            break;
        case ' ':
            generateSyntheticStereoPair();
            needsUpdate = true;
            break;
    }

    if (needsUpdate && !leftImage.empty() && !rightImage.empty()) {
        processStereoPair();
    }
}

void ofApp::processStereoPair() {
    if (leftImage.empty() || rightImage.empty()) return;

    // Compute disparity map
    stereoBM->compute(leftImage, rightImage, disparity);

    // Visualize disparity
    visualizeDisparity();
}

void ofApp::generateSyntheticStereoPair() {
    int width = 640;
    int height = 480;

    leftImage = Mat(height, width, CV_8UC1);
    rightImage = Mat(height, width, CV_8UC1);

    // Create a scene with geometric shapes at different depths
    leftImage = Scalar(50);
    rightImage = Scalar(50);

    // Far background
    rectangle(leftImage, Rect(50, 50, 540, 380), Scalar(80), cv::FILLED);
    rectangle(rightImage, Rect(45, 50, 540, 380), Scalar(80), cv::FILLED);

    // Middle layer - circle
    circle(leftImage, Point(320, 240), 80, Scalar(150), cv::FILLED);
    circle(rightImage, Point(310, 240), 80, Scalar(150), cv::FILLED);

    // Near layer - squares
    rectangle(leftImage, Rect(150, 150, 100, 100), Scalar(200), cv::FILLED);
    rectangle(rightImage, Rect(135, 150, 100, 100), Scalar(200), cv::FILLED);

    rectangle(leftImage, Rect(400, 250, 120, 120), Scalar(220), cv::FILLED);
    rectangle(rightImage, Rect(380, 250, 120, 120), Scalar(220), cv::FILLED);

    // Add some texture
    Mat noise(height, width, CV_8UC1);
    randn(noise, 0, 10);
    leftImage += noise;
    rightImage += noise;

    ofLogNotice() << "Generated new synthetic stereo pair";
}

void ofApp::visualizeDisparity() {
    if (disparity.empty()) return;

    // Normalize disparity to 0-255 for visualization
    double minVal, maxVal;
    minMaxLoc(disparity, &minVal, &maxVal);

    disparity.convertTo(disparityVis, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    // Apply color map for better visualization
    Mat disparityColor;
    applyColorMap(disparityVis, disparityColor, cv::COLORMAP_JET);
    cvtColor(disparityColor, disparityVis, cv::COLOR_BGR2RGB);
}
