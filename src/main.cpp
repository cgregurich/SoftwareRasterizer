
#include "tgaimage.h"
#include <iostream>
#include <cstdlib>
#include <array>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>

#include "Geometry.hpp"
#include "Matrix.hpp"


/*-------------------------------------
IMAGE SETUP/CREATION
-------------------------------------*/
#define WIDTH 600
#define HEIGHT 600
#define DEFAULT_COLOR black
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);


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

/* Overloaded: uses hardcoded color instead of arg */
void drawLine(Point a, Point b, bool normalized) {
    drawLine(a, b, normalized, DEFAULT_COLOR);
}

/* Overloaded: four floats instead of two Points */
void drawLine(float x0, float y0, float x1, float y1, bool normalized, Color color) {
    drawLine(Point(x0, y0), Point(x1, y1), normalized, color);
}

/* Overloaded: uses hardcoded color instead of arg */
void drawLine(float x0, float y0, float x1, float y1, bool normalized) {
    drawLine(x0, y0, x1, y1, normalized, DEFAULT_COLOR);
}


/*-------------------------------------
FUNCTION DECLARATIONS
-------------------------------------*/
Color blendColors(Color c1, Color c2, float weight1, float weight2);
void printColor(Color c);
Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p);


/*-------------------------------------
FUNCTION DEFINITIONS
-------------------------------------*/
/*
Sets all pixels in the image to the given color
*/
void setBackgroundColor(Color color) {
    for (int x=0; x<=image.get_width(); x++) {
        for (int y=0; y<=image.get_height(); y++) {
            image.set(x, y, color);
        }
    }
}

/*
Does linear interpolation of three colors based on a barycentric coordinate
*/
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
                    // Blend color with bg (only has an effect if triangle's color has some transparency)
                    // Try blending with background??
                    Color bg = image.get(p.x, p.y);
                    // printColor(bg);
                    float fgWeight = t.color.a / 255.0;
                    float bgWeight = 1 - fgWeight;
                    Color newColor = blendColors(t.color, bg, fgWeight, bgWeight);

                    image.set(p.x, p.y, newColor);
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

/*
Draw a wireframe rectangle. Not particularly robust i.e. doesn't use
normalized coordinates, can't be filled, etc.
*/
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

/*
Sets a point a hard-coded color
*/
void drawPoint(Point p, bool normalized) {
    int x, y;
    if (normalized) {
        x = WIDTH * p.x;
        y = HEIGHT * p.y;
    }
    else {
        x = p.x;
        y = p.y;
    }
    image.set(x, y, DEFAULT_COLOR);
}

