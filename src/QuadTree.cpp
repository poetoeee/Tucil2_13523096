#include "QuadTree.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <functional>
#include "gif.h"

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
                    error += pow(pixels[i][j].r - avg.r, 2) + pow(pixels[i][j].g - avg.g, 2) + pow(pixels[i][j].b - avg.b, 2);
                    count++;
                }
            }
            return count > 0 ? error / (count * 3) : 0;
        }
        
        case 2: { 
            for (int i = y; i < y + height && i < pixels.size(); i++) {
                for (int j = x; j < x + width && j < pixels[i].size(); j++) {
                    error += abs(pixels[i][j].r - avg.r) + abs(pixels[i][j].g - avg.g) +abs(pixels[i][j].b - avg.b);
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

        case 5: {
            const double C1 = 6.5025, C2 = 58.5225;
            int count = 0;
        
            double mu[3] = {0}, sigma[3] = {0};
            for (int i = y; i < y + height; i++) {
                for (int j = x; j < x + width; j++) {
                    mu[0] += pixels[i][j].r;
                    mu[1] += pixels[i][j].g;
                    mu[2] += pixels[i][j].b;
                    count++;
                }
            }
            for (int c = 0; c < 3; c++) mu[c] /= count;

            // Hitung variance
            for (int i = y; i < y + height; i++) {
                for (int j = x; j < x + width; j++) {
                    sigma[0] += pow(pixels[i][j].r - mu[0], 2);
                    sigma[1] += pow(pixels[i][j].g - mu[1], 2);
                    sigma[2] += pow(pixels[i][j].b - mu[2], 2);
                }
            }
            for (int c = 0; c < 3; c++) sigma[c] /= count;

            double ssimSum = 0;
            for (int c = 0; c < 3; c++) {
                const double numerator = (2 * mu[c] * mu[c] + C1) * C2;
                const double denominator = (mu[c] * mu[c] + mu[c] * mu[c] + C1) * (sigma[c] + C2);
                ssimSum += numerator / denominator;
            }
            return ssimSum / 3.0;
        }
        
        default:
            return 0;
    }
}

void QuadTree::recordCompressionFrame(const std::vector<std::vector<RGB>>& frame) {
    compressionFrames.push_back(frame);
}

bool QuadTree::saveCompressionGIF(const std::string& filename, int delay) {
    if (compressionFrames.empty()) return false;
    
    int width = compressionFrames[0][0].size();
    int height = compressionFrames[0].size();
    
    GifWriter g;
    if (!GifBegin(&g, filename.c_str(), width, height, delay)) {
        return false;
    }
    
    std::vector<uint8_t> image(width * height * 4);
    for (const auto& frame : compressionFrames) {
        // ... [isi frame ke image] ...
        if (!GifWriteFrame(&g, image.data(), width, height, delay)) {
            GifEnd(&g);
            return false;
        }
    }
    
    return GifEnd(&g);
}

// size_t QuadTree::estimateCompressedSize(double tempThreshold) {
//     std::function<size_t(int, int, int, int, double)> estimate;
    
//     estimate = [&](int x, int y, int width, int height, double thresh) -> size_t {
//         if (width * height <= minBlockSize) {
//             return sizeof(RGB); 
//         }

//         double error = calculateError(x, y, width, height);
//         if (error <= thresh) {
//             return sizeof(RGB); 
//         }

//         int halfW = width / 2, halfH = height / 2;
//         return estimate(x, y, halfW, halfH, thresh) +
//                estimate(x + halfW, y, width - halfW, halfH, thresh) +
//                estimate(x, y + halfH, halfW, height - halfH, thresh) +
//                estimate(x + halfW, y + halfH, width - halfW, height - halfH, thresh) +
//                sizeof(void*) * 4; 
//     };

//     return estimate(0, 0, pixels[0].size(), pixels.size(), tempThreshold);
// }

// void QuadTree::adjustThresholdForCompression(const std::vector<std::vector<RGB>>& imagePixels, double targetRatio, size_t originalSize) {
//     pixels = imagePixels;
    
//     if (targetRatio <= 0) return;

//     double low = 0;
//     double high = (errorMethod == 1) ? 65025 : 
//                  (errorMethod == 2 || errorMethod == 3) ? 255 : 8;
//     double mid = threshold;
//     double currentRatio = 0;
//     const double epsilon = 0.01; 
//     const int maxIterations = 20; 

//     for (int i = 0; i < maxIterations; i++) {
//         size_t estimatedSize = estimateCompressedSize(mid);
//         currentRatio = 1.0 - (double)estimatedSize / originalSize;

//         if (fabs(currentRatio - targetRatio) < epsilon) {
//             break;
//         }

//         if (currentRatio < targetRatio) {
//             low = mid;
//         } else {
//             high = mid;
//         }
//         mid = (low + high) / 2;
//     }

//     threshold = mid;
// }


std::shared_ptr<QuadNode> QuadTree::buildTree(int x, int y, int width, int height) {
    if (width * height <= minBlockSize) {
        auto node = std::make_shared<QuadNode>(x, y, width, height, 
                                     calculateAverage(x, y, width, height), true);
        // Rekam frame setelah pembuatan leaf node
        auto frame = reconstructImage();
        recordCompressionFrame(frame);
        return node;
    }

    double error = calculateError(x, y, width, height);
    if (error <= threshold) {
        auto node = std::make_shared<QuadNode>(x, y, width, height, 
                                     calculateAverage(x, y, width, height), true);
        // Rekam frame setelah pembuatan leaf node
        auto frame = reconstructImage();
        recordCompressionFrame(frame);
        return node;
    }

    int halfW = width / 2, halfH = height / 2;
    auto nw = buildTree(x, y, halfW, halfH);
    auto ne = buildTree(x + halfW, y, width - halfW, halfH);
    auto sw = buildTree(x, y + halfH, halfW, height - halfH);
    auto se = buildTree(x + halfW, y + halfH, width - halfW, height - halfH);

    auto node = std::make_shared<QuadNode>(x, y, width, height, RGB(), false);
    node->setChildren(nw, ne, sw, se);
    
    // Rekam frame setelah penggabungan node
    auto frame = reconstructImage();
    recordCompressionFrame(frame);
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