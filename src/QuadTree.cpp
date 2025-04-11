#include "QuadTree.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>
#include <functional>
#include <vector>
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
            bool pixelFound = false;
            for (int i = y; i < y + height && i < pixels.size(); i++) {
                for (int j = x; j < x + width && j < pixels[i].size(); j++) {
                    minR = min(minR, pixels[i][j].r);
                    maxR = max(maxR, pixels[i][j].r);
                    minG = min(minG, pixels[i][j].g);
                    maxG = max(maxG, pixels[i][j].g);
                    minB = min(minB, pixels[i][j].b);
                    maxB = max(maxB, pixels[i][j].b);
                    pixelFound = true;
                }
            }
            if (!pixelFound) return 0.0;
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
            avg = calculateAverage(x, y, width, height);
            long long totalPixels = 0;
            double meanOrigR = 0, meanOrigG = 0, meanOrigB = 0;
            double varOrigR = 0, varOrigG = 0, varOrigB = 0;

            for (int i = y; i < y + height && i < pixels.size(); ++i) {
                for (int j = x; j < x + width && j < pixels[i].size(); ++j) {
                    meanOrigR += pixels[i][j].r;
                    meanOrigG += pixels[i][j].g;
                    meanOrigB += pixels[i][j].b;
                    totalPixels++;
                }
            }

            if (totalPixels == 0) return 0.0; 

            meanOrigR /= totalPixels;
            meanOrigG /= totalPixels;
            meanOrigB /= totalPixels;

            for (int i = y; i < y + height && i < pixels.size(); ++i) {
                for (int j = x; j < x + width && j < pixels[i].size(); ++j){
                    double diffR = pixels[i][j].r - meanOrigR;
                    double diffG = pixels[i][j].g - meanOrigG;
                    double diffB = pixels[i][j].b - meanOrigB;
                    varOrigR += diffR * diffR;
                    varOrigG += diffG * diffG;
                    varOrigB += diffB * diffB;
                }
            }

             if (totalPixels > 1) {
                 varOrigR /= (totalPixels - 1); 
                 varOrigG /= (totalPixels - 1); 
                 varOrigB /= (totalPixels - 1); 
             } else {
                 varOrigR = varOrigG = varOrigB = 0;
             }

            double meanCompR = avg.r;
            double meanCompG = avg.g;
            double meanCompB = avg.b;
            const double varCompR = 0.0, varCompG = 0.0, varCompB = 0.0;
            const double covR = 0.0, covG = 0.0, covB = 0.0;

            const double K1 = 0.01;
            const double K2 = 0.03;
            const double L = 255.0; 
            const double C1 = (K1 * L) * (K1 * L);
            const double C2 = (K2 * L) * (K2 * L);

            double ssimR = ((2.0 * meanOrigR * meanCompR + C1) * C2) /
                           ((meanOrigR * meanOrigR + meanCompR * meanCompR + C1) * (varOrigR + varCompR + C2));
            double ssimG = ((2.0 * meanOrigG * meanCompG + C1) * C2) /
                           ((meanOrigG * meanOrigG + meanCompG * meanCompG + C1) * (varOrigG + varCompG + C2));
            double ssimB = ((2.0 * meanOrigB * meanCompB + C1) * C2) /
                           ((meanOrigB * meanOrigB + meanCompB * meanCompB + C1) * (varOrigB + varCompB + C2));

            double avgSSIM = (ssimR + ssimG + ssimB) / 3.0;

            avgSSIM = std::max(-1.0, std::min(1.0, avgSSIM));

            return 1.0 - avgSSIM;
        }
        
        default:
            return 0;
    }
}

std::shared_ptr<QuadNode> QuadTree::buildTree(int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        return nullptr; 
    }
    bool makeLeaf = false;
    if (width * height <= minBlockSize) {
        makeLeaf = true;
    } else {
        double error = calculateError(x, y, width, height);
        if (errorMethod == 5) { 
             if (1.0 - error >= threshold) { 
                 makeLeaf = true;
             }
         } else { 
             if (error <= threshold) {
                 makeLeaf = true;
             }
         }
    }

    if (!makeLeaf) {
        int halfW = width / 2;
        int halfH = height / 2;
        if (halfW == 0 || halfH == 0) { 
             makeLeaf = true;
        } else {
             int remW = width - halfW;
             int remH = height - halfH;
             if ((halfW * halfH < minBlockSize) ||
                 (remW * halfH < minBlockSize) ||
                 (halfW * remH < minBlockSize) ||
                 (remW * remH < minBlockSize))
             {
                  makeLeaf = true;
             }
        }
    }
    if (makeLeaf) {
        return std::make_shared<QuadNode>(x, y, width, height, calculateAverage(x, y, width, height), true);
    } else {
        int halfW = width / 2;
        int halfH = height / 2;
        int remW = width - halfW;
        int remH = height - halfH; 

        if (halfW <= 0 || halfH <= 0 || remW <= 0 || remH <= 0) { 
             return std::make_shared<QuadNode>(x, y, width, height, calculateAverage(x, y, width, height), true);
        }

        auto nw = buildTree(x, y, halfW, halfH);
        auto ne = buildTree(x + halfW, y, remW, halfH);
        auto sw = buildTree(x, y + halfH, halfW, remH);
        auto se = buildTree(x + halfW, y + halfH, remW, remH);
        RGB avgColorForGif = calculateAverage(x, y, width, height);
        auto node = std::make_shared<QuadNode>(x, y, width, height, avgColorForGif, false);
        node->setChildren(nw, ne, sw, se);
        return node;
    }
}

