#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	ofSetVerticalSync(true);

	ofMatrix4x4 rigid;
	glm::vec3 translation(ofRandomf(), ofRandomf(), ofRandomf());
	ofQuaternion rotation(ofRandomf(), glm::vec3(ofRandomf(), ofRandomf(), ofRandomf()));
	rigid.translate(translation);
	rigid.rotate(rotation);

	vector<glm::vec3> from;
	for(int i = 0; i < 4; i++) {
		from.push_back(glm::vec3(ofRandom(1,2), ofRandom(1,2), ofRandom(5,6)));
		from.push_back(glm::vec3(ofRandom(3,4), ofRandom(3,4), ofRandom(5,6)));
		from.push_back(glm::vec3(ofRandom(1,2), ofRandom(3,4), ofRandom(5,6)));
		from.push_back(glm::vec3(ofRandom(3,4), ofRandom(1,2), ofRandom(5,6)));
	}

	vector<glm::vec3> to;
	for(int i = 0; i < from.size(); i++) {
		// opencv assumes you're doing premultiplication
		ofVec4f result = rigid.preMult(ofVec4f(from[i].x, from[i].y, from[i].z, 1.0f));
		to.push_back(glm::vec3(result.x, result.y, result.z));
	}

	ofMatrix4x4 rigidEstimate = estimateAffine3D(from, to);

	cout << "original matrix: " << endl << rigid << endl;
	cout << "estimated as: " << endl << rigidEstimate << endl;

	for(int i = 0; i < from.size(); i++) {
		// opencv assumes you're doing premultiplication
		ofVec4f result = rigidEstimate.preMult(ofVec4f(from[i].x, from[i].y, from[i].z, 1.0f));
		glm::vec3 after(result.x, result.y, result.z);
		cout << from[i] << " -> " << to[i] << " estimated as: " << after << endl;
	}

	ofVec3f decompTranslation, decompScale;
	ofQuaternion decompRotation, decompSo;
	rigidEstimate.decompose(decompTranslation, decompRotation, decompScale, decompSo);
	cout << "translation: " << translation << endl;
	cout << "estimated as: " << decompTranslation << endl;
	cout << "rotation: " << endl << rotation << endl;
	cout << "estimated as: " << endl << decompRotation << endl;
}

void ofApp::update() {
}

void ofApp::draw() {
	ofBackground(0);
	ofDrawBitmapString("See console window for results.", 10, 20);
}
