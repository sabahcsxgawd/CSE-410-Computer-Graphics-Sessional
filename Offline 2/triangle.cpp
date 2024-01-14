#include <cassert>
#include <iostream>
#include "triangle.h"

static unsigned long int g_seed = 1;

inline int genRand()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

Triangle::Triangle(point& p0, point& p1, point& p2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;

    rgb[0] = genRand() % 256;
    rgb[1] = genRand() % 256;
    rgb[2] = genRand() % 256;

    if(points[0].getY() < points[1].getY()) {
        std::swap(points[0], points[1]);
    }

    if(points[0].getY() < points[2].getY()) {
        std::swap(points[0], points[2]);
    }

    if(points[1].getY() < points[2].getY()) {
        std::swap(points[1], points[2]);
    }
}

point Triangle::getPoint(int index) {
    assert(index >= 0 && index < 3);
    return points[index];
}

int Triangle::getColor(int index) {
    assert(index >= 0 && index < 3 && rgb[index] >= 0 && rgb[index] < 256);
    return rgb[index];
}