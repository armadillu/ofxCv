#include "GeometryHelpers.h"

using namespace ofxCv;
using namespace cv;

// Helper function to rotate a 2D vector
inline glm::vec2 rotateVec2(const glm::vec2& v, float angleDeg) {
    float angleRad = angleDeg * DEG_TO_RAD;
    float c = cos(angleRad);
    float s = sin(angleRad);
    return glm::vec2(v.x * c - v.y * s, v.x * s + v.y * c);
}

template <typename Real>
Real DistancePointEllipseSpecial (const Real e[2], const Real y[2], Real x[2])
{
    Real distance;
    if (y[1] > (Real)0)
    {
        if (y[0] > (Real)0)
        {
            // Bisect to compute the root of F(t) for t >= -e1*e1.
            Real esqr[2] = { e[0]*e[0], e[1]*e[1] };
            Real ey[2] = { e[0]*y[0], e[1]*y[1] };
            Real t0 = -esqr[1] + ey[1];
            Real t1 = -esqr[1] + sqrt(ey[0]*ey[0] + ey[1]*ey[1]);
            Real t = t0;
            const int imax = 2*std::numeric_limits<Real>::max_exponent;
            for (int i = 0; i < imax; ++i)
            {
                t = ((Real)0.5)*(t0 + t1);
                if (t == t0 || t == t1)
                {
                    break;
                }

                Real r[2] = { ey[0]/(t + esqr[0]), ey[1]/(t + esqr[1]) };
                Real f = r[0]*r[0] + r[1]*r[1] - (Real)1;
                if (f > (Real)0)
                {
                    t0 = t;
                }
                else if (f < (Real)0)
                {
                    t1 = t;
                }
                else
                {
                    break;
                }
            }

            x[0] = esqr[0]*y[0]/(t + esqr[0]);
            x[1] = esqr[1]*y[1]/(t + esqr[1]);
            Real d[2] = { x[0] - y[0], x[1] - y[1] };
            distance = sqrt(d[0]*d[0] + d[1]*d[1]);
        }
        else  // y0 == 0
        {
            x[0] = (Real)0;
            x[1] = e[1];
            distance = fabs(y[1] - e[1]);
        }
    }
    else  // y1 == 0
    {
        Real denom0 = e[0]*e[0] - e[1]*e[1];
        Real e0y0 = e[0]*y[0];
        if (e0y0 < denom0)
        {
            // y0 is inside the subinterval.
            Real x0de0 = e0y0/denom0;
            Real x0de0sqr = x0de0*x0de0;
            x[0] = e[0]*x0de0;
            x[1] = e[1]*sqrt(fabs((Real)1 - x0de0sqr));
            Real d0 = x[0] - y[0];
            distance = sqrt(d0*d0 + x[1]*x[1]);
        }
        else
        {
            // y0 is outside the subinterval.  The closest ellipse point has
            // x1 == 0 and is on the domain-boundary interval (x0/e0)^2 = 1.
            x[0] = e[0];
            x[1] = (Real)0;
            distance = fabs(y[0] - e[0]);
        }
    }
    return distance;
}
//----------------------------------------------------------------------------
// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1.  The query point is (y0,y1).
// The function returns the distance from the query point to the ellipse.
// It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
//----------------------------------------------------------------------------
template <typename Real>
Real DistancePointEllipse (const Real e[2], const Real y[2], Real x[2])
{
    // Determine reflections for y to the first quadrant.
    bool reflect[2];
    int i, j;
    for (i = 0; i < 2; ++i)
    {
        reflect[i] = (y[i] < (Real)0);
    }

    // Determine the axis order for decreasing extents.
    int permute[2];
    if (e[0] < e[1])
    {
        permute[0] = 1;  permute[1] = 0;
    }
    else
    {
        permute[0] = 0;  permute[1] = 1;
    }

    int invpermute[2];
    for (i = 0; i < 2; ++i)
    {
        invpermute[permute[i]] = i;
    }

    Real locE[2], locY[2];
    for (i = 0; i < 2; ++i)
    {
        j = permute[i];
        locE[i] = e[j];
        locY[i] = y[j];
        if (reflect[j])
        {
            locY[i] = -locY[i];
        }
    }

    Real locX[2];
    Real distance = DistancePointEllipseSpecial(locE, locY, locX);

    // Restore the axis order and reflections.
    for (i = 0; i < 2; ++i)
    {
        j = invpermute[i];
        if (reflect[j])
        {
            locX[j] = -locX[j];
        }
        x[i] = locX[j];
    }

    return distance;
}

