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

Point NDCToScreen(const Point ndc, int width, int height);
Point NDCToNormalizedScreen(Point ndc, int width, int height);