/*
Sets a point a given color
*/
void drawPoint(Point p, Color color) {
    image.set(p.x, p.y, color);
}

Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p) {
    float alpha = ((b.y - c.y)*(p.x-c.x) + (c.x - b.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float beta = ((c.y-a.y)*(p.x-c.x) + (a.x-c.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float gamma = 1 - alpha - beta;
    return Point(alpha, beta, gamma);
}

/*
Very specific sandbox function. Probably not needed
*/
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

    Triangle a(v0, v6, v5, true, colorTop);
    Triangle b(v0, v1, v6, true, colorTop);
    Triangle c(v1, v2, v6, true, colorSide);
    Triangle d(v6, v2, v3, true, colorSide);
    Triangle e(v6, v3, v4, true, colorFront);
    Triangle f(v5, v6, v4, true, colorFront);

    std::array<Triangle, 6> triangles = {a, b, c, d, e, f};
    
    for (int i=0; i<triangles.size(); i++) {
        drawTriangle(triangles[i], true, true, false);
    }
    image.write_tga_file("output.tga");

}

void printColor(Color c) {
    std::cout << "Color: (" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ")\n";
}

// FIXME
Color blendColorsByAlpha(Color bg, Color fg) {

    /* Color's alpha is 0-255 but we want to work with floats for
    these calculations */
    float fgWeight = fg.a / 255.0;
    float bgWeight = 1 - fgWeight;

    int newRed = fgWeight*(fg.r) + bgWeight*bg.r;
    int newGreen = fgWeight*fg.g + bgWeight*bg.g;
    int newBlue = fgWeight*fg.b + bgWeight*bg.b;
    std::cout << "newRed=" << newRed << std::endl;
    // Color blended(fg.a*(fg.r)+(1-fg.a)*bg.r, fg.a*fg.g+(1-fg.a)*bg.g, fg.a*fg.b+(1-fg.a)*bg.b, 255);
    Color blended(newRed, newGreen, newBlue, 255);
    printColor(blended);
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


/*
Convert normalized cartesian coordinates
to normalized screen coordinates.
Not sure if I'm using these terms correctly, so what I mean is:

Screen coordinates:
Origin in upper left
y is positive going down
x is positive going right
Negative values are out of bounds
Normalized range is 0.0 to 1.0

Cartesian coordinates:
Origin is in the center
Left is negative x
Right is positive x
Down is negative y
Up is positive y
Negative and positive values are valid
Normalized range is -1.0 to 1.0
*/
Vec3 normCartToNormScreen(Vec3 normCart, int width, int height) {
    float screenX = normCart.x * width/2 + width/2;
    float normScreenX = screenX / width;
    float screenY = -(normCart.y * height/2) + width/2;
    float normScreenY = screenY / height;

    return Vec3(normScreenX, normScreenY);
}

bool parseVertexFromObjLine(std::string rawLine, Vertex &vertex) {
    /*
    Currently working with simple .obj files with lines like:
    v 0.5 0.12 0.654

    So we're parsing them as:
    <string> <float> <float> <float>

    And passing these three floats into the given Vertex's x, y, and z
    */
    std::stringstream ss(rawLine);

    /* Check if line can is a vertex line */
    std::string keyword;
    ss >> keyword;
    if (keyword != "v") return false;

    /* Parse x, y, and z values from line */
    float x, y, z;
    ss >> x >> y >> z;

    vertex.x = x;
    vertex.y = y;
    vertex.z = z;

    return true;
}

bool parseFaceFromObjLine(std::string rawLine, std::vector<int> &vertexIndices) {
    std::stringstream ss(rawLine);
    std::string keyword;
    ss >> keyword;
    if (keyword != "f") return false;

    /* Parse vertex indices from line */
    /* Line example:
    f 24/1/25 25/2/25 26/3/26
    */
    int a, b, c;
    ss >> a >> b >> c;

    vertexIndices[0] = a;
    vertexIndices[1] = b;
    vertexIndices[2] = c;

    // std::cout << "a=" << a << "  b=" << b << "  c=" << c << std::endl;

    return true;
}

void drawObj(std::string filepath, CoordinateType coordType ) {
    // Model model(filepath, coordType);

    ///////////////
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    std::string line; // Line read from file
    std::vector<Vertex> vertices; // Vertices parsed from file
    Vertex vertex;

    /* Parse all the vertex position lines and turn them into a vector of vertices
    which will be used to index into based off the faces */
    while (std::getline(file, line)) {
        if (parseVertexFromObjLine(line, vertex)) {
            vertices.push_back(vertex);
        }
    }
    std::cout << "Done parsing vertices" << std::endl;


    /* Parse the face elements */
    // todo: can optimize by reading file once
    file.clear(); 
    file.seekg(0);
    std::vector<int> vertexIndices(3);
    int faceCounter = 0;
    while (std::getline(file, line)) {
        if (parseFaceFromObjLine(line, vertexIndices)) {
            // todo this is just for debugging/seeing progress
            faceCounter++;
            if (faceCounter % 100 == 0) std::cout << "faceCounter=" << faceCounter << std::endl;
            // todo end

            int v1Index, v2Index, v3Index;
            v1Index = vertexIndices[0];
            v2Index = vertexIndices[1];
            v3Index = vertexIndices[2];

            Vertex v1, v2, v3;
            v1 = vertices[v1Index-1];
            v2 = vertices[v2Index-1];
            v3 = vertices[v3Index-1];

            /* Need to convert to screen coords if the input is Cartesian */
            if (coordType == CoordinateType::Cartesian) {
                v1 = normCartToNormScreen(v1, WIDTH, HEIGHT);
                v2 = normCartToNormScreen(v2, WIDTH, HEIGHT);
                v3 = normCartToNormScreen(v3, WIDTH, HEIGHT);
            }

            Triangle t(v1, v2, v3, true);
            drawTriangle(t, true, true, false);
        }
    }

    file.close();
}


// todo fix parsing obj file (instead of having to manually modify the file to accomodate my flawed face element parsing logic)



#define DEG_TO_RAD(deg) deg * M_PI/180

template <typename T>
Point pointMatrixMultiply(Point p, const Matrix<T>& m) {
    if (m.getRows() != 3) {
        throw std::invalid_argument("Matrix has invalid number of rows for multiplication.");
    }
    T newX, newY, newZ;
    newX = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0];
    newY = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1];
    newZ = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2];
    Point newP(newX, newY, newZ);

    return newP;
}

/* Rotate point about the z axis */
Point rotatePointZ(Point p, int degrees) {
    double rad = DEG_TO_RAD(degrees);
    Matrix<double> rotateMatrix(
        {
            { cos(rad),     sin(rad),   0 },
            { -sin(rad),    cos(rad),   0 },
            { 0,            0,          1 }
        }
    );
    float x, y, z;
    Point newP = pointMatrixMultiply(p, rotateMatrix);
    return newP;
}

Triangle rotateTriangleZ(Triangle t, int degrees) {
    Point rotatedA, rotatedB, rotatedC;
    rotatedA = rotatePointZ(t.p0, degrees);
    rotatedB = rotatePointZ(t.p1, degrees);
    rotatedC = rotatePointZ(t.p2, degrees);
    Triangle rotated(rotatedA, rotatedB, rotatedC, t.normalized, yellow);
    return rotated;
}

void drawCoordinatePlane() {
    Color color = white;
    /* Draw grid */
    drawLine(0.5, -0.1, 0.5, 1.1, true, white);
    drawLine(-0.1, 0.5, 1.1, 0.5, true, white);

    int ticks = 10;
    float tickIncrement = 0.5 / ticks;

    /* Draw positive x ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.5 + tickIncrement * i;
        tickY0 = 0.49;
        tickX1 = 0.5 + tickIncrement * i;
        tickY1 = 0.51;
        drawLine(tickX0, tickY0, tickX1, tickY1, true, white);
    }

    /* Draw negative x ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.5 - tickIncrement * i;
        tickY0 = 0.49;
        tickX1 = 0.5 - tickIncrement * i;
        tickY1 = 0.51;
        drawLine(tickX0, tickY0, tickX1, tickY1, true, white);
    }

    /* Draw negative y ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.49;
        tickY0 = 0.5 + tickIncrement * i;
        tickX1 = 0.51;
        tickY1 = 0.5 + tickIncrement * i;
        drawLine(tickX0, tickY0, tickX1, tickY1, true, white);
    }

    /* Draw positive y ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.49;
        tickY0 = 0.5 - tickIncrement * i;
        tickX1 = 0.51;
        tickY1 = 0.5 - tickIncrement * i;
        drawLine(tickX0, tickY0, tickX1, tickY1, true, white);
    }
}

int main(int argc, char** argv) {
    setBackgroundColor(black);


    // drawObj("obj/head copy.obj", CoordinateType::Cartesian);

    Matrix<float> matrix(
        {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        }
    );

    // todo: implemented some version of rotation, but based on how the image coordinate system
    // works, it's drawing the lower right quarter of a circle. Need to understand what's going on
    // behind the scenes and alter the implementation to deal with it. Refresh on the different coordinate systems and then implement functionality to convert between the two?

    // I think part of what is happening is some lack of clarity regarding what the origin is. Think of the situation where you're trying to rotate a triangle but the triangle is not centered around the origin

    // I mean really what's happening is that it IS rotating about the origin. but the origin here is the upper left corner--it's (0, 0)

    // Point p(100, 100, 0);
    // drawPoint(p, false);
    // for (int i=0; i<360; ++i) {
    //     drawPoint(rotatePointZ(p, i), false);
    // }


    Point a(0.6, 0.5);
    Point b(.8, .5);
    Point c(.6, .25);
    Triangle t(a, b, c, true, green);
    Triangle t2(Point(0.8, 0.2), Point(0.7, .37), Point(0.67, 0.15), true, blue);

    drawCoordinatePlane();

    drawTriangle(t, true, false, false);
    drawTriangle(rotateTriangleZ(t, 45), true, false, false);

    drawTriangle(t2, true, false, false);

    // // todo 90 degrees doesn't work or at least isn't visible:?
    for (int i=1; i<=60; ++i) {
        Triangle tRotated = rotateTriangleZ(t, i);
        tRotated.color = red;
        drawTriangle(tRotated, true, false, false);
    }

    for (int i=1; i<=60; ++i) {
        Triangle tRotated = rotateTriangleZ(t2, i);
        tRotated.color = yellow;
        drawTriangle(tRotated, true, false, false);
    }



	image.write_tga_file("output.tga");
    return 0;
}
