#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetWindowTitle("Document Scanner - OpenCV 4.x cv::getPerspectiveTransform");

    camera.setup(640, 480);

    selectedCorner = -1;
    manualMode = false;

    // Initialize corners to screen corners
    corners.resize(4);
    corners[0] = Point2f(100, 100);    // top-left
    corners[1] = Point2f(540, 100);    // top-right
    corners[2] = Point2f(540, 380);    // bottom-right
    corners[3] = Point2f(100, 380);    // bottom-left
}

void ofApp::update() {
    camera.update();

    if (camera.isFrameNew()) {
        frame = toCv(camera);

        if (!manualMode) {
            detectDocument();
        }

        warpPerspective();
    }
}

bool ofApp::detectDocument() {
    // Convert to grayscale
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Gaussian blur to reduce noise
    GaussianBlur(gray, gray, Size(5, 5), 0);

    // Canny edge detection
    Canny(gray, edges, 50, 150);

    // Morphological closing to close gaps in edges
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(edges, morphed, MORPH_CLOSE, kernel);

    // Find contours
    vector<vector<Point>> contours;
    findContours(morphed, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return false;
    }

    // Get quadrilateral from largest contour
    vector<Point2f> quad = getQuadrilateral(contours);

    if (quad.size() == 4) {
        corners = orderPoints(quad);
        return true;
    }

    return false;
}

vector<Point2f> ofApp::getQuadrilateral(const vector<vector<Point>>& contours) {
    // Find largest contour by area
    double maxArea = 0;
    int maxIdx = -1;

    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > maxArea && area > 1000) {  // Minimum area threshold
            maxArea = area;
            maxIdx = i;
        }
    }

    if (maxIdx < 0) {
        return vector<Point2f>();
    }

    // Approximate contour to polygon
    vector<Point> approx;
    double epsilon = 0.02 * arcLength(contours[maxIdx], true);
    approxPolyDP(contours[maxIdx], approx, epsilon, true);

    // Check if we have a quadrilateral
    if (approx.size() == 4) {
        vector<Point2f> result;
        for (const auto& pt : approx) {
            result.push_back(Point2f(pt.x, pt.y));
        }
        return result;
    }

    return vector<Point2f>();
}

vector<Point2f> ofApp::orderPoints(const vector<Point2f>& pts) {
    if (pts.size() != 4) {
        return pts;
    }

    vector<Point2f> ordered(4);

    // Sum and diff to find corners
    vector<float> sums, diffs;
    for (const auto& pt : pts) {
        sums.push_back(pt.x + pt.y);
        diffs.push_back(pt.y - pt.x);
    }

    // Top-left has smallest sum
    auto minSum = min_element(sums.begin(), sums.end());
    ordered[0] = pts[std::distance(sums.begin(), minSum)];

    // Bottom-right has largest sum
    auto maxSum = max_element(sums.begin(), sums.end());
    ordered[2] = pts[std::distance(sums.begin(), maxSum)];

    // Top-right has smallest diff
    auto minDiff = min_element(diffs.begin(), diffs.end());
    ordered[1] = pts[std::distance(diffs.begin(), minDiff)];

    // Bottom-left has largest diff
    auto maxDiff = max_element(diffs.begin(), diffs.end());
    ordered[3] = pts[std::distance(diffs.begin(), maxDiff)];

    return ordered;
}

void ofApp::warpPerspective() {
    if (corners.size() != 4) {
        return;
    }

    // Calculate output dimensions
    float widthTop = pointDistance(corners[0], corners[1]);
    float widthBottom = pointDistance(corners[2], corners[3]);
    float width = max(widthTop, widthBottom);

    float heightLeft = pointDistance(corners[0], corners[3]);
    float heightRight = pointDistance(corners[1], corners[2]);
    float height = max(heightLeft, heightRight);

    // Destination points (A4 ratio: ~1.414)
    vector<Point2f> dst;
    dst.push_back(Point2f(0, 0));
    dst.push_back(Point2f(width - 1, 0));
    dst.push_back(Point2f(width - 1, height - 1));
    dst.push_back(Point2f(0, height - 1));

    // Get perspective transform matrix
    Mat M = getPerspectiveTransform(corners, dst);

    // Apply warp
    cv::warpPerspective(frame, warped, M, Size(width, height));

    // Convert to ofImage for display
    toOf(warped, warpedImg);
    warpedImg.update();
}

void ofApp::draw() {
    ofBackground(40);

    // Draw camera feed
    camera.draw(0, 0);

    // Draw detected corners
    ofSetColor(0, 255, 0);
    ofSetLineWidth(2);

    if (corners.size() == 4) {
        for (int i = 0; i < 4; i++) {
            int next = (i + 1) % 4;
            ofDrawLine(corners[i].x, corners[i].y, corners[next].x, corners[next].y);
        }

        // Draw corner handles
        ofFill();
        for (int i = 0; i < 4; i++) {
            if (i == selectedCorner) {
                ofSetColor(255, 0, 0);
            } else {
                ofSetColor(0, 255, 0);
            }
            ofDrawCircle(corners[i].x, corners[i].y, 8);
        }
    }

    // Draw warped document
    if (warpedImg.isAllocated()) {
        float scale = 240.0f / warpedImg.getHeight();
        float w = warpedImg.getWidth() * scale;
        float h = 240;
        warpedImg.draw(650, 10, w, h);

        ofSetColor(255);
        ofDrawBitmapString("Warped Document", 650, h + 30);
    }

    // Instructions
    ofSetColor(255);
    ofDrawBitmapString("Press 'm' to toggle manual mode", 10, 500);
    ofDrawBitmapString("Press 's' to save warped image", 10, 520);
    ofDrawBitmapString("Press 'a' to toggle auto-detect", 10, 540);

    string mode = manualMode ? "MANUAL" : "AUTO";
    ofDrawBitmapString("Mode: " + mode, 10, 570);
}

void ofApp::keyPressed(int key) {
    if (key == 'm') {
        manualMode = !manualMode;
    }
    else if (key == 's' && warpedImg.isAllocated()) {
        warpedImg.save("scanned_" + ofGetTimestampString() + ".png");
        ofLogNotice() << "Saved warped image";
    }
    else if (key == 'a') {
        manualMode = false;
        detectDocument();
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (!manualMode) return;

    // Find closest corner
    selectedCorner = -1;
    float minDist = 20;  // Selection threshold

    for (int i = 0; i < corners.size(); i++) {
        float d = pointDistance(Point2f(x, y), corners[i]);
        if (d < minDist) {
            minDist = d;
            selectedCorner = i;
        }
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if (manualMode && selectedCorner >= 0) {
        corners[selectedCorner] = Point2f(x, y);
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    selectedCorner = -1;
}

float ofApp::pointDistance(const Point2f& a, const Point2f& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}
