#include <iostream>
#include <chrono>
#include <iomanip>
#include "Image.hpp"
#include "QuadTree.hpp"
#include <filesystem>

using namespace std;
using namespace std::chrono;
namespace fs = filesystem;

void printHeader() {
    cout << "\n";
    cout << "------------------------------------------------------------------------\n";
    cout << "|    Q U A D T R E E   I M A G E   C O M P R E S S I O N   T O O L     |\n";
    cout << "------------------------------------------------------------------------\n";
    cout << "\n";
}

bool validateImage(const string& path) {
    if (!fs::exists(path)) {
        cerr << "Error: File does not exist\n";
        return false;
    }
    
    ifstream file(path, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file\n";
        return false;
    }
    char header[8];
    file.read(header, 8);
    return (header[0] == -1 && header[1] == -40) || (header[0] == -119 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G');
}

bool validateThreshold(int method, double threshold) {
    const pair<double, double> ranges[] = {
        {0, 65025}, 
        {0, 255},  
        {0, 255},   
        {0, 8},
        {0, 1}     
    };
    return threshold >= ranges[method-1].first && threshold <= ranges[method-1].second;
}

int main() {
    printHeader();

    string inputPath;
    cout << "Input image path:\n>> ";
    cin >> inputPath;
    
    if (!validateImage(inputPath)) return 1;

    Image img;
    if (!img.loadImage(inputPath)) {
        cerr << "Error: Failed to load image\n";
        return 1;
    }

    int method;
    cout << "\nSelect Error measurement method (1-4):" << endl;
    cout << "1. Variance (0-65025)" << endl;
    cout << "2. MAD (0-255)" << endl;
    cout << "3. Max Pixel Difference (0-255)" << endl;
    cout << "4. Entropy (0-8)" << endl;
    cout << "5. SSIM (0-1)" << endl;
    cout << ">> ";
    cin >> method;
    if (method < 1 || method > 5){
        cerr << "Error: Invalid method selected\n";
        return 1;
    }

    double threshold;
    cout << "\nEnter threshold:\n>> ";
    cin >> threshold;
    if (!validateThreshold(method, threshold)) {
        cerr << "Error: Invalid threshold for selected method\n";
        return 1;
    }

    int minBlock;
    cout << "\nEnter minimum block size (pixels):\n>> ";
    cin >> minBlock;
    if (minBlock < 0){
        cerr << "Error: Invalid minimum block size\n";
        return 1;
    }

    // double targetCompression;
    // cout << "\nEnter target compression ratio (0-1, 0 to use threshold):\n>> ";
    // cin >> targetCompression;
    // if (targetCompression < 0 || targetCompression > 1){
    //     cerr << "Error: Invalid target compression ratio\n";
    //     return 1;
    // }

    string outputPath;
    cout << "\nOutput image path:\n>> ";
    cin >> outputPath;
    fs::create_directories(fs::path(outputPath).parent_path());

    // string gifPath;
    // cout << "\nOutput GIF path (leave empty to skip):\n>> ";
    // cin.ignore(); // Membersihkan newline sebelumnya
    // getline(cin, gifPath);

    auto start = high_resolution_clock::now();
    
    QuadTree quadtree(threshold, minBlock, method);
    // if (targetCompression > 0) {
    //     size_t originalSize = img.getFileSize(inputPath);
    //     quadtree.adjustThresholdForCompression(img.getPixels(), targetCompression, originalSize);
    // }
    quadtree.compress(img.getPixels());
    auto compressedImg = quadtree.reconstructImage();
    img.saveImage(compressedImg, outputPath);
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    // if (!gifPath.empty()) {
    //     if (quadtree.saveCompressionGIF(gifPath)) {
    //         cout << "GIF created successfully at: " << gifPath << endl;
    //     } else {
    //         cerr << "Failed to create GIF" << endl;
    //     }
    // }

    size_t originalSize = img.getFileSize(inputPath);
    size_t compressedSize = img.getFileSize(outputPath);
    double ratio = 100.0 * (1.0 - (double)compressedSize/originalSize);

    cout << fixed << setprecision(2);
    cout << "\n";
    cout << "------------------------------------------------\n";
    cout << "|    C O M P R E S S I O N   R E S U L T S     |\n";
    cout << "------------------------------------------------\n";
    cout << "\n";
    cout << "Original size      : " << originalSize << " bytes" << endl;
    cout << "Compressed size    : " << compressedSize << " bytes" << endl;
    cout << "Compression ratio  : " << ratio << "%" << endl;
    cout << "Error threshold    : " << threshold << endl;
    cout << "Min Block size     : " << minBlock << " pixels" << endl;
    cout << "Processing time    : " << duration.count() << " ms" << endl;
    cout << "Quadtree nodes     : " << quadtree.countNodes() << endl;
    cout << "Leaf nodes         : " << quadtree.countLeaves() << endl;
    cout << "Tree depth         : " << quadtree.getDepth() << endl << endl;

    return 0;
}