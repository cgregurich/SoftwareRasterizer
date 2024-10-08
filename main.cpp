
#include "tgaimage.h"
#include <iostream>
#include <cstdlib>
#include <array>

const TGAColor black(0,0,0, 255);
const TGAColor white(255,255,255, 255);
const TGAColor red(255,0,0, 255);
const TGAColor lime(0,255,0, 255);
const TGAColor blue(0,0,255, 255);
const TGAColor yellow(255,255,0, 255);
const TGAColor cyan(0,255,255, 255);
const TGAColor magenta(255,0,255, 255);
const TGAColor silver(192,192,192, 255);
const TGAColor gray(128,128,128, 255);
const TGAColor maroon(128,0,0, 255);
const TGAColor olive(128,128,0, 255);
const TGAColor green(0,128,0, 255);
const TGAColor purple(128,0,128, 255);
const TGAColor lightPurple(190, 142, 190, 255);
const TGAColor teal(0,128,128, 255);
const TGAColor navy(0,0,128, 255);

#define WIDTH 1000
#define HEIGHT 1000
#define DEFAULT_COLOR purple
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);



class Vec3 {
	public:
        float x;
        float y;
        float z;

		Vec3() : x(0), y(0), z(0) {
		}
		Vec3(float x, float y) : y(y), x(x), z(0) {
		}
		Vec3(float x, float y, float z) : y(y), x(x), z(z) {
		}

        // friend std::ostream& operator<<(std::ostream &os, const Vec3 &v) {
        friend std::ostream& operator<<(std::ostream &os, const Vec3 &v) {
            os << "Vec3: (" << v.x << ", " << v.y << ", " << v.z << ")";
            return os;
        }

        Vec3 operator-(const Vec3 &other) const {
            return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
        }

        static Vec3 cross(Vec3 a, Vec3 b) {
            Vec3 result(
                (a.y * b.z - a.z * b.y),
                (a.z*b.x - a.x*b.z),
                (a.x*b.y - a.y*b.x)
                );
            return result;
        }
};

typedef Vec3 Point;
typedef TGAColor Color;

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        bool normalized;
        Color color;

        Triangle(Point p0, Point p1, Point p2, Color c, bool normalized) : p0(p0), p1(p1), p2(p2), color(c), normalized(normalized) {

        }

        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c, bool normalized) {
            p0 = Point(x0, y0);
            p1 = Point(x1, y1);
            p2 = Point(x2, y2);
            color = c;
            this->normalized = normalized;
        }
};


void drawLine(Point a, Point b, bool normalized, Color color) {
    // This version does everything in one function
    // instead of using helper functions for high vs low lines

    if (normalized) {
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
        image.set(x, y, color);
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

void drawLine(float x0, float y0, float x1, float y1, bool normalized) {
	drawLine(Point(x0, y0), Point(x1, y1), normalized, DEFAULT_COLOR);
}

void setBackgroundColor(Color color) {
    for (int x=0; x<=image.get_width(); x++) {
        for (int y=0; y<=image.get_height(); y++) {
            image.set(x, y, color);
        }
    }
}

void fillTriangle(Triangle t) {
    if (t.normalized) {
        t.p0.x *= image.get_width();
        t.p0.y *= image.get_height();
        t.p1.x *= image.get_width();
        t.p1.y *= image.get_height();
        t.p2.x *= image.get_width();
        t.p2.y *= image.get_height();
    }
    for (int x=0; x<WIDTH; x++) {
        for (int y=0; y<HEIGHT; y++) {
            Point p(x, y);
            Vec3 edgeVec, pointVec;
            Vec3 results[3];

            // p0 to p1
            edgeVec = t.p1 - t.p0;
            pointVec = p - t.p0;
            results[0] = Vec3::cross(edgeVec, pointVec);

            // p1 to p2
            edgeVec = t.p2 - t.p1;
            pointVec = p - t.p1;
            results[1] = Vec3::cross(edgeVec, pointVec);

            // p2 to p0
            edgeVec = t.p0 - t.p2;
            pointVec = p - t.p2;
            results[2] = Vec3::cross(edgeVec, pointVec);

            Vec3 crossRes = Vec3::cross(edgeVec, pointVec);
            // std::cout << results[0] << std::endl;
            // std::cout << results[1] << std::endl;
            // std::cout << results[2] << std::endl;

            if ((results[0].z > 0 && results[1].z > 0 && results[2].z > 0) || (results[0].z < 0 && results[1].z < 0 && results[2].z < 0)) {
                // Pixel should be colored
                image.set(p.x, p.y, t.color);
            }

        }
    }
}

void drawTriangle(Triangle t, bool fill) {
    drawLine(t.p0, t.p1, t.normalized, t.color);
    drawLine(t.p1, t.p2, t.normalized, t.color);
    drawLine(t.p2, t.p0, t.normalized, t.color);
    if (fill) {
        fillTriangle(t);
    }
}

void drawRectangle(int width, int height, Point center) {

    Point ul(center.x - (width/2), center.y - (height/2));
    Point ur(center.x + (width/2), center.y - (height/2));
    Point ll(center.x - (width/2), center.y + (height/2));
    Point lr(center.x + (width/2), center.y + (height/2));

    drawLine(ul, ur, false, DEFAULT_COLOR);
    drawLine(ur, lr, false, DEFAULT_COLOR);
    drawLine(lr, ll, false, DEFAULT_COLOR);
    drawLine(ll, ul, false, DEFAULT_COLOR);
}

void drawPoint(Point p) {
    image.set(p.x, p.y, purple);
}

void drawPoint(Point p, Color color) {
    image.set(p.x, p.y, color);
}


int main(int argc, char** argv) {
	setBackgroundColor(white);


    Vec3 v0(.3,  .25,   0);
    Vec3 v1(.5,  .25,   0);
    Vec3 v2(.5,  .425, 0);
    Vec3 v3(.4,  .6,   0);
    Vec3 v4(.2,  .6,   0);
    Vec3 v5(.2,  .4,   0);
    Vec3 v6(.4,  .4,   0);

    Color colorSide(0xca, 0xdb, 0xed, 0xff);
    Color colorFront(0x9a, 0xb9, 0xd5, 0xff);
    Color colorTop(0x86, 0xad, 0xd6, 255);

    Triangle a(v0, v6, v5, colorTop, true);
    Triangle b(v0, v1, v6, colorTop, true);
    Triangle c(v1, v2, v6, colorSide, true);
    Triangle d(v6, v2, v3, colorSide, true);
    Triangle e(v6, v3, v4, colorFront, true);
    Triangle f(v5, v6, v4, colorFront, true);

    std::array<Triangle, 6> triangles = {a, b, c, d, e, f};
    
    for (int i=0; i<triangles.size(); i++) {
        drawTriangle(triangles[i], true);
    }



	image.write_tga_file("output.tga");
	return 0;
}
