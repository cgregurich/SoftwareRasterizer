
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
#include "Canvas.hpp"


/*-------------------------------------
IMAGE SETUP/CREATION
-------------------------------------*/
#define WIDTH 100
#define HEIGHT 100
#define DEFAULT_COLOR black
TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);


/*-------------------------------------
FUNCTION DEFINITIONS
-------------------------------------*/




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
// void drawObj(const std::string filepath, const CoordinateType coordType ) {
//     // Model model(filepath, coordType);

//     ///////////////
//     std::ifstream file(filepath);
//     if (!file) {
//         std::cerr << "Error opening file" << std::endl;
//         return;
//     }

//     std::string line; // Line read from file
//     std::vector<Vertex> vertices; // Vertices parsed from file
//     Vertex vertex;

//     /* Parse all the vertex position lines and turn them into a vector of vertices
//     which will be used to index into based off the faces */
//     while (std::getline(file, line)) {
//         if (parseVertexFromObjLine(line, vertex)) {
//             vertices.push_back(vertex);
//         }
//     }
//     std::cout << "Done parsing vertices" << std::endl;


//     /* Parse the face elements */
//     // todo: can optimize by reading file once
//     file.clear(); 
//     file.seekg(0);
//     std::vector<int> vertexIndices(3);
//     int faceCounter = 0;
//     while (std::getline(file, line)) {
//         if (parseFaceFromObjLine(line, vertexIndices)) {
//             // todo this is just for debugging/seeing progress
//             faceCounter++;
//             if (faceCounter % 100 == 0) std::cout << "faceCounter=" << faceCounter << std::endl;
//             // todo end

//             int v1Index, v2Index, v3Index;
//             v1Index = vertexIndices[0];
//             v2Index = vertexIndices[1];
//             v3Index = vertexIndices[2];

//             Vertex v1, v2, v3;
//             v1 = vertices[v1Index-1];
//             v2 = vertices[v2Index-1];
//             v3 = vertices[v3Index-1];

//             /* Need to convert to screen coords if the input is NDC */
//             if (coordType == CoordinateType::NDC) {
//                 v1 = NDCToNormalizedScreen(v1, WIDTH, HEIGHT);
//                 v2 = NDCToNormalizedScreen(v2, WIDTH, HEIGHT);
//                 v3 = NDCToNormalizedScreen(v3, WIDTH, HEIGHT);
//             }

//             Triangle t(v1, v2, v3);
//             drawTriangle(t, CoordinateType::NormalizedScreen, true, true, false);
//         }
//     }

//     file.close();
// }


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


// void drawGrid(int ticks) {
//     /* Draw center lines */
//     /* x-axis */
//     drawLine(-1.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, red);
//     /* y-axis */
//     drawLine(0.5, -1.1, 0.5, 1.1, CoordinateType::NormalizedScreen, green);
// }

// class CoordinatePlane {
//     public:
//         /* gridResolution is total number of lines in either x and y direction
//         (currently only supports equal width and height)
//         eg. if you want to be able to map a point at (11, 0) the resolution will
//         need to be at least 22 (-11 to the left and +11 to the right) */
//         CoordinatePlane(int gridResolution) : gridResolution(gridResolution) {
//             if (gridResolution % 2 != 0) {
//                 std::cerr << "[Warning]: passed `gridResolution` is not even. This may cause unexpected results." << std::endl;
//             }
//         }
//         void drawVector(Vec3 v, Color color) {
//             /* Convert grid coordinates to NDC */
//             float ndcX = v.x / (gridResolution / 2);
//             float ndcY = v.y / (gridResolution / 2);
//             // Draw main vector line
//             ::drawLine(0, 0, ndcX, ndcY, CoordinateType::NDC, color);
//             // Draw arrow head // todo gotta use some math to figure this out hmmmm
//         }

//         /* todo this is not laid out well and is quite confusing.
//         `origin` is used both as a vector and a point. It's passed in to
//         drawLine as the starting point for the line to be drawn. But it's also
//         used as the "vector" from which to start drawing the new vector
//         */
        
//         void drawVectorFromPoint(Vec3 v, Vec3 origin) {
//             v = v + origin;
//             std::cout << "drawVectorFromPoint:\n";
//             std::cout << "\tv: " << v << "\n";
//             std::cout << "\torigin: " << origin << "\n";
//             float ndcX = v.x / (gridResolution / 2);
//             float ndcY = v.y / (gridResolution / 2);
//             float ndcOriginX = origin.x / (gridResolution / 2);
//             float ndcOriginY = origin.y / (gridResolution / 2);

//             ::drawLine(ndcOriginX, ndcOriginY, ndcX, ndcY, CoordinateType::NDC, magenta);
//         }

