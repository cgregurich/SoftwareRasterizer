#include "CoordinateSpaces.hpp"

/*
Converts a Point with x and y values in range [-1, 1] to a Point
with x and y values in the range of [0, width] and [0, height] respectively.
*/
Point NDCToScreen(const Point ndc, int width, int height) {
    float pixelX = ndc.x * width/2 + width/2;
    float pixelY = -(ndc.y * height/2) + height/2;
    return Point(pixelX, pixelY);
}


Point NDCToNormalizedScreen(Point ndc, int width, int height) {
    float screenX = ndc.x * width/2 + width/2;
    float normScreenX = screenX / width;
    float screenY = -(ndc.y * height/2) + height/2;
    float normScreenY = screenY / height;

    return Point(normScreenX, normScreenY);
}