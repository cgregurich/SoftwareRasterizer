
#include <SDL.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "Matrix.hpp"
#include "CoordinateSpaces.hpp"
#include "Vec.hpp"
#include "Triangle.hpp"
#include "Canvas.hpp"


Triangle transformTriangle(const Triangle& triangle, const Matrix<double>& transformMatrix) {
    Vec4 a(triangle.p0, 1);
    Vec4 b(triangle.p1, 1);
    Vec4 c(triangle.p2, 1);

    a = transformMatrix * a;
    b = transformMatrix * b;
    c = transformMatrix * c;

    Triangle transformedTriangle(a.x, a.y, b.x, b.y, c.x, c.y);

    return transformedTriangle;
}
// todo putting object logic in main to sandbox it; it should end up in its own
// source and header files.
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

// bool parseFaceFromObjLine(const std::string rawLine, std::vector<int> &vertexIndices) {
//     std::stringstream ss(rawLine);
//     std::string keyword;
//     ss >> keyword;
//     if (keyword != "f") return false;

//     /* Parse vertex indices from line */
//     /* Line example:
//     f 24/1/25 25/2/25 26/3/26
//     */
//     int a, b, c;
//     ss >> a >> b >> c;

//     vertexIndices[0] = a;
//     vertexIndices[1] = b;
//     vertexIndices[2] = c;

//     return true;
// }

// todo: implement grabbing textureIndices and normalVertices; it's 25% there but just not worrying about it because I'm not using those things yet; really main thing missing is simply that drawObj() doesn't has std::vectors for texture or normal indices.
bool parseFaceFromObjLine(const std::string rawLine, std::vector<int> &vertexIndices) {
    /*
    Example of rawLine:
    f 1200/1246/1200 1220/1281/1220 1198/1247/1198

    Each set of 3 slash-separated numbers is set of face element
    Within each set, it contains the vertex index, the texture index,
    and the normal index.

    */
    std::istringstream iss(rawLine);
    std::string keyword;
    iss >> keyword;

    if (keyword != "f") return false;

    /* Parse sets of face elements */
    std::vector<std::string> faceStrings;
    std::string token;
    while (std::getline(iss, token, ' ')) {
        // todo clean up the streets of Gotham
        // disgusting code golf way of checking if the given substring is just empty space
        // in order to skip the space between the f and the first set of face elements
        if (std::all_of(token.begin(), token.end(), [](unsigned char c) { return std::isspace(c);})) continue;
        faceStrings.push_back(token);
    }

    /* Parse elements from each face */
    int vertexIndex, textureIndex, normalIndex;
    char slash; /* For consuming slash between face elements */
    for (auto faceString : faceStrings) {
        std::istringstream iss2(faceString);

    
        iss2 >> vertexIndex >> slash >> textureIndex >> slash >> normalIndex;
        vertexIndices.push_back(vertexIndex);
    }

    return true;
}

// todo get rid of these. Only doing this for the moment because drawObj() currently uses WIDTH and HEIGHT
// for converting between coordinate spaces, so just using these macros for that and the canvas size so I can focus on the meat
#define WIDTH 500
#define HEIGHT 500

/*
Todo:
Will likely need to make an Object class, or perhaps a Model class
that contains the data parsed from a .obj file, which can then be passed to Canvas::drawObj() (a method that doesn't currently exist for the Canvas class).
*/
// todo should rename to this because we're not drawing the object but rather creating triangles to return to then have the canvas draw
std::vector<Triangle> drawObj(const std::string filepath ) {
    // Model model(filepath, coordType);
    std::vector<Triangle> triangles;

    ///////////////
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return triangles;
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
    std::cout << "Done parsing vertices" << std::endl << std::endl;


    /* Parse the face elements */
    // todo: can optimize by reading file once
    file.clear(); 
    file.seekg(0);

    std::vector<int> vertexIndices(3);
    int faceCounter = 0; /* For debugging/seeing progress */
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
            vertexIndices.clear(); /* we used what we need so clear it so it can be empty for parsing the next face */

            Vertex v1, v2, v3;
            v1 = vertices[v1Index-1];
            v2 = vertices[v2Index-1];
            v3 = vertices[v3Index-1];

            /* Need to convert to screen coords if the input is NDC */
            // todo fix this. We don't want to depend on WIDTH and HEIGHT since width and height has been moved to the Canvas class
            // if (coordType == CoordinateType::NDC) {
            //     v1 = NDCToNormalizedScreen(v1, WIDTH, HEIGHT);
            //     v2 = NDCToNormalizedScreen(v2, WIDTH, HEIGHT);
            //     v3 = NDCToNormalizedScreen(v3, WIDTH, HEIGHT);
            // }

            Triangle t(v1, v2, v3);
            triangles.push_back(t);
            // drawTriangle(t, CoordinateType::NormalizedScreen, true, true, false);
        }
    }

    file.close();
    return triangles;
}

