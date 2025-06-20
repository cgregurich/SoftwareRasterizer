#include "Canvas.hpp"

// todo deprecate due to now using SDL
void Canvas::save() {
    this->image.write_tga_file(this->imageFileName.c_str());
}

void Canvas::update() {
    SDL_UpdateWindowSurface(this->window);
}

void Canvas::setPixel(int x, int y, Color color) {
    SDL_Surface* windowSurface = this->getWindowSurface();
    uint32_t* pixels = (uint32_t*)windowSurface->pixels;
    int pixelIndex = windowSurface->w * y + x;
    pixels[pixelIndex] = color.val;
}

// todo optimization opportunity: take args by reference
void Canvas::drawLine(Point a, Point b, CoordinateType coordType, Color color) {
    // This version does everything in one function
    // instead of using helper functions for high vs low lines

    /* We want the coordinates to be in Screen space for the line drawing logic */
    if (coordType == CoordinateType::NormalizedScreen) {
        a.x *= this->width;
        a.y *= this->height;
        b.x *= this->width;
        b.y *= this->height;
    }
    else if (coordType == CoordinateType::NDC) {
        a = NDCToScreen(a, this->width, this->height);
        b = NDCToScreen(b, this->width, this->height);
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
        this->setPixel(x, y, color);
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
void Canvas::drawLine(Point a, Point b, CoordinateType coordType) {
    drawLine(a, b, coordType, DEFAULT_COLOR);
}

/* Overloaded: four floats instead of two Points */
void Canvas::drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType, Color color) {
    drawLine(Point(x0, y0), Point(x1, y1), coordType, color);
}

/* Overloaded: uses hardcoded color instead of arg */
void Canvas::drawLine(float x0, float y0, float x1, float y1, CoordinateType coordType) {
    drawLine(x0, y0, x1, y1, coordType, DEFAULT_COLOR);
}

void Canvas::fill(Color color) {
    for (int x=0; x<=this->width; x++) {
        for (int y=0; y<=this->height; y++) {
            this->setPixel(x, y, color);
        }
    }
}

void Canvas::fillTriangle(const Triangle t, const CoordinateType coordType, const bool lerp) {

    Triangle tCopy(t);
    
    /* We want to work with screen coordinates for barycentric calculations */
    if (coordType == CoordinateType::NormalizedScreen) {
        tCopy.p0.x *= this->width;
        tCopy.p0.y *= this->height;
        tCopy.p1.x *= this->width;
        tCopy.p1.y *= this->height;
        tCopy.p2.x *= this->width;
        tCopy.p2.y *= this->height;
    }
    else if (coordType == CoordinateType::NDC) {
        tCopy.p0 = NDCToScreen(tCopy.p0, this->width, this->height);
        tCopy.p1 = NDCToScreen(tCopy.p1, this->width, this->height);
        tCopy.p2 = NDCToScreen(tCopy.p2, this->width, this->height);
    }
    for (int x=0; x<this->width; x++) {
        for (int y=0; y<this->height; y++) {
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

                    this->setPixel(p.x, p.y, newColor);
                }
                else { // If lerp is false then just use the triangle's overall color
                    // Blend color with bg (only has an effect if triangle's color has some transparency)
                    // Try blending with background??
                    // todo need to fix this so it works with SDL stuff; haven't messed with alpha values at all yet
                    // nor do I have the functionality to get the color of a given pixel
                    // std::cout << "\n\nWARNING: still need to implement blending with the background color" << std::endl;
                    // Color bg = image.get(p.x, p.y);
                    // float fgWeight = tCopy.color.a / 255.0;
                    // float bgWeight = 1 - fgWeight;
                    // Color newColor = blendColors(tCopy.color, bg, fgWeight, bgWeight);
                    Color newColor = tCopy.color; // todo for now just using the triangles color and not blending anything

                    this->setPixel(p.x, p.y, newColor);
                }
            }

        }
    }
}