//         void drawGrid() {
//             setBackgroundColor(white);
//             /* Colors to use throughout */
//             Color tickColor(200, 200, 200, 255); // Color to use for grid lines/ticks
//             Color xAxisColor(200, 0, 0, 255);
//             Color yAxisColor(0, 200, 0, 255);

//             /* Draw grid */

//             int ticks = gridResolution / 2;
//             float tickIncrement = 0.5 / ticks;

//             /* Draw positive x grid lines */
//             for (int i=1; i<=ticks; ++i) {
//                 float tickX0, tickY0, tickX1, tickY1;

//                 tickX0 = 0.5 + tickIncrement * i;
//                 tickX1 = 0.5 + tickIncrement * i;

//                 tickY0 = -.01;
//                 tickY1 = 1.01;

//                 ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
//             }

//             /* Draw negative x grid lines */
//             for (int i=1; i<=ticks; ++i) {
//                 float tickX0, tickY0, tickX1, tickY1;

//                 tickX0 = 0.5 - tickIncrement * i;
//                 tickX1 = 0.5 - tickIncrement * i;

//                 tickY0 = -.01;
//                 tickY1 = 1.01;

//                 ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
//             }

//             /* Draw negative y grid lines */
//             for (int i=1; i<=ticks; ++i) {
//                 float tickX0, tickY0, tickX1, tickY1;

//                 tickX0 = -.01;
//                 tickX1 = 1.01;

//                 tickY0 = 0.5 + tickIncrement * i;
//                 tickY1 = 0.5 + tickIncrement * i;

//                 ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
//             }

//             /* Draw positive y grid lines */
//             for (int i=1; i<=ticks; ++i) {
//                 float tickX0, tickY0, tickX1, tickY1;

//                 tickX0 = -.01;
//                 tickX1 = 1.01;

//                 tickY0 = 0.5 - tickIncrement * i;
//                 tickY1 = 0.5 - tickIncrement * i;

//                 ::drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
//             }


//             /* Draw x and y axes */
//             /* These negative values are deliberately outside of the screen's bounds */
//             ::drawLine(0.5, -0.1, 0.5, 1.1, CoordinateType::NormalizedScreen, yAxisColor);
//             ::drawLine(-0.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, xAxisColor);
//         }
//     private:
//         int gridResolution;
// };

// void showMenu() {
//     std::cout << "MENU\n";
//     std::cout << "\tc - clear\n";
//     std::cout << "\td - draw line\n";
//     std::cout << "\tm - show menu\n";
//     std::cout << "\tq - quit\n";
// }
// Point getPointFromUser() {
//     // todo add loop and error checking
//     float x, y;
//     std::cout << "Enter x: ";
//     std::cin >> x;
//     std::cout << "Enter y: ";
//     std::cin >> y;
//     std::cout << "returning point" << std::endl;
//     return Point(x, y);
    
// }
// void gridCLI(CoordinatePlane grid) {
//     char command;
//     bool quit = false;
//     Point point;
//     showMenu();
//     while (!quit) {
//         image.write_tga_file("output.tga");
//         std::cout << "Enter command: ";

//         // Grab just the first character and throw away the rest
//         std::cin >> command;
//         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

//         switch(command) {
//             case 'q':
//                 quit = true;
//                 break;
//             case 'd':
//                 std::cout << "draw todo" << std::endl;
//                 point = getPointFromUser();
//                 std::cout << point << std::endl;
//                 grid.drawVector(point, DEFAULT_COLOR);
//                 std::cout << "draw vector done" << std::endl;
//                 break;
//             case 'c':
//                 std::cout << "clear todo" << std::endl;
//                 break;
//             case 'm':
//                 showMenu(); 
//                 break;
//             default:
//                 std::cout << "invalid command" << std::endl;
//                 break;
//         }
//     }
// }

int main(int argc, char** argv) {
    // setBackgroundColor(white);

    /* todos 3/16: 
    - Some sort of enum for triangle fill type to replace the three outline + fill + lerp args?
    - windowing library 
    - Test all the stuff after refactoring a bunch! Write some sort of screenshot tests perhaps?? 
    - Refactor by making more CoordinateType conversion functions? Not really needed but could make things slightly cleaner?
    - Split out coordinate logic from Geometry.hpp. Need to figure out the dos and don'ts of hpp vs cpp, etc.

    OKay windowing needs to be next: I want to make this sick ass triangle rotation thing where the fidelity keeps increasing
    and it has this cool animation effect. at least that's the vision.
    */
    
    Canvas canvas("output.tga", 500, 500, TGAImage::RGB);
    canvas.setBackgroundColor(white);

    canvas.save();
    return 0;
}
