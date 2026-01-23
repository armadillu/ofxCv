#include "ofApp.h"

using namespace cv;
using namespace cv::dnn;

void ofApp::setup() {
    ofSetWindowTitle("DNN Text Detection - EAST");
    ofSetFrameRate(60);

    camera.setup(640, 480);
    modelLoaded = false;

    // Load EAST text detection model
    string modelPath = ofToDataPath("frozen_east_text_detection.pb");
    if (ofFile::doesFileExist(modelPath)) {
        if (loadModel(modelPath)) {
            ofLogNotice() << "EAST model loaded successfully";
        } else {
            ofLogError() << "Failed to load EAST model";
        }
    } else {
        ofLogWarning() << "Model not found. Run bin/data/download_model.sh to download.";
    }
}

bool ofApp::loadModel(const string& modelPath) {
    try {
        net = readNet(modelPath);
        net.setPreferableBackend(DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(DNN_TARGET_CPU);
        modelLoaded = true;
        return true;
    } catch (const cv::Exception& e) {
        ofLogError() << "OpenCV Exception: " << e.what();
        modelLoaded = false;
        return false;
    }
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew() && modelLoaded) {
        frame = ofxCv::toCv(camera.getPixels());
        cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

        detectedBoxes = detectText(frame);
    }
}

vector<ofApp::TextBox> ofApp::detectText(const cv::Mat& input) {
    vector<TextBox> results;

    // Create blob from image
    Mat blob;
    blobFromImage(input, blob, 1.0, Size(inputWidth, inputHeight),
                  Scalar(123.68, 116.78, 103.94), true, false);

    // Set input
    net.setInput(blob);

    // Forward pass - get output layers
    vector<String> outNames = {"feature_fusion/Conv_7/Sigmoid",
                               "feature_fusion/concat_3"};
    vector<Mat> outs;
    net.forward(outs, outNames);

    Mat scores = outs[0];
    Mat geometry = outs[1];

    // Decode detections
    vector<RotatedRect> boxes;
    vector<float> confidences;
    decode(scores, geometry, confThreshold, boxes, confidences);

    // Apply NMS
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    // Scale boxes to original image size
    float ratioh = (float)input.rows / inputHeight;
    float ratiow = (float)input.cols / inputWidth;

    for (int idx : indices) {
        RotatedRect& box = boxes[idx];

        // Scale the box
        box.center.x *= ratiow;
        box.center.y *= ratioh;
        box.size.width *= ratiow;
        box.size.height *= ratioh;

        TextBox textBox;
        textBox.box = box;
        textBox.confidence = confidences[idx];
        results.push_back(textBox);
    }

    return results;
}

void ofApp::decode(const Mat& scores, const Mat& geometry, float scoreThresh,
                   vector<RotatedRect>& detections, vector<float>& confidences) {
    detections.clear();
    confidences.clear();

    const int height = scores.size[2];
    const int width = scores.size[3];

    for (int y = 0; y < height; y++) {
        const float* scoresData = scores.ptr<float>(0, 0, y);
        const float* x0_data = geometry.ptr<float>(0, 0, y);
        const float* x1_data = geometry.ptr<float>(0, 1, y);
        const float* x2_data = geometry.ptr<float>(0, 2, y);
        const float* x3_data = geometry.ptr<float>(0, 3, y);
        const float* anglesData = geometry.ptr<float>(0, 4, y);

        for (int x = 0; x < width; x++) {
            float score = scoresData[x];
            if (score < scoreThresh)
                continue;

            // Decode bounding box
            float offsetX = x * 4.0f;
            float offsetY = y * 4.0f;
            float angle = anglesData[x];
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float h = x0_data[x] + x2_data[x];
            float w = x1_data[x] + x3_data[x];

            Point2f offset(offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                          offsetY - sinA * x1_data[x] + cosA * x2_data[x]);
            Point2f p1 = Point2f(-sinA * h, -cosA * h) + offset;
            Point2f p3 = Point2f(-cosA * w, sinA * w) + offset;
            RotatedRect r(0.5f * (p1 + p3), Size2f(w, h), -angle * 180.0f / CV_PI);

            detections.push_back(r);
            confidences.push_back(score);
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    camera.draw(0, 0);

    if (modelLoaded) {
        // Draw detected text boxes
        ofNoFill();
        for (const auto& textBox : detectedBoxes) {
            Point2f vertices[4];
            textBox.box.points(vertices);

            // Draw rotated rectangle
            ofSetColor(0, 255, 0);
            ofSetLineWidth(2);
            for (int i = 0; i < 4; i++) {
                ofDrawLine(vertices[i].x, vertices[i].y,
                          vertices[(i + 1) % 4].x, vertices[(i + 1) % 4].y);
            }

            // Draw confidence
            ofSetColor(255, 255, 0);
            string confStr = ofToString(textBox.confidence, 2);
            ofDrawBitmapString(confStr, textBox.box.center.x, textBox.box.center.y);
        }

        // Info
        ofSetColor(255);
        ofFill();
        ofDrawBitmapStringHighlight("FPS: " + ofToString(ofGetFrameRate(), 1), 10, 20);
        ofDrawBitmapStringHighlight("Text boxes detected: " + ofToString(detectedBoxes.size()), 10, 40);
        ofDrawBitmapStringHighlight("Conf threshold: " + ofToString(confThreshold, 2) + " (Up/Down)", 10, 60);
        ofDrawBitmapStringHighlight("NMS threshold: " + ofToString(nmsThreshold, 2) + " (Left/Right)", 10, 80);
        ofDrawBitmapStringHighlight("Press SPACE to toggle camera", 10, 100);
    } else {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Model not loaded. Run bin/data/download_model.sh", 10, 20);
    }
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        if (camera.isInitialized()) {
            camera.close();
        } else {
            camera.setup(640, 480);
        }
    } else if (key == OF_KEY_UP) {
        confThreshold = min(confThreshold + 0.05f, 1.0f);
        ofLogNotice() << "Confidence threshold: " << confThreshold;
    } else if (key == OF_KEY_DOWN) {
        confThreshold = max(confThreshold - 0.05f, 0.0f);
        ofLogNotice() << "Confidence threshold: " << confThreshold;
    } else if (key == OF_KEY_RIGHT) {
        nmsThreshold = min(nmsThreshold + 0.05f, 1.0f);
        ofLogNotice() << "NMS threshold: " << nmsThreshold;
    } else if (key == OF_KEY_LEFT) {
        nmsThreshold = max(nmsThreshold - 0.05f, 0.0f);
        ofLogNotice() << "NMS threshold: " << nmsThreshold;
    }
}
