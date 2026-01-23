#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "GeometryHelpers.h"

class Recognizer {	
public:
	enum GestureType {GESTURE_LINE, GESTURE_ARC}; 
	
	Recognizer()
	:lineRatio(6)
	,gestureType(GESTURE_LINE)
	,fitError(0)
	{}
	void setLineRatio(float lineRatio) {
		this->lineRatio = lineRatio;
	}
	GestureType getGestureType() const {
		return gestureType;
	}
	float getFitError() const {
		return fitError;
	}
	ofPolyline& getPolyline() {
		return idealized;
	}
	void update(ofPolyline& polyline) {
		if(polyline.size() > 5) {
			ellipse = fitEllipse(polyline);
		}
		rect = minAreaRect(polyline);
		fitLine(polyline, linePoint, lineDirection);
		
		float lineSum = 0, ellipseSum = 0;
		for(int i = 0; i < polyline.size(); i++) {
			lineSum += distanceToRay(polyline[i], linePoint, linePoint + lineDirection);
			ellipseSum += distanceToEllipse(polyline[i], ellipse);
		}
		float perimeter = polyline.getPerimeter();
		lineSum /= perimeter, ellipseSum /= perimeter;
		
		bool isLine = rect.size.width / rect.size.height > lineRatio
		|| rect.size.height / rect.size.width > lineRatio
		|| (lineSum < ellipseSum)
		|| ellipseSum != ellipseSum;
		idealized.clear();
		if(isLine) {
			gestureType = GESTURE_LINE;
			fitError = lineSum;
			glm::vec2 p0 = closestPointOnRay(linePoint, linePoint + lineDirection, polyline[0]);
			glm::vec2 p1 = closestPointOnRay(linePoint, linePoint + lineDirection, polyline[polyline.size() - 1]);
			idealized.addVertex(glm::vec3(p0, 0));
			idealized.addVertex(glm::vec3(p1, 0));
		} else {
			gestureType = GESTURE_ARC;
			fitError = ellipseSum;
			glm::vec2 center(ellipse.center.x, ellipse.center.y);
			// it would make more sense to do this at a fixed resolution
			for(int i = 0; i < polyline.size(); i++) {
				glm::vec2 cur = polyline[i];
				cur -= center;
				float angle = -ellipse.angle * DEG_TO_RAD;
				cur = glm::vec2(cur.x * cos(angle) - cur.y * sin(angle), cur.x * sin(angle) + cur.y * cos(angle));
				float a = ellipse.size.width / 2, b = ellipse.size.height / 2, x0 = cur.x, y0 = cur.y;
				cur *= (a * b) / sqrtf(a * a * y0 * y0 + b * b * x0 * x0);
				angle = ellipse.angle * DEG_TO_RAD;
				cur = glm::vec2(cur.x * cos(angle) - cur.y * sin(angle), cur.x * sin(angle) + cur.y * cos(angle));
				cur += center;
				idealized.addVertex(glm::vec3(cur, 0));
			}
		}
	}
	
protected:
	cv::RotatedRect ellipse, rect;
	glm::vec2 linePoint, lineDirection;
	ofPolyline idealized;
	float lineRatio, fitError;
	GestureType gestureType;
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	
	ofPolyline polyline;
	
	Recognizer recognizer;
};
