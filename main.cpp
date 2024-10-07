
#include "tgaimage.h"
#include <iostream>
#include <cstdlib>

const TGAColor white(255, 255, 255, 255);
const TGAColor red(255, 0,   0,   255);
const TGAColor green(40,   200, 100,   255);
const TGAColor purple(127,   0, 127,   255);
#define WIDTH 100
#define HEIGHT 100
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);


class Vec2 {
	public:
        float x;
        float y;

		Vec2() : x(x), y(x) {
		}
		Vec2(float x, float y) : y(y), x(x) {
		}

};

typedef Vec2 Point;

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        bool areNormalized;

        Triangle(Point p0, Point p1, Point p2) : p0(p0), p1(p1), p2(p2) {

        }

        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, bool areNormalized) {
            p0 = Point(x0, y0);
            p1 = Point(x1, y1);
            p2 = Point(x2, y2);
            this->areNormalized = areNormalized;
        }
};


void drawLine(Point a, Point b, bool areNormalized) {
    // This version does everything in one function
    // instead of using helper functions for high vs low lines

    if (areNormalized) {
        a.x *= image.get_width();
        a.y *= image.get_height();
        b.x *= image.get_width();
        b.y *= image.get_height();
    }

    int x0, x1, y0, y1;
    x0 = a.x;
    y0 = a.y;
    x1 = b.x;
    y1 = b.y;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    int x = x0;
    int y = y0;

    while (true) {
        image.set(x, y, purple);
        if (x == x1 && y == y1) break;
        int e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y += sy;
        }
    }
}

void drawLine(float x0, float y0, float x1, float y1, bool areNormalized) {
	drawLine(Point(x0, y0), Point(x1, y1), areNormalized);
}

void setBackgroundColor(TGAColor color) {
    for (int x=0; x<=image.get_width(); x++) {
        for (int y=0; y<=image.get_height(); y++) {
            image.set(x, y, color);
        }
    }
}

void drawTriangle(Triangle t) {
    drawLine(t.p0, t.p1, t.areNormalized);
    drawLine(t.p1, t.p2, t.areNormalized);
    drawLine(t.p2, t.p0, t.areNormalized);
}

void drawRectangle(int width, int height, Point center) {

    Point ul(center.x - (width/2), center.y - (height/2));
    Point ur(center.x + (width/2), center.y - (height/2));
    Point ll(center.x - (width/2), center.y + (height/2));
    Point lr(center.x + (width/2), center.y + (height/2));

    drawLine(ul, ur, false);
    drawLine(ur, lr, false);
    drawLine(lr, ll, false);
    drawLine(ll, ul, false);
}


int main(int argc, char** argv) {
	setBackgroundColor(white);

    Triangle t(.2, .1, .5, .3, .3, .4, true);
    drawTriangle(t);





	image.write_tga_file("output.tga");
	return 0;
}
