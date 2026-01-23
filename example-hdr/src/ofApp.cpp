#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("HDR Imaging - example-hdr");
    ofSetFrameRate(60);

    hdrComputed = false;
    displayMode = 3; // Start with HDR fused
    gamma = 2.2;
    exposure = 1.0;

    // Generate test images with different exposures
    generateTestImages();

    // Compute HDR fusion
    computeHDR();

    ofLogNotice() << "HDR sample ready";
    ofLogNotice() << "Press 0/1/2/3 to switch between exposure modes";
    ofLogNotice() << "Press UP/DOWN to adjust gamma";
    ofLogNotice() << "Press LEFT/RIGHT to adjust exposure";
}

void ofApp::generateTestImages() {
    // Create a synthetic scene with wide dynamic range
    int width = 800;
    int height = 600;

    // Simulate 3 different exposures: underexposed, normal, overexposed
    exposureTimes = {0.25f, 1.0f, 4.0f};

    for (int i = 0; i < 3; i++) {
        cv::Mat img(height, width, CV_8UC3);

        float exposureMult = exposureTimes[i];

        // Create a scene with bright sky and dark foreground
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float u = (float)x / width;
                float v = (float)y / height;

                // Sky gradient (bright)
                float skyValue = 250.0f * (1.0f - v);

                // Sun (very bright spot)
                float sunDist = std::sqrt(std::pow(u - 0.7f, 2.0f) + std::pow(v - 0.2f, 2.0f));
                float sunValue = 255.0f * std::exp(-sunDist * 10.0f);

                // Ground (dark)
                float groundValue = 50.0f * v;

                // Building silhouette (very dark)
                bool inBuilding = (u > 0.3 && u < 0.5 && v > 0.6);
                float buildingValue = inBuilding ? 10.0f : 0.0f;

                // Combine
                float rawValue = skyValue + sunValue + groundValue - buildingValue;

                // Apply exposure
                float exposedValue = rawValue * exposureMult;
                exposedValue = std::min(255.0f, exposedValue);

                cv::Vec3b color;
                // Sky is bluish, ground is brownish
                if (v < 0.5) {
                    color[0] = (uchar)(exposedValue * 0.8); // B
                    color[1] = (uchar)(exposedValue * 0.9); // G
                    color[2] = (uchar)exposedValue;         // R
                } else {
                    color[0] = (uchar)(exposedValue * 0.6); // B
                    color[1] = (uchar)(exposedValue * 0.8); // G
                    color[2] = (uchar)exposedValue;         // R
                }

                img.at<cv::Vec3b>(y, x) = color;
            }
        }

        images.push_back(img);
    }

    ofLogNotice() << "Generated " << images.size() << " test images with exposures: 0.25s, 1.0s, 4.0s";
}

void ofApp::computeHDR() {
    if (images.empty()) {
        ofLogError() << "No images to process";
        return;
    }

    // Simple exposure fusion algorithm (weighted average based on exposure quality)
    // We'll weight each pixel by how well-exposed it is (avoid over/under exposure)

    int width = images[0].cols;
    int height = images[0].rows;

    // Create HDR image as float mat
    hdrImage = cv::Mat::zeros(height, width, CV_32FC3);
    cv::Mat weightSum = cv::Mat::zeros(height, width, CV_32F);

    // For each image, compute weight based on how close to mid-gray (well-exposed)
    for (size_t i = 0; i < images.size(); i++) {
        cv::Mat floatImg;
        images[i].convertTo(floatImg, CV_32FC3, 1.0 / 255.0);

        // Compute weight for each pixel (how well exposed it is)
        // Weight is higher when pixel is near 0.5 (middle gray)
        cv::Mat gray;
        cv::cvtColor(floatImg, gray, cv::COLOR_BGR2GRAY);

        cv::Mat weight;
        // Gaussian weight centered at 0.5
        cv::subtract(gray, cv::Scalar(0.5), weight);
        cv::multiply(weight, weight, weight);
        cv::multiply(weight, cv::Scalar(-20.0), weight); // -20 * (x - 0.5)^2
        cv::exp(weight, weight);

        // Add small constant to avoid division by zero
        weight += 0.01;

        // Accumulate weighted images
        for (int c = 0; c < 3; c++) {
            cv::Mat channel;
            cv::extractChannel(floatImg, channel, c);
            cv::multiply(channel, weight, channel);
            cv::Mat hdrChannel;
            cv::extractChannel(hdrImage, hdrChannel, c);
            hdrChannel += channel;
            cv::insertChannel(hdrChannel, hdrImage, c);
        }

        weightSum += weight;
    }

    // Normalize by weight sum
    for (int c = 0; c < 3; c++) {
        cv::Mat channel;
        cv::extractChannel(hdrImage, channel, c);
        cv::divide(channel, weightSum, channel);
        cv::insertChannel(channel, hdrImage, c);
    }

    hdrComputed = true;

    // Apply initial tonemap
    applyTonemap();

    ofLogNotice() << "HDR fusion complete: " << hdrImage.cols << "x" << hdrImage.rows;
}

