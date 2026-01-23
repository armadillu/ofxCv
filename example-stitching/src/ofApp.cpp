#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("OpenCV Panorama Stitching Example (Manual)");
    ofSetFrameRate(60);

    stitchingDone = false;
    stitchingFailed = false;

    loadImages();

    if (inputImages.size() >= 2) {
        statusMessage = "Press SPACE to stitch images (manual feature-based stitching)";
    } else {
        statusMessage = "Need at least 2 images in bin/data/ named image1.jpg, image2.jpg, ...";
    }

    ofLogNotice() << "Loaded " << inputImages.size() << " images";
}

void ofApp::loadImages() {
    // Try to load sequentially numbered images
    int imageIndex = 1;
    while (true) {
        string filename = "image" + ofToString(imageIndex) + ".jpg";
        ofImage img;
        if (img.load(filename)) {
            inputImages.push_back(img);
            ofLogNotice() << "Loaded: " << filename;
            imageIndex++;
        } else {
            // Also try .png extension
            filename = "image" + ofToString(imageIndex) + ".png";
            if (img.load(filename)) {
                inputImages.push_back(img);
                ofLogNotice() << "Loaded: " << filename;
                imageIndex++;
            } else {
                break;
            }
        }
    }
}

void ofApp::performStitching() {
    if (inputImages.size() < 2) {
        statusMessage = "Need at least 2 images to stitch";
        stitchingFailed = true;
        return;
    }

    statusMessage = "Stitching in progress...";
    ofLogNotice() << "Starting manual panorama stitching with " << inputImages.size() << " images";

    try {
        // Convert first two images (simple 2-image stitching for demo)
        cv::Mat img1 = ofxCv::toCv(inputImages[0]);
        cv::Mat img2 = ofxCv::toCv(inputImages[1]);

        // Find homography between images
        cv::Mat H;
        if (!findHomography(img1, img2, H)) {
            statusMessage = "Error: Can't find matching features between images";
            stitchingFailed = true;
            ofLogError() << "Failed to find homography";
            return;
        }

        // Warp and blend images
        panoramaMat = warpAndBlend(img1, img2, H);

        // Convert result back to ofImage
        ofxCv::toOf(panoramaMat, panorama);
        panorama.update();

        stitchingDone = true;
        stitchingFailed = false;
        statusMessage = "Stitching successful! Press 's' to save result";
        ofLogNotice() << "Panorama created: " << panoramaMat.cols << "x" << panoramaMat.rows;
    } catch (cv::Exception& e) {
        statusMessage = "Error: " + string(e.what());
        stitchingFailed = true;
        ofLogError() << "OpenCV Exception: " << e.what();
    }
}

