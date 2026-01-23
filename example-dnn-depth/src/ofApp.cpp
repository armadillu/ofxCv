#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(30);
    ofSetWindowTitle("OpenCV 4.x DNN: Monocular Depth Estimation (MiDaS)");

    // Camera setup
    cam.setDesiredFrameRate(30);
    cam.initGrabber(640, 480);

    // Load MiDaS model
    string modelPath = ofToDataPath("midas_v21_small_256.onnx", true);

    if (ofFile::doesFileExist(modelPath)) {
        try {
            net = cv::dnn::readNetFromONNX(modelPath);

            if (!net.empty()) {
                // Use CPU backend by default
                net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
                net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

                modelLoaded = true;
                ofLogNotice("ofApp") << "MiDaS model loaded: " << modelPath;
            } else {
                ofLogError("ofApp") << "Failed to load model (empty network)";
            }
        } catch (const cv::Exception& e) {
            ofLogError("ofApp") << "OpenCV exception: " << e.what();
        }
    } else {
        ofLogError("ofApp") << "Model not found: " << modelPath;
        ofLogError("ofApp") << "Run: ./bin/data/download_model.sh";
    }

    ofLogNotice("ofApp") << "Controls:";
    ofLogNotice("ofApp") << "  SPACE: Toggle depth map visibility";
    ofLogNotice("ofApp") << "  C: Toggle colorization";
    ofLogNotice("ofApp") << "  N: Toggle auto-normalization";
}

void ofApp::update() {
    cam.update();

    if (cam.isFrameNew() && modelLoaded) {
        cv::Mat frame = ofxCv::toCv(cam.getPixels());
        updateDepth(frame);
    }
}

void ofApp::updateDepth(const cv::Mat& frame) {
    uint64_t start = ofGetElapsedTimeMillis();

    // Prepare input blob
    cv::Mat blob;
    cv::Mat inputFrame;

    // MiDaS expects RGB input
    if (frame.channels() == 3) {
        cv::cvtColor(frame, inputFrame, cv::COLOR_BGR2RGB);
    } else if (frame.channels() == 4) {
        cv::cvtColor(frame, inputFrame, cv::COLOR_BGRA2RGB);
    } else {
        cv::cvtColor(frame, inputFrame, cv::COLOR_GRAY2RGB);
    }

    // Create blob: scale pixels to [0,1], resize to input size
    cv::dnn::blobFromImage(
        inputFrame,
        blob,
        1.0 / 255.0,
        cv::Size(inputWidth, inputHeight),
        cv::Scalar(0, 0, 0),
        false,
        false
    );

    // Forward pass
    net.setInput(blob);
    cv::Mat output = net.forward();

    // Output shape: [1, 1, H, W]
    // Extract depth map
    std::vector<int> size = {output.size[2], output.size[3]};
    cv::Mat depth(size, CV_32F, output.ptr<float>());

    // Resize to original frame size
    cv::resize(depth, depthMat, frame.size(), 0, 0, cv::INTER_CUBIC);

    // Normalize depth values
    if (autoNormalize) {
        normalizeDepth(depthMat, depthNormalized);
    } else {
        depthNormalized = depthMat.clone();
    }

    // Colorize for visualization
    if (showColorized) {
        colorizeDepth(depthNormalized, depthColorized);
    }

    uint64_t end = ofGetElapsedTimeMillis();
    inferenceTime = (end - start);
}

void ofApp::normalizeDepth(const cv::Mat& depth, cv::Mat& output) {
    double min, max;
    cv::minMaxLoc(depth, &min, &max);

    minDepth = static_cast<float>(min);
    maxDepth = static_cast<float>(max);

    // Normalize to [0, 1]
    depth.convertTo(output, CV_32F, 1.0 / (max - min), -min / (max - min));
}

void ofApp::colorizeDepth(const cv::Mat& normalized, cv::Mat& output) {
    // Convert to 8-bit
    cv::Mat depth8u;
    normalized.convertTo(depth8u, CV_8U, 255.0);

    // Apply COLORMAP_TURBO (good for depth visualization)
    cv::applyColorMap(depth8u, output, cv::COLORMAP_TURBO);
}

void ofApp::draw() {
    ofBackground(30);

    int x = 10, y = 10;

    // Draw camera feed
    ofSetColor(255);
    if (cam.isInitialized()) {
        cam.draw(x, y, 640, 480);
        ofDrawBitmapStringHighlight("Camera Feed", x + 5, y + 15, ofColor::black, ofColor::white);
        x += 650;
    }

    // Draw depth map
    if (modelLoaded && showDepthMap && depthColorized.rows > 0) {
        ofSetColor(255);
        ofxCv::drawMat(depthColorized, x, y, 640, 480);

        string label = showColorized ? "Depth Map (Colorized)" : "Depth Map (Grayscale)";
        ofDrawBitmapStringHighlight(label, x + 5, y + 15, ofColor::black, ofColor::white);
    }

    // Draw info
    ofSetColor(255);
    int infoY = 510;

    if (modelLoaded) {
        ofDrawBitmapStringHighlight(
            "Inference: " + ofToString(inferenceTime, 1) + " ms | " +
            "FPS: " + ofToString(ofGetFrameRate(), 1),
            10, infoY, ofColor::black, ofColor::yellow
        );
        infoY += 20;

        ofDrawBitmapStringHighlight(
            "Depth range: [" + ofToString(minDepth, 2) + ", " + ofToString(maxDepth, 2) + "]",
            10, infoY, ofColor::black, ofColor::cyan
        );
        infoY += 20;

        ofDrawBitmapStringHighlight(
            "Input: " + ofToString(inputWidth) + "x" + ofToString(inputHeight),
            10, infoY, ofColor::black, ofColor::cyan
        );
    } else {
        ofDrawBitmapStringHighlight(
            "Model not loaded. Run: ./bin/data/download_model.sh",
            10, infoY, ofColor::black, ofColor::red
        );
    }

    // Draw controls
    infoY += 30;
    ofSetColor(200);
    ofDrawBitmapString("Controls:", 10, infoY);
    infoY += 15;
    ofDrawBitmapString("  SPACE: Toggle depth map [" + string(showDepthMap ? "ON" : "OFF") + "]", 10, infoY);
    infoY += 15;
    ofDrawBitmapString("  C: Toggle colorization [" + string(showColorized ? "ON" : "OFF") + "]", 10, infoY);
    infoY += 15;
    ofDrawBitmapString("  N: Toggle auto-normalize [" + string(autoNormalize ? "ON" : "OFF") + "]", 10, infoY);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        showDepthMap = !showDepthMap;
    } else if (key == 'c' || key == 'C') {
        showColorized = !showColorized;
        if (modelLoaded && depthNormalized.rows > 0) {
            colorizeDepth(depthNormalized, depthColorized);
        }
    } else if (key == 'n' || key == 'N') {
        autoNormalize = !autoNormalize;
    }
}
