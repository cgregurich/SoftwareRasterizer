#include <iostream>
#include "Color.hpp"




class Vec3 {
	public:
        float x;
        float y;
        float z;
        Color color;

        /* Constructors */
		Vec3() : x(0), y(0), z(0) {}
		Vec3(float value) : x(value), y(value), z(value) { }
		Vec3(float x, float y) : x(x), y(y), z(0) { }
		Vec3(float x, float y, Color c) : x(x), y(y), z(0), color(c) {}
		Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vec3(float x, float y, float z, Color c) : x(x), y(y), z(z), color(c) {}



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
typedef Vec3 Vertex;

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        Color color;

        
        // Points' and triangle's colors uninitialized
        Triangle(Point p0, Point p1, Point p2) : p0(p0), p1(p1), p2(p2) {

        }

        // Points' colors uninitialized
        Triangle(Point p0, Point p1, Point p2, Color c) : p0(p0), p1(p1), p2(p2), color(c) {

        }

        // Points' colors uninitialized
        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c) : Triangle(Point(x0, y0), Point(x1, y1), Point(x2, y2), c ){
        }

        // Triangle's color uninitialized
        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c0, Color c1, Color c2) {
            p0 = Point(x0, y0, 0, c0);
            p1 = Point(x1, y1, 0, c1);
            p2 = Point(x2, y2, 0, c2);
        }
};


/*===========================================
Coordinate Stuff
===========================================*/

/*
NDC:
    x: [-1, 1]
    y: [-1, 1]
    origin: center

Screen:
    x: [0, width]
    y: [0, height]
    origin: upper left

NormalizedScreen:
    x: [0, 1]
    y: [0, 1]
    origin: upper left
*/
enum class CoordinateType {
    NormalizedDeviceCoordinates,
    NDC = NormalizedDeviceCoordinates,
    Screen,
    NormalizedScreen,
};

/*
Converts a Point with x and y values in range [-1, 1] to a Point
with x and y values in the range of [0, width] and [0, height] respectively.
*/
Point NDCToScreen(const Point ndc, int width, int height) {
    float pixelX = ndc.x * width/2 + width/2;
    float pixelY = -(ndc.y * height/2) + height/2;
    return Point(pixelX, pixelY, ndc.color);
}


Point NDCToNormalizedScreen(Point ndc, int width, int height) {
    float screenX = ndc.x * width/2 + width/2;
    float normScreenX = screenX / width;
    float screenY = -(ndc.y * height/2) + height/2;
    float normScreenY = screenY / height;

    return Point(normScreenX, normScreenY, ndc.color);
}