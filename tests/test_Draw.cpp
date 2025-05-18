
#include <iostream>
#include <vector>

// #include "../src/Geometry.hpp"


/*
Draw 8 lines in a 100x100 image and compare
against a saved baseline image
*/
void test_drawLine() {
    // class TestCase {
    //     public:
    //         /* FUT args */
    //         Point point1;
    //         Point point2;

    //         TestCase(Point point1, Point point2) : point1(point1), point2(point2) {}

    // };

    // std::vector<TestCase> testCases = {
    // };

    // int fails = 0;
    // int passes = 0;

    // std::cout << "\nTesting normCartesianToPixel:\n";

    drawLine(Point(.75, .75), Point(.25, .25), CoordinateType::NDC);
    drawLine(Point(.5, .3), Point(.1, .95), CoordinateType::NDC);
    drawLine(Point(-.42, .4), Point(-.1, .3), CoordinateType::NDC);
    drawLine(Point(-.78, .3), Point(-.67, .4), CoordinateType::NDC);
    drawLine(Point(-.5, -.34), Point(-.03, -.7), CoordinateType::NDC);
    drawLine(Point(-.1, -.34), Point(-.73, -.7), CoordinateType::NDC);
    drawLine(Point(.43, -.1), Point(.87, -.7), CoordinateType::NDC);
    drawLine(Point(.63, -.1), Point(.27, -.7), CoordinateType::NDC);


}

int main() {
    test_drawLine();

}