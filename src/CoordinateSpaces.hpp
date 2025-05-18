#pragma once
#include "Vec.hpp"

/*===========================================
Coordinate Stuff
===========================================*/

/*
NDC:
    x: [-1, 1]
    y: [-1, 1]
    origin: center

Screen:
    x: [0, width]
    y: [0, height]
    origin: upper left

NormalizedScreen:
    x: [0, 1]
    y: [0, 1]
    origin: upper left
*/
enum class CoordinateType {
    NormalizedDeviceCoordinates,
    NDC = NormalizedDeviceCoordinates,
    Screen,
    NormalizedScreen,
};

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