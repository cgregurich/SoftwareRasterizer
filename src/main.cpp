
#include "tgaimage.h"
#include <iostream>
#include <cstdlib>
#include <array>
#include <sstream>
#include <vector>
#include <regex>
#include <cmath>

#include "Matrix.hpp"
#include "CoordinateSpaces.hpp"
#include "Vec.hpp"
#include "Triangle.hpp"

Triangle scaleTriangle(const Triangle t, const Vec3 scaleFactor);
Triangle rotateTriangleZ(const Triangle t, const int degrees);

/*-------------------------------------
IMAGE SETUP/CREATION
-------------------------------------*/
#define WIDTH 300
#define HEIGHT 300
#define DEFAULT_COLOR black
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);


// todo optimization opportunity: take args by reference
void drawLine(Point a, Point b, CoordinateType coordType, Color color) {
    // This version does everything in one function
    // instead of using helper functions for high vs low lines

    int width = image.get_width();
    int height = image.get_height();

    /* We want the coordinates to be in Screen space for the line drawing logic */
    if (coordType == CoordinateType::NormalizedScreen) {
        a.x *= width;
        a.y *= height;
        b.x *= width;
        b.y *= height;
    }
    else if (coordType == CoordinateType::NDC) {
        a = NDCToScreen(a, width, height);
        b = NDCToScreen(b, width, height);
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
void drawLine(Point a, Point b, CoordinateType coordType) {
    drawLine(a, b, coordType, DEFAULT_COLOR);
}

/* Overloaded: four floats instead of two Points */
void drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType, Color color) {
    drawLine(Point(x0, y0), Point(x1, y1), coordType, color);
}

/* Overloaded: uses hardcoded color instead of arg */
void drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType) {
    drawLine(x0, y0, x1, y1, coordType, DEFAULT_COLOR);
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
FIXME: it doesn't really set the "background" color, it just
sets the color of the entire image.
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

void fillTriangle(const Triangle t, const CoordinateType coordType, const bool lerp) {

    Triangle tCopy(t);
    
    /* We want to work with screen coordinates for barycentric calculations */
    if (coordType == CoordinateType::NormalizedScreen) {
        tCopy.p0.x *= image.get_width();
        tCopy.p0.y *= image.get_height();
        tCopy.p1.x *= image.get_width();
        tCopy.p1.y *= image.get_height();
        tCopy.p2.x *= image.get_width();
        tCopy.p2.y *= image.get_height();
    }
    else if (coordType == CoordinateType::NDC) {
        tCopy.p0 = NDCToScreen(tCopy.p0, WIDTH, HEIGHT);
        tCopy.p1 = NDCToScreen(tCopy.p1, WIDTH, HEIGHT);
        tCopy.p2 = NDCToScreen(tCopy.p2, WIDTH, HEIGHT);
    }
    for (int x=0; x<WIDTH; x++) {
        for (int y=0; y<HEIGHT; y++) {
            Point p(x, y);

            Point barycentric = calcBarycentricCoordinates(tCopy.p0, tCopy.p1, tCopy.p2, p);

            // p is inside the triangle and therefore should be colored
            if(barycentric.x > 0 && barycentric.y > 0 && barycentric.z > 0) {

                // If lerp is true then use each point's colors to interpolate the final pixel color
                if (lerp) {

                    // Blend A and B
                    Color newColor = blendColors(t.p0Color, t.p1Color, barycentric.x, barycentric.y);

                    // Then blend that color with C
                    newColor = blendColors(newColor, t.p2Color, 1-barycentric.z, barycentric.z);

                    image.set(p.x, p.y, newColor);
                }
                else { // If lerp is false then just use the triangle's overall color
                    // Blend color with bg (only has an effect if triangle's color has some transparency)
                    // Try blending with background??
                    Color bg = image.get(p.x, p.y);
                    float fgWeight = tCopy.color.a / 255.0;
                    float bgWeight = 1 - fgWeight;
                    Color newColor = blendColors(tCopy.color, bg, fgWeight, bgWeight);

                    image.set(p.x, p.y, newColor);
                }
            }

        }
    }
}

void drawTriangle(Triangle t, CoordinateType coordType, bool outline, bool fill, bool lerp) {
    // todo interpolate points that make up lines to decide line colors. Maybe some way to make sort of a general function? Idk.
    if (outline) {
        drawLine(t.p0, t.p1, coordType, t.color);
        drawLine(t.p1, t.p2, coordType, t.color);
        drawLine(t.p2, t.p0, coordType, t.color);
    }
    if (fill) {
        fillTriangle(t, coordType, lerp);
    }
}

/*
Draw a wireframe rectangle. Not particularly robust i.e. doesn't use
normalized coordinates, can't be filled, etc.
*/
void drawRectangle(const int width, const int height, const Point center) {

    Point ul(center.x - (width/2), center.y - (height/2));
    Point ur(center.x + (width/2), center.y - (height/2));
    Point ll(center.x - (width/2), center.y + (height/2));
    Point lr(center.x + (width/2), center.y + (height/2));

    drawLine(ul, ur, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(ur, lr, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(lr, ll, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(ll, ul, CoordinateType::Screen, DEFAULT_COLOR);
}

void drawPoint(Point p, Color color, CoordinateType coordType) {
    int x = 0;
    int y = 0;

    Point screenCoords;

    /* Use x and y in Screen space  */
    switch(coordType) {
        case CoordinateType::NDC:
            {
                Point screenCoords = NDCToScreen(p, WIDTH, HEIGHT);
                break;
            }
        case CoordinateType::NormalizedScreen:
            screenCoords.x = WIDTH * p.x;
            screenCoords.y = HEIGHT * p.y;
            break;
        case CoordinateType::Screen:
            screenCoords.x = p.x;
            screenCoords.y = p.y;
            break;
    }
    std::cout << "Drawing point @ (" << x << ", " << y << ")" << std::endl;
    image.set(screenCoords.x, screenCoords.y, color);
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
    Vec3 v0(.3,  .25,   0);
    Vec3 v1(.5,  .25,   0);
    Vec3 v2(.5,  .425,  0);
    Vec3 v3(.4,  .6,    0);
    Vec3 v4(.2,  .6,    0);
    Vec3 v5(.2,  .4,    0);
    Vec3 v6(.4,  .4,    0);

    Color colorSide(0xca, 0xdb, 0xed, 0xff);
    Color colorFront(0x9a, 0xb9, 0xd5, 0xff);
    Color colorTop(0x86, 0xad, 0xd6, 255);

    Triangle a(v0, v6, v5, colorTop);
    Triangle b(v0, v1, v6, colorTop);
    Triangle c(v1, v2, v6, colorSide);
    Triangle d(v6, v2, v3, colorSide);
    Triangle e(v6, v3, v4, colorFront);
    Triangle f(v5, v6, v4, colorFront);

    std::array<Triangle, 6> triangles = {a, b, c, d, e, f};
    
    for (int i=0; i<triangles.size(); i++) {
        drawTriangle(triangles[i], CoordinateType::NormalizedScreen, true, true, false);
    }
    image.write_tga_file("output.tga");
}

void printColor(const Color c) {
    std::cout << "Color: (" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ")\n";
}

// FIXME
Color blendColorsByAlpha(const Color bg, const Color fg) {

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
Color blendColors(const Color c1, const Color c2) {
    return blendColors(c1, c2, 0.5, 0.5);
}

Color blendColors(const Color c1, const Color c2, const float weight1, const float weight2) {

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

bool parseFaceFromObjLine(const std::string rawLine, std::vector<int> &vertexIndices) {
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


// todo fix after refactoring drawTriangle()
void drawObj(const std::string filepath, const CoordinateType coordType ) {
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

            /* Need to convert to screen coords if the input is NDC */
            if (coordType == CoordinateType::NDC) {
                v1 = NDCToNormalizedScreen(v1, WIDTH, HEIGHT);
                v2 = NDCToNormalizedScreen(v2, WIDTH, HEIGHT);
                v3 = NDCToNormalizedScreen(v3, WIDTH, HEIGHT);
            }

            Triangle t(v1, v2, v3);
            Triangle newT = rotateTriangleZ(t, 90);
            drawTriangle(t, CoordinateType::NormalizedScreen, true, true, false);
            // drawTriangle(newT, CoordinateType::NormalizedScreen, true, true, false);
        }
    }

    file.close();
}


// todo fix parsing obj file (instead of having to manually modify the file to accomodate my flawed face element parsing logic)



#define DEG_TO_RAD(deg) deg * M_PI/180

template <typename T>
Point pointMatrixMultiply(const Point p, const Matrix<T>& m) {
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
    Matrix<double> matrix(
        {
            { cos(rad),     sin(rad),   0 },
            { -sin(rad),    cos(rad),   0 },
            { 0,            0,          1 }
        }
    );
    Point newP = pointMatrixMultiply(p, matrix);
    return newP;
}

Triangle rotateTriangleZ(const Triangle t, const int degrees) {
    Triangle tRotated(t);
    tRotated.p0 = rotatePointZ(t.p0, degrees);
    tRotated.p1 = rotatePointZ(t.p1, degrees);
    tRotated.p2 = rotatePointZ(t.p2, degrees);
    return tRotated;
}

Triangle scaleTriangle(const Triangle t, const Vec3 scaleFactor) {
    Matrix<double> matrix(
        {
            { scaleFactor.x,        0,               0            },
            {       0,        scaleFactor.y,         0            },
            {       0,              0,         scaleFactor.z      }
        }
    );
    Triangle newTriangle(t);
    newTriangle.p0 = pointMatrixMultiply(newTriangle.p0, matrix);
    newTriangle.p1 = pointMatrixMultiply(newTriangle.p1, matrix);
    newTriangle.p2 = pointMatrixMultiply(newTriangle.p2, matrix);

    return newTriangle;
}

/*
Uses a Cartesian convention of:
origin @ 0,0
X and Y both have a range [-1, 1]
*/
void drawCoordinatePlane() {
    Color color = black;
    /* Draw grid */
    /* These negative values are deliberately outside of the screen's bounds */
    drawLine(0.5, -0.1, 0.5, 1.1, CoordinateType::NormalizedScreen, color);
    drawLine(-0.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, color);

    int ticks = 5;
    float tickIncrement = 0.5 / ticks;

    /* Draw positive x ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.5 + tickIncrement * i;
        tickY0 = 0.49;
        tickX1 = 0.5 + tickIncrement * i;
        tickY1 = 0.51;
        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, color);
    }

    /* Draw negative x ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.5 - tickIncrement * i;
        tickY0 = 0.49;
        tickX1 = 0.5 - tickIncrement * i;
        tickY1 = 0.51;
        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, color);
    }

    /* Draw negative y ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.49;
        tickY0 = 0.5 + tickIncrement * i;
        tickX1 = 0.51;
        tickY1 = 0.5 + tickIncrement * i;
        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, color);
    }

    /* Draw positive y ticks */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;
        tickX0 = 0.49;
        tickY0 = 0.5 - tickIncrement * i;
        tickX1 = 0.51;
        tickY1 = 0.5 - tickIncrement * i;

        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, color);
    }
}

void drawGrid(int ticks) {
    /* Draw center lines */
    /* x-axis */
    drawLine(-1.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, red);
    /* y-axis */
    drawLine(0.5, -1.1, 0.5, 1.1, CoordinateType::NormalizedScreen, green);


}

class CoordinatePlane {
    public:
        /* gridResolution is total number of lines in either x and y direction
        (currently only supports equal width and height)
        eg. if you want to be able to map a point at (11, 0) the resolution will
        need to be at least 22 (-11 to the left and +11 to the right) */
        CoordinatePlane(int gridResolution) : gridResolution(gridResolution) {
            if (gridResolution % 2 != 0) {
                std::cerr << "[Warning]: passed `gridResolution` is not even. This may cause unexpected results." << std::endl;
            }
        }
        void drawVector(Vec3 v, Color color) {
            /* Convert grid coordinates to NDC */
            float ndcX = v.x / (gridResolution / 2);
            float ndcY = v.y / (gridResolution / 2);
            // Draw main vector line
            ::drawLine(0, 0, ndcX, ndcY, CoordinateType::NDC, color);
            // Draw arrow head // todo gotta use some math to figure this out hmmmm
        }

        /* todo this is not laid out well and is quite confusing.
        `origin` is used both as a vector and a point. It's passed in to
        drawLine as the starting point for the line to be drawn. But it's also
        used as the "vector" from which to start drawing the new vector
        */
        
        void drawVectorFromPoint(Vec3 v, Vec3 origin) {
            v = v + origin;
            std::cout << "drawVectorFromPoint:\n";
            std::cout << "\tv: " << v << "\n";
            std::cout << "\torigin: " << origin << "\n";
            float ndcX = v.x / (gridResolution / 2);
            float ndcY = v.y / (gridResolution / 2);
            float ndcOriginX = origin.x / (gridResolution / 2);
            float ndcOriginY = origin.y / (gridResolution / 2);

            ::drawLine(ndcOriginX, ndcOriginY, ndcX, ndcY, CoordinateType::NDC, magenta);
        }

        void drawGrid() {
            setBackgroundColor(white);
            /* Colors to use throughout */
            Color tickColor(200, 200, 200, 255); // Color to use for grid lines/ticks
            Color xAxisColor(200, 0, 0, 255);
            Color yAxisColor(0, 200, 0, 255);

            /* Draw grid */

            int ticks = gridResolution / 2;
            float tickIncrement = 0.5 / ticks;

            /* Draw positive x grid lines */
            for (int i=1; i<=ticks; ++i) {
                float tickX0, tickY0, tickX1, tickY1;

                tickX0 = 0.5 + tickIncrement * i;
                tickX1 = 0.5 + tickIncrement * i;

                tickY0 = -.01;
                tickY1 = 1.01;

                ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
            }

            /* Draw negative x grid lines */
            for (int i=1; i<=ticks; ++i) {
                float tickX0, tickY0, tickX1, tickY1;

                tickX0 = 0.5 - tickIncrement * i;
                tickX1 = 0.5 - tickIncrement * i;

                tickY0 = -.01;
                tickY1 = 1.01;

                ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
            }

            /* Draw negative y grid lines */
            for (int i=1; i<=ticks; ++i) {
                float tickX0, tickY0, tickX1, tickY1;

                tickX0 = -.01;
                tickX1 = 1.01;

                tickY0 = 0.5 + tickIncrement * i;
                tickY1 = 0.5 + tickIncrement * i;

                ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
            }

            /* Draw positive y grid lines */
            for (int i=1; i<=ticks; ++i) {
                float tickX0, tickY0, tickX1, tickY1;

                tickX0 = -.01;
                tickX1 = 1.01;

                tickY0 = 0.5 - tickIncrement * i;
                tickY1 = 0.5 - tickIncrement * i;

                ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
            }


            /* Draw x and y axes */
            /* These negative values are deliberately outside of the screen's bounds */
            ::drawLine(0.5, -0.1, 0.5, 1.1, CoordinateType::NormalizedScreen, yAxisColor);
            ::drawLine(-0.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, xAxisColor);
        }
    private:
        int gridResolution;
};

void showMenu() {
    std::cout << "MENU\n";
    std::cout << "\tc - clear\n";
    std::cout << "\td - draw line\n";
    std::cout << "\tm - show menu\n";
    std::cout << "\tq - quit\n";
}
Point getPointFromUser() {
    // todo add loop and error checking
    float x, y;
    std::cout << "Enter x: ";
    std::cin >> x;
    std::cout << "Enter y: ";
    std::cin >> y;
    std::cout << "returning point" << std::endl;
    return Point(x, y);
    
}
void gridCLI(CoordinatePlane grid) {
    char command;
    bool quit = false;
    Point point;
    showMenu();
    while (!quit) {
        image.write_tga_file("output.tga");
        std::cout << "Enter command: ";

        // Grab just the first character and throw away the rest
        std::cin >> command;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(command) {
            case 'q':
                quit = true;
                break;
            case 'd':
                std::cout << "draw todo" << std::endl;
                point = getPointFromUser();
                std::cout << point << std::endl;
                grid.drawVector(point, DEFAULT_COLOR);
                std::cout << "draw vector done" << std::endl;
                break;
            case 'c':
                std::cout << "clear todo" << std::endl;
                break;
            case 'm':
                showMenu(); 
                break;
            default:
                std::cout << "invalid command" << std::endl;
                break;
        }
    }
}

// Triangle translateTriangle(const Triangle t, Vec3 translation) {

// }

int main(int argc, char** argv) {
    setBackgroundColor(white);

    /* todos 3/16: 
    - Some sort of enum for triangle fill type to replace the three outline + fill + lerp args?
    - windowing library 
    - Test all the stuff after refactoring a bunch! Write some sort of screenshot tests perhaps?? 
    - Refactor by making more CoordinateType conversion functions? Not really needed but could make things slightly cleaner?
    - Split out coordinate logic from Geometry.hpp. Need to figure out the dos and don'ts of hpp vs cpp, etc.

    OKay windowing needs to be next: I want to make this sick ass triangle rotation thing where the fidelity keeps increasing
    and it has this cool animation effect. at least that's the vision.
    */

    // drawObj("obj/head copy.obj", CoordinateType::NDC);
    Triangle t(Point(0.5, 0.5), Point(0.25, 0.25), Point(0.75, 0.25));
    Matrix<double> translationMatrix(
        {
            { 1, 0, 0, 0.2 },
            { 0, 1, 0, .5 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        }
    );
    Vec4 a(t.p0);
    Vec4 b(t.p1);
    Vec4 c(t.p2);
    a = translationMatrix * a;
    b = translationMatrix * b;
    c = translationMatrix * c;

    Triangle translatedTriangle(Point(a.x, a.y), Point(b.x, b.y), Point(c.x, c.y), red);
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;
    drawTriangle(t, CoordinateType::NDC, true, false, false);
    drawTriangle(translatedTriangle, CoordinateType::NDC, true, false, false);

    // drawTriangle(t, CoordinateType::NormalizedScreen, true, false, false);
    // Triangle t2 = rotateTriangleZ(t, 30);
    // t2.color = green;
    // drawTriangle(t2, CoordinateType::NormalizedScreen, true, false, false);

    


	image.write_tga_file("output.tga");
    return 0;
}
