#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    confThreshold = 0.5f;
    nmsThreshold = 0.4f;
    inputWidth = 416;
    inputHeight = 416;
    inferenceTime = 0;
    modelLoaded = false;

    // Load YOLO model
    std::string modelConfig = ofToDataPath("yolov4-tiny.cfg");
    std::string modelWeights = ofToDataPath("yolov4-tiny.weights");

    if(ofFile::doesFileExist(modelConfig) && ofFile::doesFileExist(modelWeights)) {
        try {
            net = dnn::readNetFromDarknet(modelConfig, modelWeights);
            net.setPreferableBackend(dnn::DNN_BACKEND_DEFAULT);
            net.setPreferableTarget(dnn::DNN_TARGET_CPU);
            modelLoaded = true;
            ofLog() << "YOLO model loaded successfully!";
        } catch(const cv::Exception& e) {
            ofLogError() << "Failed to load YOLO model: " << e.what();
        }
    } else {
        ofLogError() << "Model files not found!";
        ofLogError() << "Please download YOLOv4-tiny model files:";
        ofLogError() << "  1. yolov4-tiny.cfg";
        ofLogError() << "  2. yolov4-tiny.weights";
        ofLogError() << "";
        ofLogError() << "Run: cd bin/data && ./download_model.sh";
    }

    // Load class names
    loadClassNames();

    // Generate random colors for each class
    for(int i = 0; i < 80; i++) {
        colors.push_back(ofColor::fromHsb(ofRandom(255), 200, 255));
    }

    ofSetWindowTitle("OpenCV 4.x YOLO Object Detection");
}

void ofApp::loadClassNames() {
    // COCO dataset class names
    classNames = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck",
        "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench",
        "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra",
        "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove",
        "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup",
        "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
        "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse",
        "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink",
        "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier",
        "toothbrush"
    };
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew() && modelLoaded) {
        Mat frame = toCv(cam);

        // Create blob from image
        Mat blob = dnn::blobFromImage(frame, 1/255.0, Size(inputWidth, inputHeight),
                                       Scalar(0, 0, 0), true, false);

        // Set input
        net.setInput(blob);

        // Get output layer names
        std::vector<String> outNames = net.getUnconnectedOutLayersNames();

        // Run forward pass
        float startTime = ofGetElapsedTimef();
        std::vector<Mat> outs;
        net.forward(outs, outNames);
        inferenceTime = (ofGetElapsedTimef() - startTime) * 1000;

        // Process detections
        postProcess(frame, outs);
    }
}

void ofApp::postProcess(const Mat& frame, const std::vector<Mat>& outs) {
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<Rect> boxes;

    for(size_t i = 0; i < outs.size(); i++) {
        float* data = (float*)outs[i].data;
        for(int j = 0; j < outs[i].rows; j++, data += outs[i].cols) {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

            if(confidence > confThreshold) {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }

    // Apply non-maximum suppression
    std::vector<int> indices;
    dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    detections.clear();
    for(size_t i = 0; i < indices.size(); i++) {
        int idx = indices[i];
        Detection det;
        det.classId = classIds[idx];
        det.confidence = confidences[idx];
        det.box = boxes[idx];
        detections.push_back(det);
    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    if(modelLoaded) {
        // Draw detections
        for(const auto& det : detections) {
            ofColor color = colors[det.classId % colors.size()];

            // Draw box
            ofNoFill();
            ofSetLineWidth(3);
            ofSetColor(color);
            ofDrawRectangle(det.box.x, det.box.y, det.box.width, det.box.height);

            // Draw label background
            std::string label = classNames[det.classId] + " " +
                                ofToString(det.confidence * 100, 0) + "%";

            ofFill();
            ofSetColor(color);
            ofDrawRectangle(det.box.x, det.box.y - 25, label.length() * 10 + 10, 25);

            // Draw label text
            ofSetColor(0);
            ofDrawBitmapString(label, det.box.x + 5, det.box.y - 8);
        }

        // Draw info panel
        ofSetColor(0, 180);
        ofFill();
        ofDrawRectangle(10, 10, 380, 180);

        ofSetColor(255);
        int y = 30;
        ofDrawBitmapString("OpenCV 4.x YOLO Object Detection", 20, y); y += 20;
        ofDrawBitmapString("==================================", 20, y); y += 25;

        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Objects detected: " + ofToString(detections.size()), 20, y); y += 20;

        ofSetColor(255, 255, 0);
        ofDrawBitmapString("Inference time: " + ofToString(inferenceTime, 1) + " ms", 20, y); y += 20;

        ofSetColor(200);
        ofDrawBitmapString("Confidence threshold: " + ofToString(confThreshold, 2), 20, y); y += 18;
        ofDrawBitmapString("NMS threshold: " + ofToString(nmsThreshold, 2), 20, y); y += 25;

        ofSetColor(150);
        ofDrawBitmapString("[+/-] Adjust confidence threshold", 20, y); y += 18;
        ofDrawBitmapString("[n/m] Adjust NMS threshold", 20, y);

    } else {
        // Model not loaded
        ofSetColor(0, 180);
        ofFill();
        ofDrawRectangle(10, 10, 500, 220);

        ofSetColor(255, 100, 100);
        int y = 30;
        ofDrawBitmapString("MODEL NOT LOADED!", 20, y); y += 25;

        ofSetColor(255);
        ofDrawBitmapString("Please download YOLOv4-tiny model to bin/data/:", 20, y); y += 20;
        ofDrawBitmapString("  1. yolov4-tiny.cfg", 20, y); y += 18;
        ofDrawBitmapString("  2. yolov4-tiny.weights", 20, y); y += 25;

        ofSetColor(200);
        ofDrawBitmapString("Run the download script:", 20, y); y += 18;
        ofDrawBitmapString("  cd bin/data && ./download_model.sh", 20, y); y += 25;

        ofSetColor(150);
        ofDrawBitmapString("Or download manually from:", 20, y); y += 18;
        ofDrawBitmapString("  https://github.com/AlexeyAB/darknet/releases", 20, y);
    }
}

void ofApp::keyPressed(int key) {
    if(key == '+' || key == '=') {
        confThreshold = std::min(0.95f, confThreshold + 0.05f);
    } else if(key == '-' || key == '_') {
        confThreshold = std::max(0.1f, confThreshold - 0.05f);
    } else if(key == 'n' || key == 'N') {
        nmsThreshold = std::max(0.1f, nmsThreshold - 0.05f);
    } else if(key == 'm' || key == 'M') {
        nmsThreshold = std::min(0.9f, nmsThreshold + 0.05f);
    }
}
