#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(30);

    cam.setup(640, 480);

    confidenceThreshold = 0.1f;
    inputWidth = 368;
    inputHeight = 368;
    inferenceTime = 0;
    modelLoaded = false;

    // COCO skeleton connections
    skeleton = {
        {1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7},
        {1, 8}, {8, 9}, {9, 10}, {1, 11}, {11, 12}, {12, 13},
        {1, 0}, {0, 14}, {14, 16}, {0, 15}, {15, 17}
    };

    keypointNames = {
        "Nose", "Neck", "RShoulder", "RElbow", "RWrist",
        "LShoulder", "LElbow", "LWrist", "RHip", "RKnee",
        "RAnkle", "LHip", "LKnee", "LAnkle", "REye",
        "LEye", "REar", "LEar"
    };

    // Load OpenPose model
    std::string modelFile = ofToDataPath("pose_iter_440000.caffemodel");
    std::string configFile = ofToDataPath("pose_deploy_linevec.prototxt");

    if(ofFile::doesFileExist(modelFile) && ofFile::doesFileExist(configFile)) {
        try {
            net = dnn::readNetFromCaffe(configFile, modelFile);
            net.setPreferableBackend(dnn::DNN_BACKEND_DEFAULT);
            net.setPreferableTarget(dnn::DNN_TARGET_CPU);
            modelLoaded = true;
            ofLog() << "Pose estimation model loaded!";
        } catch(const cv::Exception& e) {
            ofLogError() << "Failed to load model: " << e.what();
        }
    } else {
        ofLogError() << "Model files not found! Run download_model.sh";
    }

    ofSetWindowTitle("OpenCV 4.x Pose Estimation");
}

void ofApp::update() {
    cam.update();

    if(cam.isFrameNew() && modelLoaded) {
        Mat frame = toCv(cam);
        int frameWidth = frame.cols;
        int frameHeight = frame.rows;

        Mat blob = dnn::blobFromImage(frame, 1.0/255.0, Size(inputWidth, inputHeight),
                                       Scalar(0, 0, 0), false, false);

        float startTime = ofGetElapsedTimef();
        net.setInput(blob);
        Mat output = net.forward();
        inferenceTime = (ofGetElapsedTimef() - startTime) * 1000;

        processOutput(output, frameWidth, frameHeight);
    }
}

void ofApp::processOutput(const Mat& output, int frameWidth, int frameHeight) {
    detectedPoses.clear();

    int nParts = output.size[1];
    int H = output.size[2];
    int W = output.size[3];

    std::vector<Keypoint> keypoints(nParts);

    for(int n = 0; n < nParts; n++) {
        Mat heatMap(H, W, CV_32F, output.ptr(0, n));

        Point maxLoc;
        double maxVal;
        minMaxLoc(heatMap, nullptr, &maxVal, nullptr, &maxLoc);

        keypoints[n].position = Point2f(
            (float)maxLoc.x * frameWidth / W,
            (float)maxLoc.y * frameHeight / H
        );
        keypoints[n].confidence = (float)maxVal;
    }

    detectedPoses.push_back(keypoints);
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);

    if(modelLoaded && !detectedPoses.empty()) {
        for(const auto& pose : detectedPoses) {
            // Draw skeleton
            ofSetLineWidth(3);
            for(const auto& conn : skeleton) {
                int idxA = conn.first;
                int idxB = conn.second;

                if(idxA < pose.size() && idxB < pose.size() &&
                   pose[idxA].confidence > confidenceThreshold &&
                   pose[idxB].confidence > confidenceThreshold) {
                    ofSetColor(0, 255, 0);
                    ofDrawLine(pose[idxA].position.x, pose[idxA].position.y,
                               pose[idxB].position.x, pose[idxB].position.y);
                }
            }

            // Draw keypoints
            for(size_t i = 0; i < pose.size(); i++) {
                if(pose[i].confidence > confidenceThreshold) {
                    float hue = ofMap(i, 0, pose.size(), 0, 255);
                    ofSetColor(ofColor::fromHsb(hue, 255, 255));
                    ofFill();
                    ofDrawCircle(pose[i].position.x, pose[i].position.y, 5);
                }
            }
        }
    }

    // Info panel
    ofSetColor(0, 180);
    ofFill();
    ofDrawRectangle(10, 10, 300, 120);

    ofSetColor(255);
    int y = 30;
    ofDrawBitmapString("OpenCV 4.x Pose Estimation", 20, y); y += 25;

    if(modelLoaded) {
        ofSetColor(0, 255, 0);
        ofDrawBitmapString("Model loaded", 20, y); y += 20;
        ofDrawBitmapString("Inference: " + ofToString(inferenceTime, 1) + " ms", 20, y); y += 20;
    } else {
        ofSetColor(255, 100, 100);
        ofDrawBitmapString("Model not loaded!", 20, y); y += 20;
        ofDrawBitmapString("Run: cd bin/data && ./download_model.sh", 20, y);
    }
}

void ofApp::keyPressed(int key) {
    if(key == '+' || key == '=') {
        confidenceThreshold = std::min(0.9f, confidenceThreshold + 0.05f);
    } else if(key == '-') {
        confidenceThreshold = std::max(0.0f, confidenceThreshold - 0.05f);
    }
}
