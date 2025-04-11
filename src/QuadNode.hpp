#ifndef QUADTREE_NODE_HPP
#define QUADTREE_NODE_HPP

#include <memory>
#include "Image.hpp"

class QuadNode {
private:
    int x, y, width, height;
    RGB averageColor;
    bool isLeaf;
    std::shared_ptr<QuadNode> children[4]; 

public:
    QuadNode(int x, int y, int w, int h, RGB color, bool leaf): x(x), y(y), width(w), height(h), averageColor(color), isLeaf(leaf) {}

    void setChildren(std::shared_ptr<QuadNode> nw, std::shared_ptr<QuadNode> ne,std::shared_ptr<QuadNode> sw, std::shared_ptr<QuadNode> se) {
        children[0] = nw;
        children[1] = ne;
        children[2] = sw;
        children[3] = se;
        isLeaf = false;
    }

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    RGB getColor() const { return averageColor; }
    bool isLeafNode() const { return isLeaf; }
    std::shared_ptr<QuadNode> getChild(int index) const { return children[index]; }
};

#endif