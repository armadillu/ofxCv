#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

glm::vec2 closestPointOnRay(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
glm::vec2 closestPointOnLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
glm::vec2 closestPointOnRect(const cv::RotatedRect& rect, const glm::vec2& point);
glm::vec2 closestPointOnCircle(const glm::vec2& center, float radius, const glm::vec2& point);
glm::vec2 closestPointOnEllipse(const cv::RotatedRect& ellipse, const glm::vec2& point);

float distanceToEllipse(const glm::vec2& point, const cv::RotatedRect& ellipse);
float distanceToRect(const glm::vec2& point, const cv::RotatedRect& rect);
float distanceToRay(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end);
float distanceToLine(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end);
