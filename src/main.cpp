
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

    std::vector<int> vertexIndices;
    vertexIndices.reserve(3);
    int faceCounter = 0; /* For debugging/seeing progress */
    while (std::getline(file, line)) {
        if (parseFaceFromObjLine(line, vertexIndices)) {

            /* For debugging/seeing progress */
            faceCounter++;
            if (faceCounter % 100 == 0) std::cout << "faceCounter=" << faceCounter << std::endl;

            int v1Index, v2Index, v3Index;
            v1Index = vertexIndices[0];
            v2Index = vertexIndices[1];
            v3Index = vertexIndices[2];
            vertexIndices.clear(); /* we used what we need so clear it so it can be empty for parsing the next face */

            Vertex v1, v2, v3;
            v1 = vertices[v1Index-1];
            v2 = vertices[v2Index-1];
            v3 = vertices[v3Index-1];



            Triangle t(v1, v2, v3);
            triangles.push_back(t);
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

    Triangle rotated(a.x, a.y, b.x, b.y, c.x, c.y, t.color);

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

    Triangle rotated(a.x, a.y, b.x, b.y, c.x, c.y, t.color);

    return rotated;
}

#include <random>
/*
Used to give lines random colors to better visualize how "3D" objects are rotating
*/
Color randomColor() {
    std::vector<Color> choices = {
        red, green, blue, white
    };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, choices.size());
    return choices[distr(gen)];

}

Vec4 projectVertex(Vec3 v) {
    int width = 500;
    int height = 500;
    float aspect = width / height;
    float fov = 90.0f;
    float near = 0.1f;
    float far = 1000.0f;

    float f = 1.0f / tan(fov * 0.5f * (M_PI / 180.0f));
    float zRange = near - far;

    // todo: chatgpt's projection matrix; don't know enough to know if it's fucky or not
    // Matrix<double> projectionMatrix({
    //     { f/aspect,  0,   0,                     0                       },
    //     { 0,         f,   0,                     0                       },
    //     { 0,         0,   (far + near)/zRange,   2 * far * near / zRange },
    //     { 0,         0,   -1,                    0                       }
    // });
    WILO: okay some fucky shit is going on with this projection matrix. I need to spend a significant time actually understanding
    all of it so that I can debug it and actually understand it. Otherwise I'm just plugging shit in and hoping it will poop out gold. Silly!
    Matrix<double> projectionMatrix({
        { 1/(aspect*tan(fov/2)),   0,              0,                           0                        },
        {0,                        1/tan(fov/2),   0,                           0                        },
        {0,                        0,              -(far+near)/(far-near),      -(2*far*near)/(far-near) },
        {0,                        0,              1,                           0                        }
    });

    Vec4 projected(v, 1.0);

    projected = projectionMatrix * projected;

    projected.x /= projected.w;
    projected.y /= projected.w;
    projected.z /= projected.w;
    projected.w /= projected.w;

    return projected;


}

