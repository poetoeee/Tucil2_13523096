#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <memory>
#include <vector>
#include <cmath>
#include <map>
#include <string>
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

    void findNodesPerLevel(std::shared_ptr<QuadNode> node, int level,std::map<int, std::vector<std::shared_ptr<QuadNode>>>& nodesMap,int& maxLevelFound);
void drawNodeArea(std::vector<std::vector<RGB>>& canvas, std::shared_ptr<QuadNode> node);
   
public:
    QuadTree(double threshold, int minSize, int method): threshold(threshold), minBlockSize(minSize), errorMethod(method) {}

    void compress(const std::vector<std::vector<RGB>>& imagePixels);

    bool saveGIF(const std::string& filename, int delay = 100, bool dither = false);

    std::vector<std::vector<RGB>> reconstructImage();
    
    int countNodes() const;
    int countLeaves() const;
    int getDepth() const;
};

#endif