
#include "tgaimage.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(40,   200, 100,   255);


class Vec2 {
	public:
		int x;
		int y;
		

		Vec2() : x(x), y(x) {

		}
		Vec2(int x, int y) : y(y), x(x) {

		}
};


int main(int argc, char** argv) {
	std::cout << "hello" << std::endl;
	int width = 500;
	int height = 500;
	TGAImage image(width, height, TGAImage::RGB);
	for (int x=0; x<width; x++) {
		for (int y=0; y<height; y++) {
			image.set(x, y, white);
		}
	}
	image.set(52, 41, red);


	Vec2 upperLeft(20, 20);
	Vec2 upperRight(40, 20);
	Vec2 lowerLeft(20, 40);
	Vec2 lowerRight(40, 40);



	// Draw top line
	// for (int x=upperLeft.x; x<=upperRight.x; x++) {
	// 	image.set(x, upperRight.y, green);
	// }
	// // Draw lower line
	// for (int x=lowerLeft.x; x<=lowerRight.x; x++) {
	// 	image.set(x, lowerRight.y, green);
	// }
	// // Draw left line
	// for (int y=upperLeft.y; y<=lowerLeft.y; y++) {
	// 	image.set(lowerLeft.x, y, green);
	// }
	// // Draw right line
	// for (int y=upperRight.y; y<=lowerRight.y; y++) {
	// 	image.set(lowerRight.x, y, green);
	// }


	// Vec2 a()

	// Plot line using Bresenham's
	Vec2 a(250, 250);
	Vec2 b(400, 300);

	int dx = b.x - a.x;
	int dy = b.y - a.y;
	int y = a.y;
	int D = 2*dy - dx;

	for (int x=a.x; x<=b.x; x++) {
		image.set(x, y, green);

		if (D > 0) {
			y++;
			D -= 2*dx;
		}
		D += 2*dy;
	}



	// image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