void QuadTree::compress(const std::vector<std::vector<RGB>>& imagePixels) {
    if (imagePixels.empty() || imagePixels[0].empty()) {
        root = nullptr;
        return; 
    }
    pixels = imagePixels;
    int height = pixels.size();
    int width = pixels[0].size();

    root = buildTree(0, 0, width, height);
}

void QuadTree::findNodesPerLevel(std::shared_ptr<QuadNode> node, int level,std::map<int, std::vector<std::shared_ptr<QuadNode>>>& nodesMap, int& maxLevelFound) {
    if (!node) {
        return; 
    }
    nodesMap[level].push_back(node);
    maxLevelFound = std::max(maxLevelFound, level);

    if (!node->isLeafNode()) {
        for (int i = 0; i < 4; ++i) {
            findNodesPerLevel(node->getChild(i), level + 1, nodesMap, maxLevelFound);
        }
    }
}

void QuadTree::drawNodeArea(std::vector<std::vector<RGB>>& canvas, std::shared_ptr<QuadNode> node) {
    if (!node) return;

    RGB color = node->getColor();
    int startX = node->getX();
    int startY = node->getY();
    int nodeWidth = node->getWidth();
    int nodeHeight = node->getHeight();

    int canvasHeight = canvas.size();
    if (canvasHeight == 0) return;
    int canvasWidth = canvas[0].size();
    if (canvasWidth == 0) return;

    int endY = std::min(startY + nodeHeight, canvasHeight);
    int endX = std::min(startX + nodeWidth, canvasWidth);
    startY = std::max(0, startY); 
    startX = std::max(0, startX);

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            canvas[y][x] = color;
        }
    }
}

bool QuadTree::saveGIF(const std::string& filename, int delay, bool dither) {
   int imgHeight = pixels.size();
   int imgWidth = pixels[0].size();

   std::map<int, std::vector<std::shared_ptr<QuadNode>>> nodesByLevel;
   int maxDepth = -1;
   findNodesPerLevel(this->root, 0, nodesByLevel, maxDepth);

   if (maxDepth < 0) {
       cerr << "Error: Pohon kosong." << endl;
       return false;
   }

   GifWriter writer = {};
   int gifDelay = delay / 10; 
   if (gifDelay < 1) gifDelay = 1;

   if (!GifBegin(&writer, filename.c_str(), imgWidth, imgHeight, gifDelay)) {
       cerr << "Error: Gagal memulai pembuatan GIF ke " << filename << endl;
       return false;
   }

   vector<vector<RGB>> frameCanvas(imgHeight, vector<RGB>(imgWidth));
   vector<uint8_t> imageBuffer(imgWidth * imgHeight * 4);

   if (nodesByLevel.count(0) && !nodesByLevel[0].empty()) {
       drawNodeArea(frameCanvas, nodesByLevel[0][0]); 
   } else {
       for (auto& row : frameCanvas) std::fill(row.begin(), row.end(), RGB(0, 0, 0));
   }

   for (int y = 0; y < imgHeight; ++y) {
       for (int x = 0; x < imgWidth; ++x) {
           size_t idx = (y * imgWidth + x) * 4;
           const RGB& pixel = frameCanvas[y][x];
           imageBuffer[idx + 0] = pixel.r;
           imageBuffer[idx + 1] = pixel.g;
           imageBuffer[idx + 2] = pixel.b;
           imageBuffer[idx + 3] = 255; 
       }
   }

   if (!GifWriteFrame(&writer, imageBuffer.data(), imgWidth, imgHeight, gifDelay, 8, dither)) {
        cerr << "Error: Gagal membuat frame awal GIF." << endl;
        GifEnd(&writer);
        return false;
   }
   for (int level = 1; level <= maxDepth; ++level) {
       if (nodesByLevel.count(level)) {
           for (const auto& node : nodesByLevel[level]) {
               drawNodeArea(frameCanvas, node); 
           }
           for (int y = 0; y < imgHeight; ++y) {
               for (int x = 0; x < imgWidth; ++x) {
                   size_t idx = (y * imgWidth + x) * 4;
                   const RGB& pixel = frameCanvas[y][x];
                   imageBuffer[idx + 0] = pixel.r;
                   imageBuffer[idx + 1] = pixel.g;
                   imageBuffer[idx + 2] = pixel.b;
                   imageBuffer[idx + 3] = 255;
               }
           }

           if (!GifWriteFrame(&writer, imageBuffer.data(), imgWidth, imgHeight, gifDelay, 8, dither)) {
                cerr << "Error: Gagal membuat frame GIF level " << level << "." << endl;
                GifEnd(&writer);
                return false;
           }
       }
   }

   if (!GifEnd(&writer)) {
       cerr << "Error: Gagal menyelesaikan pembuatan GIF." << endl;
       return false;
   }

   cout << "\nGIF berhasil disimpan!" << endl;
   return true;
}

std::vector<std::vector<RGB>> QuadTree::reconstructImage() {
    std::vector<std::vector<RGB>> result(pixels.size(), std::vector<RGB>(pixels[0].size()));
    
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