bool ofApp::findHomography(const cv::Mat& img1, const cv::Mat& img2, cv::Mat& H) {
    // Convert to grayscale
    cv::Mat gray1, gray2;
    cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    // Detect ORB features
    cv::Ptr<cv::ORB> detector = cv::ORB::create(2000);
    vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;

    detector->detectAndCompute(gray1, cv::noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(gray2, cv::noArray(), keypoints2, descriptors2);

    if (keypoints1.size() < 10 || keypoints2.size() < 10) {
        ofLogError() << "Not enough keypoints detected";
        return false;
    }

    // Match features
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    vector<cv::DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    // Filter good matches
    double minDist = 100.0;
    for (size_t i = 0; i < matches.size(); i++) {
        double dist = matches[i].distance;
        if (dist < minDist) minDist = dist;
    }

    vector<cv::DMatch> goodMatches;
    for (size_t i = 0; i < matches.size(); i++) {
        if (matches[i].distance <= std::max(2.0 * minDist, 30.0)) {
            goodMatches.push_back(matches[i]);
        }
    }

    ofLogNotice() << "Found " << goodMatches.size() << " good matches out of " << matches.size();

    if (goodMatches.size() < 10) {
        ofLogError() << "Not enough good matches";
        return false;
    }

    // Extract matched point coordinates
    vector<cv::Point2f> points1, points2;
    for (size_t i = 0; i < goodMatches.size(); i++) {
        points1.push_back(keypoints1[goodMatches[i].queryIdx].pt);
        points2.push_back(keypoints2[goodMatches[i].trainIdx].pt);
    }

    // Find homography using RANSAC
    H = cv::findHomography(points2, points1, cv::RANSAC, 5.0);

    return !H.empty();
}

cv::Mat ofApp::warpAndBlend(const cv::Mat& img1, const cv::Mat& img2, const cv::Mat& H) {
    // Calculate size of output panorama
    vector<cv::Point2f> corners2(4);
    corners2[0] = cv::Point2f(0, 0);
    corners2[1] = cv::Point2f(img2.cols, 0);
    corners2[2] = cv::Point2f(img2.cols, img2.rows);
    corners2[3] = cv::Point2f(0, img2.rows);

    vector<cv::Point2f> warpedCorners(4);
    cv::perspectiveTransform(corners2, warpedCorners, H);

    // Find bounding box
    float minX = 0, maxX = img1.cols;
    float minY = 0, maxY = img1.rows;
    for (int i = 0; i < 4; i++) {
        minX = std::min(minX, warpedCorners[i].x);
        maxX = std::max(maxX, warpedCorners[i].x);
        minY = std::min(minY, warpedCorners[i].y);
        maxY = std::max(maxY, warpedCorners[i].y);
    }

    // Create translation matrix to shift everything to positive coordinates
    cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
    T.at<double>(0, 2) = -minX;
    T.at<double>(1, 2) = -minY;

    // Warp second image
    int width = static_cast<int>(maxX - minX);
    int height = static_cast<int>(maxY - minY);
    cv::Mat warped;
    cv::warpPerspective(img2, warped, T * H, cv::Size(width, height));

    // Copy first image into panorama
    cv::Mat panorama = warped.clone();
    cv::Mat roi = panorama(cv::Rect(-minX, -minY, img1.cols, img1.rows));
    img1.copyTo(roi);

    return panorama;
}

void ofApp::update() {
    // Nothing to update continuously
}

void ofApp::draw() {
    ofBackground(50);

    if (stitchingDone) {
        // Display panorama result
        float scale = std::min(ofGetWidth() / panorama.getWidth(),
                               ofGetHeight() / panorama.getHeight());
        float w = panorama.getWidth() * scale;
        float h = panorama.getHeight() * scale;
        float x = (ofGetWidth() - w) * 0.5;
        float y = (ofGetHeight() - h) * 0.5;

        panorama.draw(x, y, w, h);
    } else {
        // Display input images in a grid
        int numImages = inputImages.size();
        if (numImages > 0) {
            int cols = ceil(sqrt(numImages));
            int rows = ceil((float)numImages / cols);

            float cellWidth = ofGetWidth() / cols;
            float cellHeight = ofGetHeight() / rows;

            for (int i = 0; i < numImages; i++) {
                int col = i % cols;
                int row = i / cols;

                float x = col * cellWidth;
                float y = row * cellHeight;

                float scale = std::min(cellWidth / inputImages[i].getWidth(),
                                       cellHeight / inputImages[i].getHeight()) * 0.9;

                float w = inputImages[i].getWidth() * scale;
                float h = inputImages[i].getHeight() * scale;

                float offsetX = x + (cellWidth - w) * 0.5;
                float offsetY = y + (cellHeight - h) * 0.5;

                inputImages[i].draw(offsetX, offsetY, w, h);
            }
        }
    }

    // Draw status message
    ofSetColor(255);
    ofDrawBitmapStringHighlight(statusMessage, 10, 20);

    // Draw instructions
    string instructions = "Controls:\n  SPACE: Stitch images\n  's': Save panorama\n  'r': Reset";
    ofDrawBitmapStringHighlight(instructions, 10, ofGetHeight() - 60);
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        if (!stitchingDone) {
            performStitching();
        }
    } else if (key == 's') {
        if (stitchingDone) {
            string filename = "panorama_" + ofGetTimestampString() + ".jpg";
            panorama.save(filename);
            statusMessage = "Saved: " + filename;
            ofLogNotice() << "Saved panorama: " << filename;
        }
    } else if (key == 'r') {
        stitchingDone = false;
        stitchingFailed = false;
        statusMessage = "Press SPACE to stitch images";
    }
}
