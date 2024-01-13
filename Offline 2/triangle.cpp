#include <cassert>
#include <iostream>
#include "triangle.h"

static unsigned long int g_seed = 1;

inline int genRand()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

Triangle::Triangle(point p1, point p2, point p3) {
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;

    color[0] = genRand() % 256;
    color[1] = genRand() % 256;
    color[2] = genRand() % 256;
}

Triangle::Triangle(const Triangle& other) {
    points[0] = other.points[0];
    points[1] = other.points[1];
    points[2] = other.points[2];
}

Triangle& Triangle::operator=(const Triangle& other) {
    points[0] = other.points[0];
    points[1] = other.points[1];
    points[2] = other.points[2];
    return *this;
}

point Triangle::getPoint(int index) {
    assert(index >= 0 && index < 3);
    return points[index];
}

double Triangle::getColor(int index) {
    assert(index >= 0 && index < 3 && color[index] >= 0.0 && color[index] < 256.0);
    return color[index];
}