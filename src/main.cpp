
#include <iostream>
#include <vector>
#include <sstream>

#include "Matrix.hpp"
#include "CoordinateSpaces.hpp"
#include "Vec.hpp"
#include "Triangle.hpp"
#include "Canvas.hpp"


int main(int argc, char** argv) {

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

    Matrix<double> scaleMatrix({
        {.5, 0,  0},
        {0,  .5, 0},
        {0,  0,  0}
    });

    Matrix<double> translationMatrix({
        {1, 0, 0, .2},
        {0, 1, 0, 0 },
        {0, 0, 1, 0 },
        {0, 0, 0, 1 }
    });

    Canvas canvas("output.tga", 500, 500, TGAImage::RGB);
    canvas.drawGrid(20);
    Triangle t(0, 0.2, 0.3, -.1, -.3, -.1, purple);
    canvas.drawTriangle(t, CoordinateType::NDC, true, false, false);

    // Triangle t2(t);
    // t2.color = green;
    Vec4 p0(t.p0, 1);
    Vec4 p1(t.p1, 1);
    Vec4 p2(t.p2, 1);
    p0 = translationMatrix * p0;
    p1 = translationMatrix * p1;
    p2 = translationMatrix * p2;
    canvas.drawTriangle(Triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, green), CoordinateType::NDC, true, false, false);


    // WILO: just messing about and seeing how applying transformation matrices works.
    // Adding various vec/matrix utilities as they become needed. Next:
    // * mess with rotations
    // * combine transformation matrices together before applying them to vertices
    // * think about a way/function for applying transformations to triangles as a whole? Or that may be
    //     unnecessary.
    // * pick back up on camera view stuff/transforming model/world space into camera/view space (still don't know the difference between all these things)

    canvas.save();
    

    return 0;
}
