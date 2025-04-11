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
    std::vector<std::vector<std::vector<RGB>>> compressionFrames;
    // size_t estimateCompressedSize(double tempThreshold);

public:
    QuadTree(double threshold, int minSize, int method): threshold(threshold), minBlockSize(minSize), errorMethod(method) {}

    void compress(const std::vector<std::vector<RGB>>& imagePixels);
    void recordCompressionFrame(const std::vector<std::vector<RGB>>& frame);
    bool saveCompressionGIF(const std::string& filename, int delay = 100);
    // void adjustThresholdForCompression(const std::vector<std::vector<RGB>>& imagePixels, double targetRatio, size_t originalSize);
    std::vector<std::vector<RGB>> reconstructImage();
    
    int countNodes() const;
    int countLeaves() const;
    int getDepth() const;
};

#endif