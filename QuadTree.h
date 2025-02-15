#include <iostream>
#include <vector>
#include <memory>
#include <GLContext.h>

// =========================
// AABB (Axis-Aligned Bounding Box)
// =========================
class AABB {
public:
    vec2 inferiorLeft, superiorRight;

    AABB(vec2 inferiorLeft, vec2 superiorRight)
        : inferiorLeft(inferiorLeft), superiorRight(superiorRight){
    }

    bool contains(double x, double y) const {
        return x >= inferiorLeft.x && x <= superiorRight.x && y >= inferiorLeft.y && y <= superiorRight.y;
    }

    bool intersects(const AABB& other) const {
        return !(superiorRight.x < other.inferiorLeft.x || inferiorLeft.x > other.superiorRight.x ||
            superiorRight.y < other.inferiorLeft.y || inferiorLeft.y > other.superiorRight.y);
    }

    AABB getChild(int index) const {
        double midX = (inferiorLeft.x + superiorRight.x) / 2.0;
        double midY = (inferiorLeft.y + superiorRight.y) / 2.0;

        switch (index) {
        case 0: return AABB(vec2(midX, midY), vec2(superiorRight.x, superiorRight.y));  // Top Right
        case 1: return AABB(vec2(inferiorLeft.x, midY), vec2(midX, superiorRight.y));  // Top Left
        case 2: return AABB(vec2(inferiorLeft.x, inferiorLeft.y), vec2(midX, midY));  // Bottom Left
        case 3: return AABB(vec2(midX, inferiorLeft.y), vec2(superiorRight.x, midY));  // Bottom Right
        default: throw std::out_of_range("Invalid quadrant index");
        }
    }

	void draw(double Thickness = 1.0, vec4 Color = vec4(1,1,1,1)) const {
        GLContext::drawRect(vec2(inferiorLeft.x, inferiorLeft.y*-1), vec2(superiorRight.x, superiorRight.y * -1), Thickness, Color);
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

	void draw(double Thickness = 5.0, vec4 Color = vec4(1, 1, 1, 1)) const {
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