void Canvas::drawTriangle(Triangle t, CoordinateType coordType, bool outline, bool fill, bool lerp) {
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
void Canvas::drawRectangle(const int width, const int height, const Point center) {

    Point ul(center.x - (width/2), center.y - (height/2));
    Point ur(center.x + (width/2), center.y - (height/2));
    Point ll(center.x - (width/2), center.y + (height/2));
    Point lr(center.x + (width/2), center.y + (height/2));

    drawLine(ul, ur, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(ur, lr, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(lr, ll, CoordinateType::Screen, DEFAULT_COLOR);
    drawLine(ll, ul, CoordinateType::Screen, DEFAULT_COLOR);
}

void Canvas::drawPoint(Point p, Color color, CoordinateType coordType) {
    int x = 0;
    int y = 0;

    Point screenCoords;

    /* Use x and y in Screen space  */
    switch(coordType) {
        case CoordinateType::NDC:
            {
                Point screenCoords = NDCToScreen(p, this->width, this->height);
                break;
            }
        case CoordinateType::NormalizedScreen:
            screenCoords.x = this->width * p.x;
            screenCoords.y = this->height * p.y;
            break;
        case CoordinateType::Screen:
            screenCoords.x = p.x;
            screenCoords.y = p.y;
            break;
    }
    std::cout << "Drawing point @ (" << x << ", " << y << ")" << std::endl;
    this->setPixel(screenCoords.x, screenCoords.y, color);
}


/*
Very specific sandbox function. Probably not needed
*/
void Canvas::draw3DCube() {
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



/*
Uses a Cartesian convention of:
origin @ 0,0
X and Y both have a range [-1, 1]
*/
void Canvas::drawCoordinatePlane() {
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


void Canvas::drawGrid(int gridResolution) {
    fill(white);
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

        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
    }

    /* Draw negative x grid lines */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;

        tickX0 = 0.5 - tickIncrement * i;
        tickX1 = 0.5 - tickIncrement * i;

        tickY0 = -.01;
        tickY1 = 1.01;

        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
    }

    /* Draw negative y grid lines */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;

        tickX0 = -.01;
        tickX1 = 1.01;

        tickY0 = 0.5 + tickIncrement * i;
        tickY1 = 0.5 + tickIncrement * i;

        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
    }

    /* Draw positive y grid lines */
    for (int i=1; i<=ticks; ++i) {
        float tickX0, tickY0, tickX1, tickY1;

        tickX0 = -.01;
        tickX1 = 1.01;

        tickY0 = 0.5 - tickIncrement * i;
        tickY1 = 0.5 - tickIncrement * i;

        drawLine(tickX0, tickY0, tickX1, tickY1, CoordinateType::NormalizedScreen, tickColor);
    }


    /* Draw x and y axes */
    /* These negative values are deliberately outside of the screen's bounds */
    drawLine(0.5, -0.1, 0.5, 1.1, CoordinateType::NormalizedScreen, yAxisColor);
    drawLine(-0.1, 0.5, 1.1, 0.5, CoordinateType::NormalizedScreen, xAxisColor);
}



/*=========================================================
PRIVATE METHODS
=========================================================*/
SDL_Window* Canvas::createWindow() {
    // 1920 + 800 to put it on the right monitor
    SDL_Window* window = SDL_CreateWindow("Software Rasterizer", 1920+800, SDL_WINDOWPOS_UNDEFINED, this->width, this->height, SDL_WINDOW_SHOWN);
    return window;
}

SDL_Surface* Canvas::getWindowSurface() {
    SDL_Surface* surface = SDL_GetWindowSurface(this->window);
    return surface;
}


/*=========================================================
HELPER/UTILITY FUNCTIONS
=========================================================*/
Point calcBarycentricCoordinates(Point a, Point b, Point c, Point p) {
    float alpha = ((b.y - c.y)*(p.x-c.x) + (c.x - b.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float beta = ((c.y-a.y)*(p.x-c.x) + (a.x-c.x)*(p.y-c.y)) / ((b.y-c.y)*(a.x-c.x) + (c.x-b.x)*(a.y-c.y));
    float gamma = 1 - alpha - beta;
    return Point(alpha, beta, gamma);
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
Does linear interpolation of three colors based on a barycentric coordinate
*/
Color lerpColor(Point barycentric, Color colorA, Color colorB, Color colorC) {
    int newRed = colorA.r * barycentric.x + colorB.r * barycentric.y + colorC.r * barycentric.z;
    int newGreen = colorA.g * barycentric.x + colorB.g * barycentric.y + colorC.g * barycentric.z;
    int newBlue = colorA.b * barycentric.x + colorB.b * barycentric.y + colorC.b * barycentric.z;
    Color newColor(newRed, newGreen, newBlue, 255);
    return newColor;
}

void printColor(const Color c) {
    std::cout << "Color: (" << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << ")\n";
}