Triangle projectTriangle(Triangle& t) {
    std::cout << "projectTriangle\n";
    std::cout << "triangle points: \n";
    std::cout << t.p0 << std::endl;
    std::cout << t.p1 << std::endl;
    std::cout << t.p2 << std::endl;
    Vec4 a, b, c;
    a = projectVertex(t.p0);
    b = projectVertex(t.p1);
    c = projectVertex(t.p2);
    std::cout << "projected points: \n";
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;
    std::cout << std::endl;
    
    Triangle projected(a.x, a.y, b.x, b.y, c.x, c.y, t.color);
    return projected;

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

    // todo curious what the other possible args are for this?
    SDL_Init(SDL_INIT_VIDEO); 


    int width = 500;
    int height = 500;
    Canvas canvas(width, height);

    SDL_Event event;
    bool running = true;

    auto start = std::chrono::steady_clock::now();
    int fps = 1000;
    int msBetweenUpdates = 1000 / fps;
    auto nextUpdate = start + std::chrono::milliseconds(msBetweenUpdates);


    const Color BG_COLOR = gray;
    canvas.fill(BG_COLOR);



    int rotationAboutX = 0;
    int rotationAboutY = 0;


    Vec3 v(0.0, 100.0, 0.0);
    Vec4 projected = projectVertex(v);
    std::cout << v << std::endl;
    std::cout << projected << std::endl;



    // std::vector<Triangle> objTriangles = drawObj("obj/sphere.obj");
    // std::vector<Triangle> objTriangles = drawObj("obj/sphere_modelspace.obj");
    // for (auto& triangle : objTriangles) {
    //     triangle.color = randomColor();
    //     std::cout << triangle.p0 << std::endl;
    //     triangle = projectTriangle(triangle);
    //     std::cout << triangle.p0 << std::endl;
    //     canvas.drawTriangle(triangle, CoordinateType::NDC, true, false, false);
    // }

    return 0;
    // std::string line = "f 3657 2197 3";
    // std::vector<int> vertexIndices;
    // vertexIndices.reserve(3);
    // parseFaceFromObjLine(line, vertexIndices);
    // for (auto vertexIndex : vertexIndices) {
    //     std::cout << vertexIndex << std::endl;
    // }


    // canvas.update();
    /*
    really random idea but what would it look like if each triangle of an object
    was rotated out of sync, so like one triangle rotated each frame instead of all per frame?
    */
    running = false;
    while (running) {
        auto now = std::chrono::steady_clock::now();
        // if (now >= nextUpdate) {
        //     canvas.fill(BG_COLOR);
        //     std::cout << "rotationAboutX: " << rotationAboutX << std::endl;

        //     for (auto triangle : objTriangles) {
        //         Triangle rotatedTriangle = rotateTriangleX(triangle, rotationAboutX);
        //         rotatedTriangle = rotateTriangleY(rotatedTriangle, rotationAboutY);
        //         canvas.drawTriangle(rotatedTriangle, CoordinateType::NDC, true, false, false);
        //     }

        //     canvas.update();

        //     nextUpdate += std::chrono::milliseconds(msBetweenUpdates);
        // }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    running = false;
                }
                if (key == SDLK_UP) {
                    rotationAboutX -= 1;
                }
                if (key == SDLK_DOWN) {
                    rotationAboutX += 1;
                }
                // todo rotation about Y axis is not working as expected lol
                if (key == SDLK_LEFT) {
                    rotationAboutY -= 1;
                }
                if (key == SDLK_RIGHT) {
                    rotationAboutY += 1;
                }
            }
            else if(event.type == SDL_MOUSEMOTION) {
                if (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                    std::cout << "mouse" << std::endl;
                    int dx = event.motion.xrel;
                    int dy = event.motion.yrel;
                    int sensitivity = 1;
                    rotationAboutX += dy * sensitivity;
                    rotationAboutY += dx * sensitivity;
                }
            }
                // if (event.type == SDL_MOUSEMOTION) {
                    // int dx = event.motion.xrel;
                    // int dy = event.motion.yrel;
                    // std::cout << "dx: " << dx << std::endl;
                    // int sensitivity = 4;
                    // rotationAboutY += dx * sensitivity;
                    // rotationAboutX += dy * sensitivity;
                // }
        }
    }


    SDL_Quit();


    /*
    TODO:
    - rotationAboutY() doesn't seem to work
    - segmentation fault when rotating object 2 ticks past starting point??
    - makefile error when doing make run?
    - So I imagine the reason why rotating about the Y axis doesn't work is 
        because I haven't actually implemented *anything* to do with the Z value of
        a vertex's position (????????????)


    Q: If I set Z equal to 0 in parseVertexFromObjLine(), then run the code with
        rotateTriangleX, I get the same effect as I had with rotateTriangleY.
        I don't understand why; I didn't think I did jack with the Z coordinate,
        so how in the world does the head model, when rendered, have depth? Or
        at least the *appearance* of depth??
    A: Something to do with how the Z value of a vertex is used to modify the Y value when rotating about the Y axis.
    This was checked by removing the third row of the rotation matrix--we currently get the exact same effect when this
    row is removed. Because the third row is used to calculate the final Z value of the rotated matrix. However, removing
    the Z coordinates from the .obj file DOES have an effect because the Z value is used as part of the calculation for
    the rotated Y coordinate (i.e. second row times the vertex)
    Which means the 3D rotating depth effect isn't due to the actual Z coordinates, but rather because of
    the math behind how the Y value is changed when rotating.

    Q: With the above being said, why does rotating about X not work? What's different there?
    */
    

    return 0;
}
