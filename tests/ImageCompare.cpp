#include <iostream>
#include <vector>
#include "../src/tgaimage.h"

bool compareImages(TGAImage image1, TGAImage image2) {
    int image1Width = image1.get_width();
    int image1Height = image1.get_height();
    int image2Width = image2.get_width();
    int image2Height = image2.get_height();
    if (image1Width != image2Width || image2Width != image2Height) {
        std::cerr << "[Error] compareImages() failed due to mismatching image dimesions" << std::endl;
        return false;
    }
    for (int x=0; x<image1Width; ++x) {
        for (int y=0; y<image1Height; ++y) {
            if (image1.get(x, y) != image2.get(x, y)) {
                return false;
            }
        }
    }
    return true;
}


int main() {
    int width = 100;
    int height = 100;
    TGAImage image1(width, height, TGAImage::RGB);
    TGAImage image2(width, height, TGAImage::RGB);
    
    
}