#include "ofApp.h"

using namespace cv;
using namespace cv::dnn;
using namespace ofxCv;

void ofApp::setup() {
    ofSetWindowTitle("Image Super-Resolution (ESPCN/FSRCNN)");
    ofSetFrameRate(30);

    modelLoaded = false;
    currentModelIndex = 0;

    // Available super-resolution models
    // Format: modelname_scalefactor.pb
    modelNames.push_back("ESPCN_x4.pb");
    algorithmNames.push_back("espcn");
    scaleFactors.push_back(4);

    modelNames.push_back("FSRCNN_x4.pb");
    algorithmNames.push_back("fsrcnn");
    scaleFactors.push_back(4);

    modelNames.push_back("ESPCN_x3.pb");
    algorithmNames.push_back("espcn");
    scaleFactors.push_back(3);

    modelNames.push_back("ESPCN_x2.pb");
    algorithmNames.push_back("espcn");
    scaleFactors.push_back(2);

    // Initialize camera with lower resolution for better performance
    inputWidth = 320;
    inputHeight = 240;
    camera.setup(inputWidth, inputHeight);

    // Try to load the first model
    string modelPath = ofToDataPath(modelNames[currentModelIndex], true);
    if (ofFile::doesFileExist(modelPath)) {
        loadModel(modelPath, algorithmNames[currentModelIndex], scaleFactors[currentModelIndex]);
    } else {
        ofLogWarning() << "Model file not found: " << modelPath;
        ofLogWarning() << "Please run download_model.sh to download models";
    }
}

bool ofApp::loadModel(const string& modelPath, const string& algorithm, int scale) {
    try {
        // Read the TensorFlow model
        net = readNetFromTensorflow(modelPath);

        if (net.empty()) {
            ofLogError() << "Failed to load model: " << modelPath;
            return false;
        }

        // Use CPU backend for compatibility
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CPU);

        modelLoaded = true;
        ofLogNotice() << "Successfully loaded model: " << modelPath << " (" << algorithm << ", scale: " << scale << "x)";
        return true;
    } catch (const cv::Exception& e) {
        ofLogError() << "OpenCV exception loading model: " << e.what();
        modelLoaded = false;
        return false;
    }
}

Mat ofApp::applySuperResolution(const Mat& input) {
    if (!modelLoaded || net.empty() || input.empty()) {
        return input;
    }

    try {
        int scale = scaleFactors[currentModelIndex];

        // Convert to float and normalize
        Mat floatInput;
        input.convertTo(floatInput, CV_32F);

        // Create blob from image (no normalization needed for these models)
        Mat blob = blobFromImage(floatInput, 1.0, Size(inputWidth, inputHeight),
                                  Scalar(), false, false);

        // Set input to the network
        net.setInput(blob);

        // Forward pass
        Mat output = net.forward();

        // The output shape is [1, channels, height, width]
        // We need to convert it back to a regular Mat
        vector<Mat> channels;
        int numChannels = output.size[1];
        int outHeight = output.size[2];
        int outWidth = output.size[3];

        for (int c = 0; c < numChannels; c++) {
            Mat channel(outHeight, outWidth, CV_32F, output.ptr<float>(0, c));
            channels.push_back(channel.clone());
        }

        Mat result;
        merge(channels, result);

        // Convert back to 8-bit
        result.convertTo(result, CV_8UC3);

        return result;
    } catch (const cv::Exception& e) {
        ofLogError() << "OpenCV exception in super-resolution: " << e.what();
        return input;
    }
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew() && modelLoaded) {
        // Convert ofPixels to cv::Mat
        frame = toCv(camera.getPixels());

        // Apply super-resolution
        superResFrame = applySuperResolution(frame);
    }
}

void ofApp::draw() {
    ofBackground(0);

    // Draw original camera feed (small)
    if (camera.isInitialized()) {
        camera.draw(10, 10, 320, 240);
        ofDrawBitmapStringHighlight("Original (" + ofToString(camera.getWidth()) + "x" +
                                   ofToString(camera.getHeight()) + ")", 20, 30);
    }

    // Draw super-resolution output (upscaled)
    if (!superResFrame.empty()) {
        int scale = scaleFactors[currentModelIndex];
        int upscaledWidth = camera.getWidth() * scale;
        int upscaledHeight = camera.getHeight() * scale;

        // Draw at original size for comparison
        drawMat(superResFrame, 350, 10, upscaledWidth / 2, upscaledHeight / 2);
        ofDrawBitmapStringHighlight("Super-Resolution (" + ofToString(superResFrame.cols) + "x" +
                                   ofToString(superResFrame.rows) + ")", 360, 30);
    }

    // Draw info
    stringstream info;
    info << "FPS: " << ofToString(ofGetFrameRate(), 1) << endl;
    info << "Model: " << modelNames[currentModelIndex] << endl;
    info << "Scale: " << scaleFactors[currentModelIndex] << "x" << endl;
    info << endl;
    info << "Controls:" << endl;
    info << "  SPACE - Switch model" << endl;
    info << "  'r' - Reload model" << endl;

    if (!modelLoaded) {
        info << endl << "WARNING: Model not loaded!" << endl;
        info << "Run download_model.sh first" << endl;
    }

    ofDrawBitmapStringHighlight(info.str(), 10, 270);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        // Switch to next model
        currentModelIndex = (currentModelIndex + 1) % modelNames.size();
        string modelPath = ofToDataPath(modelNames[currentModelIndex], true);

        if (ofFile::doesFileExist(modelPath)) {
            loadModel(modelPath, algorithmNames[currentModelIndex], scaleFactors[currentModelIndex]);
        } else {
            ofLogWarning() << "Model file not found: " << modelPath;
            modelLoaded = false;
        }
    } else if (key == 'r') {
        // Reload current model
        string modelPath = ofToDataPath(modelNames[currentModelIndex], true);
        if (ofFile::doesFileExist(modelPath)) {
            loadModel(modelPath, algorithmNames[currentModelIndex], scaleFactors[currentModelIndex]);
        }
    }
}
