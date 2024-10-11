#include <stdio.h>
typedef struct Point {
    float x;
    float y;
    float z;
} Point;

Point barycentricCoords(Point a, Point b, Point c, Point p) {
    float alpha = ((b.y - c.y)*(p.x-c.x) + (c.x - b.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float beta = ((c.y-a.y)*(p.x-c.x) + (a.x-c.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float gamma = 1 - alpha - beta;
    return (Point){alpha, beta, gamma};
}

int main() {
    Point a = {2, 1, 0};
    Point b = {6, 2, 0};
    Point c = {1, 5, 0};
    Point p = {3, 3, 0};

    Point bc = barycentricCoords(a, b, c, p);

    printf("(%f, %f, %f)\n", bc.x, bc.y, bc.z);



}