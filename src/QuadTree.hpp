#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <memory>
#include <vector>
#include <cmath>
#include <unordered_map>
#include "QuadNode.hpp"
#include "Image.hpp"

class QuadTree {
private:
    std::shared_ptr<QuadNode> root;
    std::vector<std::vector<RGB>> pixels;
    double threshold;
    int minBlockSize;
    int errorMethod;
    
    RGB calculateAverage(int x, int y, int width, int height);
    double calculateError(int x, int y, int width, int height);
    std::shared_ptr<QuadNode> buildTree(int x, int y, int width, int height);
   
public:
    QuadTree(double threshold, int minSize, int method): threshold(threshold), minBlockSize(minSize), errorMethod(method) {}

    void compress(const std::vector<std::vector<RGB>>& imagePixels);
    std::vector<std::vector<RGB>> reconstructImage();
    
    int countNodes() const;
    int countLeaves() const;
    int getDepth() const;
};

#endif