void ofApp::applyTonemap() {
    cv::Mat toDisplay;

    if (displayMode >= 0 && displayMode < 3) {
        // Display one of the LDR images
        toDisplay = images[displayMode].clone();
    } else {
        // Display HDR fused image with tonemapping
        cv::Mat adjusted;
        cv::multiply(hdrImage, cv::Scalar(exposure, exposure, exposure), adjusted);

        // Apply gamma correction
        cv::pow(adjusted, 1.0 / gamma, adjusted);

        // Clamp and convert to 8-bit
        adjusted.convertTo(toDisplay, CV_8UC3, 255.0);
    }

    // Convert BGR to RGB
    cv::cvtColor(toDisplay, toDisplay, cv::COLOR_BGR2RGB);

    // Update ofImage
    displayImage.allocate(toDisplay.cols, toDisplay.rows, OF_IMAGE_COLOR);
    displayImage.setFromPixels(toDisplay.data, toDisplay.cols, toDisplay.rows, OF_IMAGE_COLOR);
    displayImage.update();
}

void ofApp::update() {
    if (hdrComputed) {
        applyTonemap();
    }
}

void ofApp::draw() {
    ofBackground(30);

    if (displayImage.isAllocated()) {
        // Center the image
        float x = (ofGetWidth() - displayImage.getWidth()) / 2.0;
        float y = (ofGetHeight() - displayImage.getHeight()) / 2.0;
        displayImage.draw(x, y);
    }

    // Draw UI
    ofSetColor(255);
    stringstream info;
    info << "HDR Imaging Demo (Exposure Fusion)\n\n";
    info << "Display Mode: ";
    if (displayMode == 0) info << "Underexposed (0.25s)";
    else if (displayMode == 1) info << "Normal (1.0s)";
    else if (displayMode == 2) info << "Overexposed (4.0s)";
    else if (displayMode == 3) info << "HDR Fused (tonemapped)";
    info << "\n";
    info << "Gamma: " << gamma << "\n";
    info << "Exposure: " << exposure << "\n\n";
    info << "Controls:\n";
    info << "0 - Underexposed LDR\n";
    info << "1 - Normal LDR\n";
    info << "2 - Overexposed LDR\n";
    info << "3 - HDR Fused\n";
    info << "UP/DOWN - Adjust gamma\n";
    info << "LEFT/RIGHT - Adjust exposure\n";
    info << "\nFPS: " << ofToString(ofGetFrameRate(), 1);

    ofDrawBitmapStringHighlight(info.str(), 20, 20);
}

void ofApp::keyPressed(int key) {
    if (key == '0') {
        displayMode = 0;
        ofLogNotice() << "Switched to underexposed mode";
    } else if (key == '1') {
        displayMode = 1;
        ofLogNotice() << "Switched to normal exposure mode";
    } else if (key == '2') {
        displayMode = 2;
        ofLogNotice() << "Switched to overexposed mode";
    } else if (key == '3') {
        displayMode = 3;
        ofLogNotice() << "Switched to HDR fused mode";
    } else if (key == OF_KEY_UP) {
        gamma += 0.1;
        gamma = std::min(gamma, 5.0f);
        ofLogNotice() << "Gamma: " << gamma;
    } else if (key == OF_KEY_DOWN) {
        gamma -= 0.1;
        gamma = std::max(gamma, 0.5f);
        ofLogNotice() << "Gamma: " << gamma;
    } else if (key == OF_KEY_RIGHT) {
        exposure += 0.1;
        exposure = std::min(exposure, 3.0f);
        ofLogNotice() << "Exposure: " << exposure;
    } else if (key == OF_KEY_LEFT) {
        exposure -= 0.1;
        exposure = std::max(exposure, 0.1f);
        ofLogNotice() << "Exposure: " << exposure;
    }
}
