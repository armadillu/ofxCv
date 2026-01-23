#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);

    cam.setup(1280, 720);

    confidenceThreshold = 0.5f;
    showConfidence = true;
    inferenceTime = 0;
    modelLoaded = false;

    // Load DNN face detection model
    // Using OpenCV's pre-trained SSD ResNet10 face detector
    std::string modelFile = ofToDataPath("res10_300x300_ssd_iter_140000.caffemodel");
    std::string configFile = ofToDataPath("deploy.prototxt");

    if(ofFile::doesFileExist(modelFile) && ofFile::doesFileExist(configFile)) {
        try {
            faceNet = dnn::readNetFromCaffe(configFile, modelFile);

            // Use CPU backend (can change to CUDA if available)
            faceNet.setPreferableBackend(dnn::DNN_BACKEND_DEFAULT);
            faceNet.setPreferableTarget(dnn::DNN_TARGET_CPU);

            modelLoaded = true;
            ofLog() << "DNN face detection model loaded successfully!";
        } catch(const cv::Exception& e) {
            ofLogError() << "Failed to load DNN model: " << e.what();
        }
    } else {
        ofLogError() << "Model files not found!";
        ofLogError() << "Please download the model files:";
        ofLogError() << "  1. deploy.prototxt";
        ofLogError() << "  2. res10_300x300_ssd_iter_140000.caffemodel";
        ofLogError() << "";
        ofLogError() << "Download from:";
        ofLogError() << "  https://github.com/opencv/opencv/tree/master/samples/dnn/face_detector";
        ofLogError() << "";
        ofLogError() << "Or run: cd bin/data && ./download_model.sh";
    }

    ofSetWindowTitle("OpenCV 4.x DNN Face Detection");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew() && modelLoaded) {
        Mat frame = toCv(cam);

        // Get frame dimensions
        int frameHeight = frame.rows;
        int frameWidth = frame.cols;

        // Create blob from image
        // The face detector expects 300x300 input
        Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
                                       Scalar(104.0, 177.0, 123.0), false, false);

        // Set input and run forward pass
        float startTime = ofGetElapsedTimef();
        faceNet.setInput(blob);
        Mat detection = faceNet.forward();
        inferenceTime = (ofGetElapsedTimef() - startTime) * 1000; // ms

        // Parse detections
        // Output shape: [1, 1, N, 7] where N is number of detections
        // Each detection: [batch_id, class_id, confidence, left, top, right, bottom]
        faces.clear();

        Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        for(int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            if(confidence > confidenceThreshold) {
                // Get bounding box coordinates (normalized 0-1)
                int left = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
                int top = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
                int right = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
                int bottom = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);

                // Clamp to frame bounds
                left = std::max(0, std::min(left, frameWidth - 1));
                top = std::max(0, std::min(top, frameHeight - 1));
                right = std::max(0, std::min(right, frameWidth - 1));
                bottom = std::max(0, std::min(bottom, frameHeight - 1));

                FaceDetection face;
                face.box = Rect(left, top, right - left, bottom - top);
                face.confidence = confidence;
                faces.push_back(face);
            }
        }
    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    if(modelLoaded) {
        // Draw face detections
        for(const auto& face : faces) {
            // Draw bounding box
            ofNoFill();
            ofSetLineWidth(3);

            // Color based on confidence
            float hue = ofMap(face.confidence, 0.5, 1.0, 0, 120); // red to green
            ofSetColor(ofColor::fromHsb(hue, 255, 255));

            ofDrawRectangle(face.box.x, face.box.y, face.box.width, face.box.height);

            // Draw confidence
            if(showConfidence) {
                ofFill();
                ofSetColor(0, 180);
                ofDrawRectangle(face.box.x, face.box.y - 25, 80, 22);

                ofSetColor(255, 255, 0);
                ofDrawBitmapString(ofToString(face.confidence * 100, 1) + "%",
                                   face.box.x + 5, face.box.y - 8);
            }
        }

        // Draw info panel
        ofSetColor(0, 180);
        ofFill();
        ofDrawRectangle(10, 10, 350, 140);

        ofSetColor(255);
        int y = 30;
        ofDrawBitmapString("OpenCV 4.x DNN Face Detection", 20, y); y += 20;
        ofDrawBitmapString("================================", 20, y); y += 25;

        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Faces detected: " + ofToString(faces.size()), 20, y); y += 20;

        ofSetColor(255, 255, 0);
        ofDrawBitmapString("Inference time: " + ofToString(inferenceTime, 1) + " ms", 20, y); y += 20;

        ofSetColor(200);
        ofDrawBitmapString("Confidence threshold: " + ofToString(confidenceThreshold, 2), 20, y); y += 20;

        ofSetColor(150);
        ofDrawBitmapString("[+/-] Adjust threshold  [c] Toggle confidence", 20, y);

    } else {
        // Model not loaded - show error
        ofSetColor(0, 180);
        ofFill();
        ofDrawRectangle(10, 10, 500, 200);

        ofSetColor(255, 100, 100);
        int y = 30;
        ofDrawBitmapString("MODEL NOT LOADED!", 20, y); y += 25;

        ofSetColor(255);
        ofDrawBitmapString("Please download the model files to bin/data/:", 20, y); y += 20;
        ofDrawBitmapString("  1. deploy.prototxt", 20, y); y += 18;
        ofDrawBitmapString("  2. res10_300x300_ssd_iter_140000.caffemodel", 20, y); y += 25;

        ofSetColor(200);
        ofDrawBitmapString("Download from:", 20, y); y += 18;
        ofDrawBitmapString("  https://github.com/opencv/opencv/tree/master/", 20, y); y += 15;
        ofDrawBitmapString("  samples/dnn/face_detector", 20, y); y += 25;

        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Or run: ./download_model.sh in bin/data/", 20, y);
    }
}

void ofApp::keyPressed(int key) {
    if(key == '+' || key == '=') {
        confidenceThreshold = std::min(0.99f, confidenceThreshold + 0.05f);
    } else if(key == '-' || key == '_') {
        confidenceThreshold = std::max(0.1f, confidenceThreshold - 0.05f);
    } else if(key == 'c' || key == 'C') {
        showConfidence = !showConfidence;
    }
}
