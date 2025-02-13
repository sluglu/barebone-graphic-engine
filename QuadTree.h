#include <iostream>
#include <vector>
#include <memory>
#include <GLContext.h>

// =========================
// AABB (Axis-Aligned Bounding Box)
// =========================
class AABB {
public:
    double xMin, xMax, yMin, yMax;

    AABB(double xMin, double xMax, double yMin, double yMax)
        : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) {
    }

    bool contains(double x, double y) const {
        return x >= xMin && x <= xMax && y >= yMin && y <= yMax;
    }

    bool intersects(const AABB& other) const {
        return !(xMax < other.xMin || xMin > other.xMax ||
            yMax < other.yMin || yMin > other.yMax);
    }

    AABB getChild(int index) const {
        double midX = (xMin + xMax) / 2.0;
        double midY = (yMin + yMax) / 2.0;

        switch (index) {
        case 0: return AABB(midX, xMax, midY, yMax);  // Top Right
        case 1: return AABB(xMin, midX, midY, yMax);  // Top Left
        case 2: return AABB(xMin, midX, yMin, midY);  // Bottom Left
        case 3: return AABB(midX, xMax, yMin, midY);  // Bottom Right
        default: throw std::out_of_range("Invalid quadrant index");
        }
    }

	void draw(double Thickness, vec4 Color) const {
        GLContext::drawRect(vec2(xMin, yMin), vec2(xMax, yMax), Thickness, Color);
	}
};
