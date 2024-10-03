
#include "tgaimage.h"
#include <iostream>
#include <cstdlib>

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

typedef Vec2 Point;

void plotLineLow(Point a, Point b, TGAImage &image) {
	// Used when slope is horizontal than vertical
	// i.e. delta X > delta Y

	std::cout << "plotLineLow\n";
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	int yi = 1;

	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}

	int D = 2*dy - dx;
	int y = a.y;

	for (int x=a.x; x<=b.x; x++) {
		image.set(x, y, green);

		if (D > 0) {
			y += yi;
			D += 2 * (dy - dx);
		}
		else {
			D += 2 * dy;
		}
	}
}

void plotLineHigh(Point a, Point b, TGAImage &image) {
	// Used when slope is more vertical than horizontal
	// i.e. delta Y > delta X
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	int xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}

	int D = 2*dy - dx;
	int x = a.x;
	for (int y=a.y; y<=b.y; y++) {
		image.set(x, y, red);
		if (D > 0) {
			x += xi;
			D += 2 * (dx - dy);
		}
		else {
			D += 2 * dx;
		}
	}
}

void plotLine(Point a, Point b, TGAImage &image) {
	if (abs(b.y - a.y) < abs(b.x - a.x)) {
		if (a.x > b.x) {
			plotLineLow(b, a, image);
		}
		else {
			plotLineLow(a, b, image);
		}
	}

	else {
		if (a.y > b.y) {
			plotLineHigh(b, a, image);
		}
		else {
			plotLineHigh(a, b, image);
		}

	}
}

// void plotLine(TGAImage &image, Vec2 a, Vec2 b) {
// 	// Plot line using Bresenham's


// 	int dx = b.x - a.x;
// 	int dy = b.y - a.y;
// 	int y = a.y;
// 	int D = 2*dy - dx;

// 	for (int x=a.x; x<=b.x; x++) {
// 		image.set(x, y, green);

// 		if (D > 0) {
// 			y++;
// 			D -= 2*dx;
// 		}
// 		D += 2*dy;
// 	}
// }

Point makeNormalizedPoint(int width, int height, float x, float y) {
	Point p(width * x, height * y);
	return p;
}

void plotLine(float x0, float y0, float x1, float y1, bool normalized, TGAImage &image) {
	if (normalized) {
		if (x0 > 1.0 || y0 > 1.0 || x1 > 1.0 || y1 > 1.0) {
			std::cerr << "ERROR: Normalized values for plotLine are outside of valid range. Expect some weird shit" << std::endl;
		}
		x0 = x0 * image.get_width();
		y0 = y0 * image.get_height();
		x1 = x1 * image.get_width();
		y1 = y1 * image.get_height();
	}
	plotLine(Point(x0, y0), Point(x1, y1), image);
}

int main(int argc, char** argv) {
	int width = 1000;
	int height = 1000;
	TGAImage image(width, height, TGAImage::RGB);
	for (int x=0; x<width; x++) {
		for (int y=0; y<height; y++) {
			image.set(x, y, white);
		}
	}


	plotLine(.49, .5, .5, .75, true, image);
	plotLine(.5, .5, .75, .5, true, image);
	plotLine(.75, .75, .75, .5, true, image);
	plotLine(.5, .75, .75, .75, true, image);

	plotLine(0.2, 0.2, 0.4, 0.6, true, image);
	plotLine(0.4, 0.6, 0.6, 0.2, true, image);



	// image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
