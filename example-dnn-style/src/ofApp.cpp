#include "ofApp.h"

using namespace cv;
using namespace cv::dnn;
using namespace ofxCv;

void ofApp::setup() {
    ofSetWindowTitle("Neural Style Transfer");
    ofSetFrameRate(30);

    modelLoaded = false;
    currentModelIndex = 0;

    // Available style models
    modelNames.push_back("mosaic.t7");
    modelNames.push_back("candy.t7");
    modelNames.push_back("rain_princess.t7");
    modelNames.push_back("udnie.t7");

    // Initialize camera
    camera.setup(640, 480);

    // Try to load the first model
    string modelPath = ofToDataPath(modelNames[currentModelIndex], true);
    if (ofFile::doesFileExist(modelPath)) {
        loadModel(modelPath);
    } else {
        ofLogWarning() << "Model file not found: " << modelPath;
        ofLogWarning() << "Please run download_model.sh to download models";
    }
}

bool ofApp::loadModel(const string& modelPath) {
    try {
        net = readNetFromTorch(modelPath);

        if (net.empty()) {
            ofLogError() << "Failed to load model: " << modelPath;
            return false;
        }

        // Use CPU for compatibility
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);

        modelLoaded = true;
        ofLogNotice() << "Successfully loaded model: " << modelPath;
        return true;
    } catch (const cv::Exception& e) {
        ofLogError() << "OpenCV exception loading model: " << e.what();
        modelLoaded = false;
        return false;
    }
}

Mat ofApp::applyStyle(const Mat& input) {
    if (!modelLoaded || net.empty() || input.empty()) {
        return input;
    }

    try {
        // Create blob from image
        Mat blob = blobFromImage(input, 1.0, Size(640, 480),
                                  Scalar(103.939, 116.779, 123.68),
                                  false, false);

        // Set input to the network
        net.setInput(blob);

        // Forward pass
        Mat output = net.forward();

        // Reshape output to image format
        // Output shape is [1, 3, height, width]
        vector<Mat> channels(3);
        for (int c = 0; c < 3; c++) {
            Mat channel(output.size[2], output.size[3], CV_32F,
                       output.ptr<float>(0, c));

            // Add mean back
            if (c == 0) channel += 103.939;
            else if (c == 1) channel += 116.779;
            else channel += 123.68;

            channels[2 - c] = channel; // BGR order
        }

        Mat result;
        merge(channels, result);
        result.convertTo(result, CV_8UC3);

        return result;
    } catch (const cv::Exception& e) {
        ofLogError() << "OpenCV exception in style transfer: " << e.what();
        return input;
    }
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew() && modelLoaded) {
        // Convert ofPixels to cv::Mat
        frame = toCv(camera.getPixels());

        // Apply style transfer
        styledFrame = applyStyle(frame);
    }
}

void ofApp::draw() {
    ofBackground(0);

    // Draw original camera feed on the left
    if (camera.isInitialized()) {
        camera.draw(0, 0, 320, 240);
        ofDrawBitmapStringHighlight("Original", 10, 20);
    }

    // Draw styled output on the right
    if (!styledFrame.empty()) {
        drawMat(styledFrame, 320, 0, 320, 240);
        ofDrawBitmapStringHighlight("Styled", 330, 20);
    }

    // Draw info
    stringstream info;
    info << "FPS: " << ofGetFrameRate() << endl;
    info << "Model: " << modelNames[currentModelIndex] << endl;
    info << "Press SPACE to switch style" << endl;
    info << "Press 'r' to reload model" << endl;

    if (!modelLoaded) {
        info << endl << "WARNING: Model not loaded!" << endl;
        info << "Run download_model.sh first" << endl;
    }

    ofDrawBitmapStringHighlight(info.str(), 10, 260);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        // Switch to next model
        currentModelIndex = (currentModelIndex + 1) % modelNames.size();
        string modelPath = ofToDataPath(modelNames[currentModelIndex], true);

        if (ofFile::doesFileExist(modelPath)) {
            loadModel(modelPath);
        } else {
            ofLogWarning() << "Model file not found: " << modelPath;
            modelLoaded = false;
        }
    } else if (key == 'r') {
        // Reload current model
        string modelPath = ofToDataPath(modelNames[currentModelIndex], true);
        if (ofFile::doesFileExist(modelPath)) {
            loadModel(modelPath);
        }
    }
}
