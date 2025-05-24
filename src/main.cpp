
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

    return 0;
}
