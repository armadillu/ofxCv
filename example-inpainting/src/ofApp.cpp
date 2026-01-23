#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Image Inpainting - cv::inpaint");
    ofSetFrameRate(60);

    // Load sample image or create a test image
    if (!originalImg.load("sample.jpg")) {
        // Create a test image with gradient
        originalImg.allocate(640, 480, OF_IMAGE_COLOR);
        unsigned char* pixels = originalImg.getPixels().getData();
        for (int y = 0; y < 480; y++) {
            for (int x = 0; x < 640; x++) {
                int idx = (y * 640 + x) * 3;
                pixels[idx] = (x * 255) / 640;     // R
                pixels[idx + 1] = (y * 255) / 480; // G
                pixels[idx + 2] = 128;             // B
            }
        }
        originalImg.update();
    }

    // Create mask image (white on black)
    maskImg.allocate(originalImg.getWidth(), originalImg.getHeight(), OF_IMAGE_GRAYSCALE);
    maskImg.getPixels().setColor(ofColor::black);
    maskImg.update();

    // Convert to cv::Mat
    originalMat = ofxCv::toCv(originalImg);
    maskMat = ofxCv::toCv(maskImg);

    // Initialize result
    inpaintedImg.allocate(originalImg.getWidth(), originalImg.getHeight(), OF_IMAGE_COLOR);
    resultMat = originalMat.clone();

    drawing = false;
    brushSize = 20;
    needsInpaint = false;
    inpaintRadius = 5;

    ofLog() << "Image Inpainting Example (Morphological Fill)";
    ofLog() << "Instructions:";
    ofLog() << "- Drag mouse to mark areas to inpaint (white mask)";
    ofLog() << "- Press SPACE to perform inpainting";
    ofLog() << "- Press 'c' to clear mask";
    ofLog() << "- Press +/- to change brush size";
    ofLog() << "- Press 'r' to increase inpaint radius";
    ofLog() << "Inpaint radius: " << inpaintRadius;
}

void ofApp::update() {
    if (needsInpaint) {
        performInpainting();
        needsInpaint = false;
    }
}

void ofApp::draw() {
    ofSetColor(255);

    // Draw original image (left)
    originalImg.draw(0, 0, 320, 240);
    ofDrawBitmapStringHighlight("Original", 10, 15);

    // Draw mask (top right)
    maskImg.draw(320, 0, 320, 240);
    ofDrawBitmapStringHighlight("Mask (draw here)", 330, 15);

    // Draw inpainted result (bottom)
    inpaintedImg.draw(0, 240, 640, 480);
    ofDrawBitmapStringHighlight("Inpainted Result", 10, 255);

    // Draw instructions
    ofSetColor(255);
    string info = "Radius: " + ofToString(inpaintRadius) + " | Brush: " + ofToString(brushSize);
    info += " | SPACE=inpaint | c=clear | r=radius | +/-=brush";
    ofDrawBitmapStringHighlight(info, 10, ofGetHeight() - 10);

    // Draw brush preview at mouse
    if (ofGetMouseX() >= 320 && ofGetMouseX() < 640 &&
        ofGetMouseY() >= 0 && ofGetMouseY() < 240) {
        ofNoFill();
        ofSetColor(255, 0, 0);
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), brushSize);
    }
}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        needsInpaint = true;
    } else if (key == 'c') {
        // Clear mask
        maskImg.getPixels().setColor(ofColor::black);
        maskImg.update();
        maskMat = ofxCv::toCv(maskImg);
        ofLog() << "Mask cleared";
    } else if (key == 'r') {
        // Increase inpaint radius
        inpaintRadius = std::min(inpaintRadius + 2, 20);
        ofLog() << "Inpaint radius: " << inpaintRadius;
    } else if (key == '+' || key == '=') {
        brushSize = std::min(brushSize + 5, 100);
        ofLog() << "Brush size: " << brushSize;
    } else if (key == '-' || key == '_') {
        brushSize = std::max(brushSize - 5, 5);
        ofLog() << "Brush size: " << brushSize;
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if (drawing && x >= 320 && x < 640 && y >= 0 && y < 240) {
        // Draw on mask (map to original size)
        float maskX = (x - 320) * maskImg.getWidth() / 320.0f;
        float maskY = y * maskImg.getHeight() / 240.0f;

        // Draw white circle on mask
        ofPixels& pixels = maskImg.getPixels();
        for (int dy = -brushSize; dy <= brushSize; dy++) {
            for (int dx = -brushSize; dx <= brushSize; dx++) {
                if (dx*dx + dy*dy <= brushSize*brushSize) {
                    int px = maskX + dx;
                    int py = maskY + dy;
                    if (px >= 0 && px < maskImg.getWidth() &&
                        py >= 0 && py < maskImg.getHeight()) {
                        pixels.setColor(px, py, ofColor::white);
                    }
                }
            }
        }
        maskImg.update();
        maskMat = ofxCv::toCv(maskImg);
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (x >= 320 && x < 640 && y >= 0 && y < 240) {
        drawing = true;
        mouseDragged(x, y, button); // Start drawing immediately
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    drawing = false;
}

void ofApp::performInpainting() {
    // Check if mask has any white pixels
    if (cv::countNonZero(maskMat) == 0) {
        ofLog() << "No mask drawn - nothing to inpaint";
        return;
    }

    // Perform morphological inpainting (hole filling)
    // This is a simple inpainting technique using dilate and erode
    try {
        resultMat = originalMat.clone();

        // Invert mask (OpenCV dilate/erode need inverted mask)
        cv::Mat invertedMask;
        cv::bitwise_not(maskMat, invertedMask);

        // Create structuring element for morphological operations
        cv::Mat kernel = cv::getStructuringElement(
            cv::MORPH_ELLIPSE,
            cv::Size(inpaintRadius * 2 + 1, inpaintRadius * 2 + 1)
        );

        // Dilate the surrounding area into the masked region
        cv::Mat dilated;
        cv::dilate(resultMat, dilated, kernel);

        // Apply only to masked areas
        dilated.copyTo(resultMat, maskMat);

        // Apply Gaussian blur to smooth the inpainted area
        cv::Mat blurred;
        cv::GaussianBlur(resultMat, blurred, cv::Size(inpaintRadius * 2 + 1, inpaintRadius * 2 + 1), 0);
        blurred.copyTo(resultMat, maskMat);

        ofxCv::toOf(resultMat, inpaintedImg);
        inpaintedImg.update();
        ofLog() << "Inpainting completed (morphological fill)";
    } catch (cv::Exception& e) {
        ofLog() << "Inpainting failed: " << e.what();
    }
}
