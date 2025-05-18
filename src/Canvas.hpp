#pragma once
#include "Vec.hpp"
#include "Color.hpp"
#include "Triangle.hpp"
#include "CoordinateSpaces.hpp"
#include <array>
#include "tgaimage.h"
#include <string>

// todo remove uses of these and delete
#define DEFAULT_COLOR black
#define WIDTH 100
#define HEIGHT 100

class Canvas {
    public:
        Canvas(std::string imageFileName, int width, int height, int bpp) {
            this->image = TGAImage(width, height, bpp);
            this->imageFileName = imageFileName;
        }
        void save();
        // todo remove overloaded functions? they kind of just clutter the API
        void drawLine(Point a, Point b, CoordinateType coordType, Color color);
        void drawLine(Point a, Point b, CoordinateType coordType);
        void drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType, Color color);
        void drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType);
        void setBackgroundColor(Color color);
        void fillTriangle(const Triangle t, const CoordinateType coordType, const bool lerp);
        void drawTriangle(Triangle t, CoordinateType coordType, bool outline, bool fill, bool lerp);
        void drawRectangle(const int width, const int height, const Point center);
        void drawPoint(Point p, Color color, CoordinateType coordType);
        void draw3DCube();
        void drawCoordinatePlane();
        void drawGrid(int gridResolution);



    private:
        std::string imageFileName;
        TGAImage image;
};

/*=============================
Helper functions
=============================*/
Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p);
Color blendColors(const Color c1, const Color c2);
Color blendColors(const Color c1, const Color c2, const float weight1, const float weight2);
Color blendColorsByAlpha(const Color bg, const Color fg);
Color lerpColor(Point barycentric, Color colorA, Color colorB, Color colorC);
void printColor(Color c);
