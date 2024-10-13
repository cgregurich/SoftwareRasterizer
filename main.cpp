
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
const TGAColor lightpink(255, 179, 222, 255);

#define WIDTH 400
#define HEIGHT 400
#define DEFAULT_COLOR purple
typedef TGAColor Color;
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
Color blendColors(Color c1, Color c2, float weight1, float weight2);


void printColor(Color c);
class Vec3 {
	public:
        float x;
        float y;
        float z;
        Color color;

		Vec3() : x(0), y(0), z(0) {
		}
		Vec3(float x, float y) : y(y), x(x), z(0) {
		}
		Vec3(float x, float y, Color c) : y(y), x(x), z(0), color(c) {
		}
		Vec3(float x, float y, float z) : y(y), x(x), z(z) {
		}

		Vec3(float x, float y, float z, Color c) : y(y), x(x), z(z), color(c) {
		}

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

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        bool normalized;
        // todo remove this as it's not used, but currently drawLine expects it?
        Color color;

        Triangle(Point p0, Point p1, Point p2, bool normalized) : p0(p0), p1(p1), p2(p2), normalized(normalized) {

        }

        Triangle(Point p0, Point p1, Point p2, Color c, bool normalized) : p0(p0), p1(p1), p2(p2), color(c), normalized(normalized) {

        }

        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c0, Color c1, Color c2, bool normalized) {
            p0 = Point(x0, y0, 0, c0);
            p1 = Point(x1, y1, 0, c1);
            p2 = Point(x2, y2, 0, c2);
            this->normalized = normalized;
        }
};

Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p);


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

Color lerpColor(Point barycentric, Color colorA, Color colorB, Color colorC) {
    int newRed = colorA.r * barycentric.x + colorB.r * barycentric.y + colorC.r * barycentric.z;
    int newGreen = colorA.g * barycentric.x + colorB.g * barycentric.y + colorC.g * barycentric.z;
    int newBlue = colorA.b * barycentric.x + colorB.b * barycentric.y + colorC.b * barycentric.z;
    Color newColor(newRed, newGreen, newBlue, 255);
    return newColor;
}

void fillTriangle(Triangle t, bool lerp) {
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

            Point barycentric = calcBarycentricCoordinates(t.p0, t.p1, t.p2, p);

            // p is inside the triangle and therefore should be colored
            if(barycentric.x > 0 && barycentric.y > 0 && barycentric.z > 0) {

                // If lerp is true then use each point's colors to interpolate the final pixel color
                if (lerp) {

                    // Blend A and B
                    Color newColor = blendColors(t.p0.color, t.p1.color, barycentric.x, barycentric.y);

                    // Then blend that color with C
                    newColor = blendColors(newColor, t.p2.color, 1-barycentric.z, barycentric.z);

                    image.set(p.x, p.y, newColor);
                }
                else { // If lerp is false then just use the triangle's overall color
                    image.set(p.x, p.y, t.color);
                }
            }

        }
    }
}

void drawTriangle(Triangle t, bool outline, bool fill, bool lerp) {
    // todo interpolate points that make up lines to decide line colors. Maybe some way to make sort of a general function? Idk.
    if (outline) {
        drawLine(t.p0, t.p1, t.normalized, t.color);
        drawLine(t.p1, t.p2, t.normalized, t.color);
        drawLine(t.p2, t.p0, t.normalized, t.color);
    }
    if (fill) {
        fillTriangle(t, lerp);
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

Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p) {
    float alpha = ((b.y - c.y)*(p.x-c.x) + (c.x - b.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float beta = ((c.y-a.y)*(p.x-c.x) + (a.x-c.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float gamma = 1 - alpha - beta;
    return Point(alpha, beta, gamma);
}

void draw3DCube() {
    Vec3 v0(.3,  .25,   0, black);
    Vec3 v1(.5,  .25,   0, cyan);
    Vec3 v2(.5,  .425,  0, cyan);
    Vec3 v3(.4,  .6,    0, cyan);
    Vec3 v4(.2,  .6,    0, cyan);
    Vec3 v5(.2,  .4,    0, cyan);
    Vec3 v6(.4,  .4,    0, white);

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
        drawTriangle(triangles[i], true, true, false);
    }
    image.write_tga_file("output.tga");

}

void printColor(Color c) {
    std::cout << "Color: (" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ")\n";
}

Color blendColorsByAlpha(Color bg, Color fg) {
    Color blended(fg.a*(fg.r)+(1-fg.a)*bg.r, fg.a*fg.g+(1-fg.a)*bg.g, fg.a*fg.b+(1-fg.a)*bg.b, 255);
    return blended;
}

/*
Overload blendColors() to easily blend two colors, using
weight of 0.5 for both
*/
Color blendColors(Color c1, Color c2) {
    return blendColors(c1, c2, 0.5, 0.5);
}

Color blendColors(Color c1, Color c2, float weight1, float weight2) {

    int newRed = c1.r * weight1 + c2.r * weight2;
    int newGreen = c1.g * weight1 + c2.g * weight2;
    int newBlue = c1.b * weight1 + c2.b * weight2;

    // Divide by the sum of weights to account for situations
    // where the sum of the weights is less than 1.0
    newRed /= weight1 + weight2;
    newGreen /= weight1 + weight2;
    newBlue /= weight1 + weight2;
    return Color(newRed, newGreen, newBlue, 255);
}

int main(int argc, char** argv) {
    Color blended = blendColors(red, blue, 0.6, 0.4);
    setBackgroundColor(blended);

    Point a(0.21, 0.11, lightpink);
    Point b(0.61, 0.21, yellow);
    Point c(0.11, 0.51, blendColors(red, yellow, 0.8, 0.2));
    a.color.a = 127;
    b.color.a = 127;
    c.color.a = 127;
    Triangle t(a, b, c, true);

    drawTriangle(t, false, true, true);

    // todo: implement vec3 math operations

	image.write_tga_file("output.tga");
	return 0;
}
