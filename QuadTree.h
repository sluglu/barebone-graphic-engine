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

	void draw(double Thickness = 1.0, vec4 Color = vec4(1,1,1,1)) const {
        GLContext::drawRect(vec2(xMin, yMin), vec2(xMax, yMax), Thickness, Color);
	}
};

// =========================
// Quadtree Class
// =========================
template <typename T>
class Quadtree {
public:
    struct Node {
        AABB bounds;
        std::vector<std::pair<T, std::pair<double, double>>> objects;  // (Data, (X, Y))
        std::unique_ptr<Node> children[4];

        Node(const AABB& bounds) : bounds(bounds) {}

		void draw(double Thickness = 1.0, vec4 Color = vec4(1, 1, 1, 1)) const {
			bounds.draw(Thickness, Color);
			if (children[0]) {
				for (auto& child : children) {
					child->draw(Thickness, Color);
				}
			}
		}
    };

    Quadtree(const AABB& bounds, size_t maxObjects = 4, size_t maxDepth = 10)
        : maxObjects(maxObjects), maxDepth(maxDepth), root(std::make_unique<Node>(bounds)) {
    }

    void insert(const T& object, double x, double y) {
        insert(root.get(), object, x, y, 0);
    }

    std::vector<T> query(const AABB& range) {
        std::vector<T> result;
        query(root.get(), range, result);
        return result;
    }

	void draw(double Thickness = 1.0, vec4 Color = vec4(1, 1, 1, 1)) const {
		root->draw(Thickness, Color);
	}

private:
    size_t maxObjects, maxDepth;
    std::unique_ptr<Node> root;

    void insert(Node* node, const T& object, double x, double y, size_t depth) {
        if (!node->bounds.contains(x, y)) return;

        if (depth >= maxDepth || node->objects.size() < maxObjects) {
            node->objects.emplace_back(object, std::make_pair(x, y));
            return;
        }

        if (!node->children[0]) subdivide(node);

        for (auto& child : node->children) {
            if (child->bounds.contains(x, y)) {
                insert(child.get(), object, x, y, depth + 1);
                return;
            }
        }
    }

    void query(Node* node, const AABB& range, std::vector<T>& result) {
        if (!node->bounds.intersects(range)) return;

        for (const auto& obj : node->objects) {
            if (range.contains(obj.second.first, obj.second.second))
                result.push_back(obj.first);
        }

        if (!node->children[0]) return;

        for (auto& child : node->children) {
            query(child.get(), range, result);
        }
    }

    void subdivide(Node* node) {
        for (int i = 0; i < 4; i++)
            node->children[i] = std::make_unique<Node>(node->bounds.getChild(i));
    }
};

