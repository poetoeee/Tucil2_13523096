#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <fstream>
#include "stb_image.h"
#include "stb_image_write.h"

struct RGB {
    uint8_t r, g, b;
    RGB() : r(0), g(0), b(0) {}
    RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    bool operator==(const RGB& other) const {
        return r == other.r && g == other.g && b == other.b;
    }

    bool operator!=(const RGB& other) const {
        return !(*this == other);
    }
};

class Image {
private:
    std::vector<std::vector<RGB>> pixels;
    int width = 0;
    int height = 0;

public:
    bool loadImg(const std::string& filename) {
        int channels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
        if (!data) return false;

        pixels.resize(height, std::vector<RGB>(width));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                pixels[y][x] = RGB(data[idx], data[idx+1], data[idx+2]);
            }
        }
        stbi_image_free(data);
        return true;
    }

    bool saveImg(const std::vector<std::vector<RGB>>& imgData, const std::string& filename) {
        std::vector<uint8_t> data(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx = (y * width + x) * 3;
                data[idx] = imgData[y][x].r;
                data[idx+1] = imgData[y][x].g;
                data[idx+2] = imgData[y][x].b;
            }
        }
        return stbi_write_jpg(filename.c_str(), width, height, 3, data.data(), 90);
    }

    std::vector<std::vector<RGB>> getPixels() const { return pixels; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    size_t getFileSize(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        return file.tellg();
    }
};

#endif