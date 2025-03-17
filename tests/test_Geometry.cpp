#include <iostream>
#include <vector>

#include "../src/Geometry.hpp"


void test_normCartesianToPixel() {
    class TestCase {
        public:
            /* FUT args */
            Point cart;
            int width;
            int height;

            /* Expected result to compare actual against */
            Point expected;

            TestCase(Point cart, int width, int height, Point expected) : cart(cart), width(width), height(height), expected(expected) {}
    };

    std::vector<TestCase> testCases = {
        /* Non-negative x values */
        TestCase( Point(0, 0),      400, 400,   Point(200, 200) ),
        TestCase( Point(0.25, 0),   400, 400,   Point(250, 200) ),
        TestCase( Point(0.5, 0),    400, 400,   Point(300, 200) ),
        TestCase( Point(0.75, 0),   400, 400,   Point(350, 200) ),
        TestCase( Point(1.0, 0),    400, 400,   Point(400, 200) ),

        /* Negative x values */
        TestCase( Point(-0.25, 0),   400, 400,   Point(150, 200) ),
        TestCase( Point(-0.5, 0),    400, 400,   Point(100, 200) ),
        TestCase( Point(-0.75, 0),   400, 400,   Point(50, 200) ),
        TestCase( Point(-1.0, 0),    400, 400,   Point(0, 200) ),

        /* Non-negative y values */
        TestCase( Point(0, 0),      400, 400,   Point(200, 200) ),
        TestCase( Point(0, 0.25),   400, 400,   Point(200, 150) ),
        TestCase( Point(0, 0.5),    400, 400,   Point(200, 100) ),
        TestCase( Point(0, 0.75),   400, 400,   Point(200, 50) ),
        TestCase( Point(0, 1.0),    400, 400,   Point(200, 0) ),

        /* Negative x values */
        TestCase( Point(0, -0.25),   400, 400,   Point(200, 250) ),
        TestCase( Point(0, -0.5),    400, 400,   Point(200, 300) ),
        TestCase( Point(0, -0.75),   400, 400,   Point(200, 350) ),
        TestCase( Point(0, -1.0),    400, 400,   Point(200, 400) ),

        /* Misc */
        TestCase( Point(0.5, 0.25),    400, 400,   Point(300, 150) ),
    };

    int fails = 0;
    int passes = 0;

    std::cout << "\nTesting normCartesianToPixel:\n";

    for (auto& tc : testCases ) {
        Point actual = normCartesianToPixel(tc.cart, tc.width, tc.height);

        if (actual.x == tc.expected.x && actual.y == tc.expected.y) {
            std::cout << ".";
            passes++;
        }
        else {
            std::cout << "!";
            fails++;
        }
    }
    std::cout << "\n";

    std::cout << passes << "/" << testCases.size() << " passed" << std::endl;

}

void test_normCartToScreen() {
    class TestCase {
        public:
            /* FUT args */
            Point cart;
            int width;
            int height;

            /* Expected result to compare actual against */
            Point expected;

            TestCase(Point cart, int width, int height, Point expected) : cart(cart), width(width), height(height), expected(expected) {}
    };

    std::vector<TestCase> testCases = {
        /* Non-negative x values */
        TestCase( Point(0, 0),      400, 400,   Point(0.5, 0.5) ),
        TestCase( Point(0.25, 0),   400, 400,   Point(0.625, 0.5) ),
        TestCase( Point(0.5, 0),    400, 400,   Point(0.75, 0.5) ),
        TestCase( Point(0.75, 0),   400, 400,   Point(0.875, 0.5) ),
        TestCase( Point(1.0, 0),    400, 400,   Point(1.0, 0.5) ),

        /* Negative x values */
        TestCase( Point(-0.25, 0),   400, 400,   Point(0.375, 0.5) ),
        TestCase( Point(-0.5, 0),    400, 400,   Point(0.25, 0.5) ),
        TestCase( Point(-0.75, 0),   400, 400,   Point(0.125, 0.5) ),
        TestCase( Point(-1.0, 0),    400, 400,   Point(0, 0.5) ),

        /* Non-negative y values */
        TestCase( Point(0, 0),      400, 400,   Point(0.5, 0.5) ),
        TestCase( Point(0, 0.25),   400, 400,   Point(0.5, .375) ),
        TestCase( Point(0, 0.5),    400, 400,   Point(0.5, .25) ),
        TestCase( Point(0, 0.75),   400, 400,   Point(0.5, .125) ),
        TestCase( Point(0, 1.0),    400, 400,   Point(0.5, 0) ),

        /* Negative x values */
        TestCase( Point(0, -0.25),   400, 400,   Point(0.5, 0.625) ),
        TestCase( Point(0, -0.5),    400, 400,   Point(0.5, .75) ),
        TestCase( Point(0, -0.75),   400, 400,   Point(0.5, .875) ),
        TestCase( Point(0, -1.0),    400, 400,   Point(0.5, 1.0) ),

        /* Misc */
        TestCase( Point(0.5, 0.25),    400, 400,   Point(0.75, .375) ),
    };

    int fails = 0;
    int passes = 0;

    std::cout << "\nTesting normCartToScreen:\n";

    for (auto& tc : testCases ) {
        Point actual = normCartToScreen(tc.cart, tc.width, tc.height);

        if (actual.x == tc.expected.x && actual.y == tc.expected.y) {
            std::cout << ".";
            passes++;
        }
        else {
            std::cout << "!";
            fails++;
        }
    }
    std::cout << "\n";

    std::cout << passes << "/" << testCases.size() << " passed" << std::endl;

}

int main() {
    test_normCartesianToPixel();
    test_normCartToScreen();

}