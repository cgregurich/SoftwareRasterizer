#pragma once
#include "Color.hpp"
#include "Vec.hpp"

class Triangle {
    public:
        Point p0;
        Point p1;
        Point p2;
        Color p0Color;
        Color p1Color;
        Color p2Color;
        Color color;

        
        // Points' and triangle's colors uninitialized
        Triangle(Point p0, Point p1, Point p2) : p0(p0), p1(p1), p2(p2) {
        }

        Triangle(Point p0, Color p0Color, Point p1, Color p1Color, Point p2, Color p2Color)
            : p0(p0), p0Color(p0Color),
              p1(p1), p1Color(p1Color),
              p2(p2), p2Color(p2Color) { /* Empty constructor body */ }

        // Points' colors uninitialized but the triangle has a color (points passed in as Points)
        Triangle(Point p0, Point p1, Point p2, Color triangleColor) : p0(p0), p1(p1), p2(p2), color(triangleColor) { /* Empty constructor body */}

        // Points' colors uninitialized but the triangle has a color (points passed in as x and y floats)
        Triangle(
            float x0, float y0, 
            float x1, float y1, 
            float x2, float y2, 
            Color triangleColor)
            : Triangle(Point(x0, y0), Point(x1, y1), Point(x2, y2), triangleColor ) { /* Empty constructor body */ }

        // Each point has its own color and the triangle has a color
        Triangle(float x0, float y0, Color p0Color, 
                 float x1, float y1, Color p1Color, 
                 float x2, float y2, Color p2Color, 
                 Color triangleColor) {

            this->p0 = Point(x0, y0);
            this->p0Color = p0Color;

            this->p1 = Point(x1, y1);
            this->p1Color = p1Color;

            this->p2 = Point(x2, y2);
            this->p2Color = p2Color;

            this->color = triangleColor;
        }
};