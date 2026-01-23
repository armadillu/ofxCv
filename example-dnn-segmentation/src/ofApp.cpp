#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(30);

    cam.setup(1280, 720);

    loadClassNames();
    buildClassColors();

    std::string modelPath = ofToDataPath("deeplabv3_mnv2.onnx");
    if(ofFile::doesFileExist(modelPath)) {
        try {
            net = dnn::readNetFromONNX(modelPath);
            net.setPreferableBackend(dnn::DNN_BACKEND_DEFAULT);
            net.setPreferableTarget(dnn::DNN_TARGET_CPU);
            modelLoaded = true;
            ofLog() << "Segmentation model loaded successfully!";
        } catch(const cv::Exception& e) {
            ofLogError() << "Failed to load model: " << e.what();
        }
    } else {
        ofLogError() << "Model file not found!";
        ofLogError() << "Expected: " << modelPath;
        ofLogError() << "Run: cd bin/data && ./download_model.sh";
    }

    ofSetWindowTitle("OpenCV 4.x DNN Semantic Segmentation");
}

void ofApp::loadClassNames() {
    classNames = {
        "background", "aeroplane", "bicycle", "bird", "boat", "bottle",
        "bus", "car", "cat", "chair", "cow", "diningtable", "dog",
        "horse", "motorbike", "person", "pottedplant", "sheep", "sofa",
        "train", "tvmonitor"
    };
}

void ofApp::buildClassColors() {
    classColors.clear();
    classColors.reserve(classNames.size());

    for(size_t i = 0; i < classNames.size(); i++) {
        if(i == 0) {
            classColors.push_back(Vec3b(0, 0, 0));
            continue;
        }
        float hue = ofMap(i, 1, classNames.size(), 0, 255);
        ofColor color = ofColor::fromHsb(hue, 200, 255);
        classColors.push_back(Vec3b(color.r, color.g, color.b));
    }
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew() && modelLoaded) {
        Mat frame = toCv(cam);
        updateSegmentation(frame);
    }
}

void ofApp::updateSegmentation(const Mat& frame) {
    Mat blob = dnn::blobFromImage(frame, 1.0 / 127.5, Size(inputWidth, inputHeight),
                                  Scalar(127.5, 127.5, 127.5), false, false);

    float startTime = ofGetElapsedTimef();
    net.setInput(blob);
    Mat output = net.forward();
    inferenceTime = (ofGetElapsedTimef() - startTime) * 1000.0f;

    if(output.dims != 4) {
        ofLogError() << "Unexpected output dims: " << output.dims;
        return;
    }

    int classes = output.size[1];
    int outH = output.size[2];
    int outW = output.size[3];

    if(classes <= 0 || outH <= 0 || outW <= 0) {
        ofLogError() << "Invalid output shape.";
        return;
    }

    if(classColors.size() < static_cast<size_t>(classes)) {
        size_t start = classColors.size();
        classColors.resize(classes);
        for(size_t i = start; i < classColors.size(); i++) {
            float hue = ofMap(i, 0, classColors.size(), 0, 255);
            ofColor color = ofColor::fromHsb(hue, 200, 255);
            classColors[i] = Vec3b(color.r, color.g, color.b);
        }
    }

    Mat classIdMap(outH, outW, CV_8U);

    std::vector<const float*> classData(classes);
    for(int c = 0; c < classes; c++) {
        classData[c] = output.ptr<float>(0, c);
    }

    // Argmax across classes for each pixel.
    for(int y = 0; y < outH; y++) {
        unsigned char* row = classIdMap.ptr<unsigned char>(y);
        int rowOffset = y * outW;
        for(int x = 0; x < outW; x++) {
            int index = rowOffset + x;
            float bestScore = classData[0][index];
            int bestClass = 0;
            for(int c = 1; c < classes; c++) {
                float score = classData[c][index];
                if(score > bestScore) {
                    bestScore = score;
                    bestClass = c;
                }
            }
            row[x] = static_cast<unsigned char>(bestClass);
        }
    }

    Mat colorMask(outH, outW, CV_8UC3);
    for(int y = 0; y < outH; y++) {
        const unsigned char* classRow = classIdMap.ptr<unsigned char>(y);
        Vec3b* colorRow = colorMask.ptr<Vec3b>(y);
        for(int x = 0; x < outW; x++) {
            int classId = classRow[x];
            if(classId >= 0 && classId < classes) {
                colorRow[x] = classColors[classId];
            } else {
                colorRow[x] = Vec3b(0, 0, 0);
            }
        }
    }

    cv::resize(colorMask, maskMat, frame.size(), 0, 0, INTER_NEAREST);
    cv::resize(classIdMap, classIdMapResized, frame.size(), 0, 0, INTER_NEAREST);

    if(showOverlay) {
        cv::addWeighted(frame, 1.0f - overlayAlpha, maskMat, overlayAlpha, 0.0, overlayMat);
    } else {
        overlayMat = maskMat.clone();
    }
}

