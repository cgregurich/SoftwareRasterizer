#include <iostream>
#include "Color.hpp"

enum class CoordinateType {
    Cartesian,
    Screen
};


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
typedef Vec3 Vertex;

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        bool normalized;
        Color color;

        
        // Points' and triangle's colors uninitialized
        Triangle(Point p0, Point p1, Point p2, bool normalized) : p0(p0), p1(p1), p2(p2), normalized(normalized) {

        }

        // Points' colors uninitialized
        Triangle(Point p0, Point p1, Point p2, Color c, bool normalized) : p0(p0), p1(p1), p2(p2), color(c), normalized(normalized) {

        }

        // Points' colors uninitialized
        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c, bool normalized) : Triangle(Point(x0, y0), Point(x1, y1), Point(x2, y2), c, normalized ){
        }

        // Triangle's color uninitialized
        Triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color c0, Color c1, Color c2, bool normalized) {
            p0 = Point(x0, y0, 0, c0);
            p1 = Point(x1, y1, 0, c1);
            p2 = Point(x2, y2, 0, c2);
            this->normalized = normalized;
        }
};