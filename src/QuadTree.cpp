#include "QuadTree.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <functional>

using namespace std;

RGB QuadTree::calculateAverage(int x, int y, int width, int height) {
    double r = 0, g = 0, b = 0;
    int count = 0;
    
    for (int i = y; i < y + height && i < pixels.size(); i++) {
        for (int j = x; j < x + width && j < pixels[i].size(); j++) {
            r += pixels[i][j].r;
            g += pixels[i][j].g;
            b += pixels[i][j].b;
            count++;
        }
    }
    
    if (count == 0) return RGB();
    return RGB(static_cast<uint8_t>(r/count), 
              static_cast<uint8_t>(g/count), 
              static_cast<uint8_t>(b/count));
}

double QuadTree::calculateError(int x, int y, int width, int height) {
    RGB avg = calculateAverage(x, y, width, height);
    int count = 0;
    double error = 0;

    switch(errorMethod) {
        case 1: { 
            for (int i = y; i < y + height && i < pixels.size(); i++) {
                for (int j = x; j < x + width && j < pixels[i].size(); j++) {
                    error += pow(pixels[i][j].r - avg.r, 2) +
                            pow(pixels[i][j].g - avg.g, 2) +
                            pow(pixels[i][j].b - avg.b, 2);
                    count++;
                }
            }
            return count > 0 ? error / (count * 3) : 0;
        }
        
        case 2: { 
            for (int i = y; i < y + height && i < pixels.size(); i++) {
                for (int j = x; j < x + width && j < pixels[i].size(); j++) {
                    error += abs(pixels[i][j].r - avg.r) +
                            abs(pixels[i][j].g - avg.g) +
                            abs(pixels[i][j].b - avg.b);
                    count++;
                }
            }
            return count > 0 ? error / (count * 3) : 0;
        }
        
        case 3: {
            uint8_t minR = 255, maxR = 0, minG = 255, maxG = 0, minB = 255, maxB = 0;
            for (int i = y; i < y + height && i < pixels.size(); i++) {
                for (int j = x; j < x + width && j < pixels[i].size(); j++) {
                    minR = min(minR, pixels[i][j].r);
                    maxR = max(maxR, pixels[i][j].r);
                    minG = min(minG, pixels[i][j].g);
                    maxG = max(maxG, pixels[i][j].g);
                    minB = min(minB, pixels[i][j].b);
                    maxB = max(maxB, pixels[i][j].b);
                }
            }
            return ((maxR - minR) + (maxG - minG) + (maxB - minB)) / 3.0;
        }
        
        case 4: { 
            unordered_map<uint8_t, int> histR, histG, histB;
            int totalPixels = 0;
            
            for (int i = y; i < y + height; i++) {
                for (int j = x; j < x + width; j++) {
                    histR[pixels[i][j].r]++;
                    histG[pixels[i][j].g]++; 
                    histB[pixels[i][j].b]++;
                    totalPixels++;
                }
            }
            
            auto calcEntropy = [totalPixels](const auto& hist) {
                double entropy = 0;
                for (const auto& pair : hist) {
                    if (pair.second > 0) {
                        double p = pair.second / static_cast<double>(totalPixels);
                        entropy -= p * log2(p);
                    }
                }
                return entropy;
            };
            
            return (calcEntropy(histR) + calcEntropy(histG) + calcEntropy(histB)) / 3.0;
        }
        
        default:
            return 0;
    }
}


std::shared_ptr<QuadNode> QuadTree::buildTree(int x, int y, int width, int height) {
    if (width * height <= minBlockSize) {
        return std::make_shared<QuadNode>(x, y, width, height, 
                                           calculateAverage(x, y, width, height), true);
    }

    double error = calculateError(x, y, width, height);
    if (error <= threshold) {
        return std::make_shared<QuadNode>(x, y, width, height, 
                                           calculateAverage(x, y, width, height), true);
    }

    int halfW = width / 2, halfH = height / 2;
    auto nw = buildTree(x, y, halfW, halfH);
    auto ne = buildTree(x + halfW, y, width - halfW, halfH);
    auto sw = buildTree(x, y + halfH, halfW, height - halfH);
    auto se = buildTree(x + halfW, y + halfH, width - halfW, height - halfH);

    auto node = std::make_shared<QuadNode>(x, y, width, height, RGB(), false);
    node->setChildren(nw, ne, sw, se);
    return node;
}

void QuadTree::compress(const std::vector<std::vector<RGB>>& imagePixels) {
    pixels = imagePixels;
    root = buildTree(0, 0, pixels[0].size(), pixels.size());
}

std::vector<std::vector<RGB>> QuadTree::reconstructImage() {
    std::vector<std::vector<RGB>> result(pixels.size(), 
                                       std::vector<RGB>(pixels[0].size()));
    
    std::function<void(std::shared_ptr<QuadNode>)> reconstruct;
    reconstruct = [&](std::shared_ptr<QuadNode> node) {
        if (!node) return;
        
        if (node->isLeafNode()) {
            for (int y = node->getY(); y < node->getY() + node->getHeight(); y++) {
                for (int x = node->getX(); x < node->getX() + node->getWidth(); x++) {
                    result[y][x] = node->getColor();
                }
            }
        } else {
            for (int i = 0; i < 4; i++) {
                reconstruct(node->getChild(i));
            }
        }
    };
    
    reconstruct(root);
    return result;
}

int QuadTree::countNodes() const {
    std::function<int(std::shared_ptr<QuadNode>)> count;
    count = [&](std::shared_ptr<QuadNode> node) {
        if (!node) return 0;
        int total = 1;
        for (int i = 0; i < 4; i++) {
            total += count(node->getChild(i));
        }
        return total;
    };
    return count(root);
}

int QuadTree::countLeaves() const {
    std::function<int(std::shared_ptr<QuadNode>)> count;
    count = [&](std::shared_ptr<QuadNode> node) {
        if (!node) return 0;
        if (node->isLeafNode()) return 1;
        int total = 0;
        for (int i = 0; i < 4; i++) {
            total += count(node->getChild(i));
        }
        return total;
    };
    return count(root);
}

int QuadTree::getDepth() const {
    std::function<int(std::shared_ptr<QuadNode>)> depth;
    depth = [&](std::shared_ptr<QuadNode> node) {
        if (!node || node->isLeafNode()) return 1;
        int maxDepth = 0;
        for (int i = 0; i < 4; i++) {
            maxDepth = std::max(maxDepth, depth(node->getChild(i)));
        }
        return 1 + maxDepth;
    };
    return depth(root);
}