#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(30);
    ofSetWindowTitle("Face Landmark Detection");

    // Initialize camera
    cam.setup(640, 480);

    faceCascadeLoaded = false;
    eyeCascadeLoaded = false;
    noseCascadeLoaded = false;
    mouthCascadeLoaded = false;

    // Load face cascade
    string facePath = ofToDataPath("haarcascade_frontalface_alt2.xml", true);
    if (ofFile::doesFileExist(facePath)) {
        faceCascadeLoaded = faceCascade.load(facePath);
        if (faceCascadeLoaded) {
            ofLogNotice() << "Face cascade loaded successfully";
        }
    } else {
        ofLogWarning() << "Face cascade not found. Run download_model.sh";
    }

    // Load eye cascade
    string eyePath = ofToDataPath("haarcascade_eye.xml", true);
    if (ofFile::doesFileExist(eyePath)) {
        eyeCascadeLoaded = eyeCascade.load(eyePath);
        if (eyeCascadeLoaded) {
            ofLogNotice() << "Eye cascade loaded successfully";
        }
    } else {
        ofLogWarning() << "Eye cascade not found. Run download_model.sh";
    }

    // Load nose cascade
    string nosePath = ofToDataPath("haarcascade_mcs_nose.xml", true);
    if (ofFile::doesFileExist(nosePath)) {
        noseCascadeLoaded = noseCascade.load(nosePath);
        if (noseCascadeLoaded) {
            ofLogNotice() << "Nose cascade loaded successfully";
        }
    } else {
        ofLogWarning() << "Nose cascade not found. Run download_model.sh";
    }

    // Load mouth cascade
    string mouthPath = ofToDataPath("haarcascade_mcs_mouth.xml", true);
    if (ofFile::doesFileExist(mouthPath)) {
        mouthCascadeLoaded = mouthCascade.load(mouthPath);
        if (mouthCascadeLoaded) {
            ofLogNotice() << "Mouth cascade loaded successfully";
        }
    } else {
        ofLogWarning() << "Mouth cascade not found. Run download_model.sh";
    }

    frame.allocate(640, 480, OF_IMAGE_COLOR);
}

void ofApp::update() {
    cam.update();

    if (!cam.isFrameNew() || !faceCascadeLoaded) {
        return;
    }

    // Convert to cv::Mat
    cv::Mat gray = ofxCv::toCv(cam.getPixels());
    cv::cvtColor(gray, gray, cv::COLOR_RGB2GRAY);

    // Detect faces
    faces.clear();
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(60, 60));

    // Detect facial features within each face
    eyes.clear();
    noses.clear();
    mouths.clear();

    for (const auto& face : faces) {
        // Define ROI for face
        cv::Mat faceROI = gray(face);

        // Detect eyes in face region
        if (eyeCascadeLoaded) {
            std::vector<cv::Rect> faceEyes;
            eyeCascade.detectMultiScale(faceROI, faceEyes, 1.1, 3, 0, cv::Size(20, 20));
            for (auto& eye : faceEyes) {
                eye.x += face.x;
                eye.y += face.y;
                eyes.push_back(eye);
            }
        }

        // Detect nose in face region
        if (noseCascadeLoaded) {
            std::vector<cv::Rect> faceNoses;
            noseCascade.detectMultiScale(faceROI, faceNoses, 1.1, 3, 0, cv::Size(20, 20));
            for (auto& nose : faceNoses) {
                nose.x += face.x;
                nose.y += face.y;
                noses.push_back(nose);
            }
        }

        // Detect mouth in face region
        if (mouthCascadeLoaded) {
            std::vector<cv::Rect> faceMouths;
            mouthCascade.detectMultiScale(faceROI, faceMouths, 1.1, 3, 0, cv::Size(20, 20));
            for (auto& mouth : faceMouths) {
                mouth.x += face.x;
                mouth.y += face.y;
                mouths.push_back(mouth);
            }
        }
    }

    // Update frame for drawing
    frame.setFromPixels(cam.getPixels());
}

void ofApp::draw() {
    ofSetColor(255);
    frame.draw(0, 0);

    if (!faceCascadeLoaded) {
        ofSetColor(255, 100, 100);
        ofDrawBitmapString("Models not loaded. Run download_model.sh", 20, 30);
        return;
    }

    // Draw face rectangles
    ofNoFill();
    ofSetColor(0, 255, 0);
    ofSetLineWidth(2);
    for (const auto& face : faces) {
        ofDrawRectangle(face.x, face.y, face.width, face.height);
    }

    // Draw eyes
    ofSetColor(255, 255, 0);
    ofSetLineWidth(1);
    for (const auto& eye : eyes) {
        ofDrawRectangle(eye.x, eye.y, eye.width, eye.height);
        ofFill();
        ofDrawCircle(eye.x + eye.width/2, eye.y + eye.height/2, 3);
        ofNoFill();
    }

    // Draw noses
    ofSetColor(0, 255, 255);
    for (const auto& nose : noses) {
        ofDrawRectangle(nose.x, nose.y, nose.width, nose.height);
        ofFill();
        ofDrawCircle(nose.x + nose.width/2, nose.y + nose.height/2, 3);
        ofNoFill();
    }

    // Draw mouths
    ofSetColor(255, 0, 255);
    for (const auto& mouth : mouths) {
        ofDrawRectangle(mouth.x, mouth.y, mouth.width, mouth.height);
        ofFill();
        ofDrawCircle(mouth.x + mouth.width/2, mouth.y + mouth.height/2, 3);
        ofNoFill();
    }

    // Draw info
    ofSetColor(255);
    string info = "FPS: " + ofToString(ofGetFrameRate(), 1);
    info += "\nFaces: " + ofToString(faces.size());
    info += " | Eyes: " + ofToString(eyes.size());
    info += " | Noses: " + ofToString(noses.size());
    info += " | Mouths: " + ofToString(mouths.size());
    ofDrawBitmapStringHighlight(info, 20, 20);
}