int ofApp::getClassIdAt(int x, int y) const {
    if(classIdMapResized.empty()) {
        return -1;
    }

    int clampedX = ofClamp(x, 0, classIdMapResized.cols - 1);
    int clampedY = ofClamp(y, 0, classIdMapResized.rows - 1);
    return classIdMapResized.at<unsigned char>(clampedY, clampedX);
}

void ofApp::draw() {
    ofSetColor(255);

    if(modelLoaded && !overlayMat.empty()) {
        drawMat(overlayMat, 0, 0, ofGetWidth(), ofGetHeight());
    } else {
        cam.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    if(modelLoaded && showMaskThumbnail && !maskMat.empty()) {
        float thumbWidth = 320.0f;
        float thumbHeight = thumbWidth * (static_cast<float>(maskMat.rows) / maskMat.cols);
        float x = ofGetWidth() - thumbWidth - 20.0f;
        float y = ofGetHeight() - thumbHeight - 20.0f;

        ofSetColor(255);
        drawMat(maskMat, x, y, thumbWidth, thumbHeight);

        ofNoFill();
        ofSetColor(255);
        ofDrawRectangle(x, y, thumbWidth, thumbHeight);
        ofFill();
    }

    // Info panel
    ofSetColor(0, 180);
    ofDrawRectangle(10, 10, 420, 180);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x DNN Semantic Segmentation", 20, y); y += 22;
    ofDrawBitmapString("Model: deeplabv3_mnv2.onnx", 20, y); y += 20;
    ofDrawBitmapString("Input: " + ofToString(inputWidth) + "x" + ofToString(inputHeight), 20, y); y += 20;

    if(modelLoaded) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Model loaded", 20, y); y += 20;
    } else {
        ofSetColor(255, 100, 100);
        ofDrawBitmapString("Model not loaded", 20, y); y += 20;
        ofSetColor(255, 180, 180);
        ofDrawBitmapString("Run: cd bin/data && ./download_model.sh", 20, y); y += 20;
    }

    ofSetColor(255, 255, 0);
    ofDrawBitmapString("Inference: " + ofToString(inferenceTime, 1) + " ms", 20, y); y += 20;

    ofSetColor(200);
    ofDrawBitmapString("[o] overlay  [m] mask  [l] legend  [/] alpha", 20, y);

    if(modelLoaded && !classIdMapResized.empty()) {
        int classId = getClassIdAt(ofGetMouseX(), ofGetMouseY());
        if(classId >= 0) {
            std::string label;
            if(classId < static_cast<int>(classNames.size())) {
                label = classNames[classId];
            } else {
                label = "class " + ofToString(classId);
            }

            ofSetColor(0, 180);
            ofDrawRectangle(ofGetMouseX() + 12, ofGetMouseY() + 12, 160, 22);
            ofSetColor(255);
            ofDrawBitmapString("Label: " + label, ofGetMouseX() + 18, ofGetMouseY() + 28);
        }
    }

    if(modelLoaded && showLegend) {
        int rows = static_cast<int>((classNames.size() + 1) / 2);
        int colWidth = 180;
        int startX = 20;
        int startY = 180;
        int panelHeight = rows * 18 + 20;
        int panelWidth = colWidth * 2 + 20;

        ofSetColor(0, 160);
        ofDrawRectangle(startX - 10, startY - 15, panelWidth, panelHeight);

        for(size_t i = 0; i < classNames.size(); i++) {
            int col = static_cast<int>(i / rows);
            int row = static_cast<int>(i % rows);
            int x = startX + col * colWidth;
            int y = startY + row * 18;

            ofSetColor(classColors[i][0], classColors[i][1], classColors[i][2]);
            ofDrawRectangle(x, y - 10, 10, 10);

            ofSetColor(255);
            ofDrawBitmapString(classNames[i], x + 16, y);
        }
    }
}

void ofApp::keyPressed(int key) {
    if(key == 'o' || key == 'O') {
        showOverlay = !showOverlay;
    } else if(key == 'm' || key == 'M') {
        showMaskThumbnail = !showMaskThumbnail;
    } else if(key == 'l' || key == 'L') {
        showLegend = !showLegend;
    } else if(key == '[') {
        overlayAlpha = std::max(0.0f, overlayAlpha - 0.05f);
    } else if(key == ']') {
        overlayAlpha = std::min(1.0f, overlayAlpha + 0.05f);
    }
}