glm::vec2 closestPointOnRay(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
	if(p1 == p2) {
		return p1;
	}

	float u = (p3.x - p1.x) * (p2.x - p1.x);
	u += (p3.y - p1.y) * (p2.y - p1.y);
	float len = glm::length(p2 - p1);
	u /= (len * len);

	return glm::mix(p1, p2, u);
}

glm::vec2 closestPointOnLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
	if(p1 == p2) {
		return p1;
	}

	float u = (p3.x - p1.x) * (p2.x - p1.x);
	u += (p3.y - p1.y) * (p2.y - p1.y);
	float len = glm::length(p2 - p1);
	u /= (len * len);

	// clamp u
	if(u > 1) {
		u = 1;
	} else if(u < 0) {
		u = 0;
	}
	return glm::mix(p1, p2, u);
}

glm::vec2 closestPointOnRect(const cv::RotatedRect& rect, const glm::vec2& point) {
	glm::vec2 norm = point;
	glm::vec2 offset(rect.center.x, rect.center.y);
	norm -= offset;
	norm = rotateVec2(norm, -rect.angle);
	float w = rect.size.width / 2, h = rect.size.height / 2;
	glm::vec2 nearest;
	if((norm.x > w || norm.x < -w) || (norm.y < -h || norm.y > h)) {
		nearest = glm::vec2(ofClamp(norm.x, -w, w), ofClamp(norm.y, -h, h));
	} else {
		if(fabsf(fabsf(norm.x) - w) < fabsf(fabsf(norm.y) - h)) {
			nearest = glm::vec2(w * (norm.x > 0 ? 1 : -1), norm.y);
		} else {
			nearest = glm::vec2(norm.x, h * (norm.y > 0 ? 1 : -1));
		}
	}
	nearest = rotateVec2(nearest, rect.angle);
	nearest += offset;
	return nearest;
}

glm::vec2 closestPointOnCircle(const glm::vec2& center, float radius, const glm::vec2& point) {
	glm::vec2 nearest = point - center;
	nearest *= radius / glm::length(nearest);
	nearest += center;
	return nearest;
}

glm::vec2 closestPointOnEllipse(const cv::RotatedRect& ellipse, const glm::vec2& point) {
	glm::vec2 norm = point;
	glm::vec2 offset(ellipse.center.x, ellipse.center.y);
	norm -= offset;
	norm = rotateVec2(norm, -ellipse.angle);
	bool flipX = norm.x < 0, flipY = norm.y < 0;
	if(flipX) norm.x *= -1;
	if(flipY) norm.y *= -1;
	float e[] = {ellipse.size.width / 2, ellipse.size.height / 2};
	float y[] = {norm.x, norm.y};
	float x[2];
	DistancePointEllipse(e, y, x);
	glm::vec2 result(x[0], x[1]);
	if(flipX) result.x *= -1;
	if(flipY) result.y *= -1;
	result = rotateVec2(result, ellipse.angle);
	result += offset;
	return result;
}

float distanceToEllipse(const glm::vec2& point, const cv::RotatedRect& ellipse) {
	return glm::distance(closestPointOnEllipse(ellipse, point), point);
}

float distanceToRect(const glm::vec2& point, const cv::RotatedRect& rect) {
	return glm::distance(closestPointOnRect(rect, point), point);
}

float distanceToLine(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end) {
	return glm::distance(closestPointOnLine(start, end, point), point);
}

float distanceToRay(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end) {
	return glm::distance(closestPointOnRay(start, end, point), point);
}
