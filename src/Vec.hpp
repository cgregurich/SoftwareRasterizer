#pragma once
#include <iostream>

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

        friend std::ostream& operator<<(std::ostream &os, const Vec3 &v) {
            os << "Vec3: (" << v.x << ", " << v.y << ", " << v.z << ")";
            return os;
        }

        Vec3 operator-(const Vec3 &other) const {
            return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
        }

        Vec3 operator+(const Vec3 &other) const {
            return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
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
class Vec4 {
	public:
        float x;
        float y;
        float z;
        float w;

		Vec4() : x(0), y(0), z(0), w(0) {
		}
		Vec4(float x, float y) : x(x), y(y), z(0), w(0) {
		}
		Vec4(float x, float y, float z) : x(x), y(y), z(z), w(0) {
		}
		Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
		}
        Vec4(Vec3 vec3, float w) : Vec4(vec3.x, vec3.y, vec3.z, w) {}

        friend std::ostream& operator<<(std::ostream &os, const Vec4 &v) {
            os << "Vec4: (" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
            return os;
        }

        Vec4 operator-(const Vec4 &other) const {
            return Vec4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
        }

        Vec4 operator+(const Vec4 &other) const {
            return Vec4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
        }


};
/* Typedefs for to make it more clear what the Vec3 is
being used for in certain scenarios */
typedef Vec3 Point;
typedef Vec3 Vertex;