#include <iostream>
#include <vector>
#include <memory>
#include <GLContext.h>

// =========================
// AABB (Axis-Aligned Bounding Box)
// =========================
class AABB {
public:
    pair<double,double> inferiorLeft, superiorRight;

    AABB(pair<double,double> inferiorLeft, pair<double,double> superiorRight)
        : inferiorLeft(inferiorLeft), superiorRight(superiorRight){
    }

    bool contains(double x, double y) const {
        return x >= inferiorLeft.first && x < superiorRight.first && y >= inferiorLeft.second && y < superiorRight.second;
    }

    bool intersects(const AABB& other) const {
        return !(superiorRight.first < other.inferiorLeft.first || inferiorLeft.first > other.superiorRight.first ||
            superiorRight.second < other.inferiorLeft.second || inferiorLeft.second > other.superiorRight.second);
    }

    AABB getChild(int index) const {
        double midX = (inferiorLeft.first + superiorRight.first) / 2.0;
        double midY = (inferiorLeft.second + superiorRight.second) / 2.0;

        switch (index) {
        case 0: return AABB(pair<double,double>(midX, midY), pair<double,double>(superiorRight.first, superiorRight.second));  // Top Right
        case 1: return AABB(pair<double,double>(inferiorLeft.first, midY), pair<double,double>(midX, superiorRight.second));  // Top Left
        case 2: return AABB(pair<double,double>(inferiorLeft.first, inferiorLeft.second), pair<double,double>(midX, midY));  // Bottom Left
        case 3: return AABB(pair<double,double>(midX, inferiorLeft.second), pair<double,double>(superiorRight.first, midY));  // Bottom Right
        default: throw std::out_of_range("Invalid quadrant index");
        }
    }

	void draw(double Thickness = 1.0, vec4 Color = vec4(1,1,1,1)) const {
        GLContext::drawRect(inferiorLeft, superiorRight, Thickness, Color);
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
        // If the point is not within the node's bounds, do nothing.
        if (!node->bounds.contains(x, y))
            return;

        // If we're at maximum depth or there's room in this node, store the object.
        if (!node->children[0] && (depth >= maxDepth || node->objects.size() < maxObjects)) {
            node->objects.emplace_back(object, std::make_pair(x, y));
            return;
        }

        // Subdivide if not already done.
        if (!node->children[0]) {
            subdivide(node);
            // Move existing objects into appropriate children.
            auto temp = node->objects;
            node->objects.clear();
            for (const auto& obj : temp) {
                for (auto& child : node->children) {
                    if (child->bounds.contains(obj.second.first, obj.second.second)) {
                        insert(child.get(), obj.first, obj.second.first, obj.second.second, depth + 1);
                        break;
                    }
                }
            }
        }

        // Insert the new object into the correct child.
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