Triangle rotateTriangleZ(Triangle t, int degrees) {
    float rad = degrees * (M_PI / 180);

    Matrix<double> matrix({
        { cos(rad), -1 * sin(rad), 0, 0 },
        { sin(rad), cos(rad), 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    });

    Vec4 a, b, c;
    a = Vec4(t.p0, 1);
    b = Vec4(t.p1, 1);
    c = Vec4(t.p2, 1);

    a = matrix * a;
    b = matrix * b;
    c = matrix * c;

    Triangle rotated(a.x, a.y, b.x, b.y, c.x, c.y);

    return rotated;
}

Triangle rotateTriangleY(Triangle t, int degrees) {
    float rad = degrees * (M_PI / 180);

    Matrix<double> matrix({
        { cos(rad), 0, sin(rad), 0 },
        { 0, 1, 0, 0 },
        { -sin(rad), 0, cos(rad), 0 },
        { 0, 0, 0, 1 }
    });

    Vec4 a, b, c;
    a = Vec4(t.p0, 1);
    b = Vec4(t.p1, 1);
    c = Vec4(t.p2, 1);

    a = matrix * a;
    b = matrix * b;
    c = matrix * c;

    Triangle rotated(a.x, a.y, b.x, b.y, c.x, c.y);

    return rotated;
}

Triangle rotateTriangleX(Triangle t, int degrees) {
    float rad = degrees * (M_PI / 180);

    Matrix<double> matrix({
        { 1, 0, 0, 0 },
        { 0, cos(rad), -sin(rad), 0 },
        { 0, sin(rad), cos(rad), 0 },
        { 0, 0, 0, 1 }
    });

    Vec4 a, b, c;
    a = Vec4(t.p0, 1);
    b = Vec4(t.p1, 1);
    c = Vec4(t.p2, 1);

    a = matrix * a;
    b = matrix * b;
    c = matrix * c;

    Triangle rotated(a.x, a.y, b.x, b.y, c.x, c.y);

    return rotated;
}

// int main(int argc, char** argv) {
int main(int argc, char* args[]) {

    /* todos 3/16: 
    - Some sort of enum for triangle fill type to replace the three outline + fill + lerp args?
    - windowing library 
    - Test all the stuff after refactoring a bunch! Write some sort of screenshot tests perhaps?? 
    - Refactor by making more CoordinateType conversion functions? Not really needed but could make things slightly cleaner?
    - Split out coordinate logic from Geometry.hpp. Need to figure out the dos and don'ts of hpp vs cpp, etc.

    OKay windowing needs to be next: I want to make this sick ass triangle rotation thing where the fidelity keeps increasing
    and it has this cool animation effect. at least that's the vision.
    */
    
    // Canvas canvas("output.tga", 500, 500, TGAImage::RGB);
    // canvas.drawGrid(20);
    // Triangle t(0, 0.2, 0.3, -.1, -0.3, -.1, orange);
    // canvas.drawTriangle(t, CoordinateType::NDC, true, true, false);

    // canvas.save();

    // Matrix<double> matrix({
    //     {1, 0, 0, 0},
    //     {0, 1, 0, 0},
    //     {0, 0, 1, 0},
    //     {0, 0, 0, 1}
    // });

    // matrix.print();
    // matrix.applyTranslation(42, 0, 0);
    // matrix.print();

    // Matrix<double> trans({
    //     {1, 0, 0, .1},
    //     {0, 1, 0, .1},
    //     {0, 0, 1, 0},
    //     {0, 0, 0, 1}
    // });


    // Matrix<double> scale({
    //     {0.5, 0, 0, 0},
    //     {0, 0.5, 0, 0},
    //     {0, 0, 0.5, 0},
    //     {0, 0, 0, 1}
    // });


    // Matrix<double> result = trans * scale;

    // result.print();

    // Triangle t(0.5, .2, .2, .4, .75, .5, black);
    // Canvas canvas("output.tga", 500, 500, TGAImage::RGB);
    // canvas.fillCanvas(white);
    // canvas.drawTriangle(t, CoordinateType::NormalizedScreen, true, false, false);

    // Triangle tTranslated = transformTriangle(t, trans);
    // tTranslated.color = red;

    // Triangle tScaled = transformTriangle(t, scale);
    // tScaled.color = green;

    // Triangle tScaledTranslated = transformTriangle(transformTriangle(t, scale), trans);
    // tScaledTranslated.color = magenta;

    // Triangle tTranslatedScaled = transformTriangle(transformTriangle(t, trans), scale);
    // tTranslatedScaled.color = yellow;
    
    // canvas.drawTriangle(tScaled, CoordinateType::NormalizedScreen, true, false, false);
    // canvas.drawTriangle(tTranslated, CoordinateType::NormalizedScreen, true, false, false);
    // canvas.drawTriangle(tScaledTranslated, CoordinateType::NormalizedScreen, true, false, false);
    // canvas.drawTriangle(tTranslatedScaled, CoordinateType::NormalizedScreen, true, false, false);

    

    // WILO:  6/11: got proper face parsing implemented so I can draw that head properly and rotate it and shit!! looks sick as hell
    // next step is to figure out how world space and view space etc. work so I can draw other obj files.
    // Then work on windonewTriangle can actually animate shit!!!=very cool.
    // Canvas canvas("output.tga", 1000, 1000, TGAImage::RGB);
    // canvas.fillCanvas(white);
    // std::vector<Triangle> headTriangles = drawObj("obj/head.obj");
    // for (auto triangle : headTriangles) {
    //     Triangle newTriangle = rotateTriangleY(triangle, 45);
    //     // newTriangle = rotateTriangleX(triangle, -45);
    //     canvas.drawTriangle(newTriangle, CoordinateType::NDC, true, false, false);
    // }
    // // drawObj("obj/simple.obj", CoordinateType::NDC);

    // canvas.save();
    


    // SDL_Window* window = SDL_CreateWindow("Software Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Init(SDL_INIT_VIDEO); // todo curious what the other possible args are for this?

    Canvas canvas(500, 500);

    SDL_Event e;
    bool running = true;
    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed;
    canvas.setPixel(200, 200, red);
    canvas.update();
    while (running) {
        auto end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        // if (elapsed % 1  0) {
        //     canvas.update();
        // }
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }
        // todo temp timeout to avoid infinite loop that can't be exited
        if (elapsed.count() > 5) running = false;
    }


    SDL_Quit();

   
    

    return 0;
}
