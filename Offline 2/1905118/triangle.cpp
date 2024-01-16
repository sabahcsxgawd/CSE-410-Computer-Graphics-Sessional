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
}

Triangle::Triangle(const Triangle& other) {
    points[0] = other.points[0];
    points[1] = other.points[1];
    points[2] = other.points[2];

    rgb[0] = other.rgb[0];
    rgb[1] = other.rgb[1];
    rgb[2] = other.rgb[2];
}

Triangle& Triangle::operator=(const Triangle& other) {
    points[0] = other.points[0];
    points[1] = other.points[1];
    points[2] = other.points[2];

    rgb[0] = other.rgb[0];
    rgb[1] = other.rgb[1];
    rgb[2] = other.rgb[2];

    return *this;
}

point Triangle::getPoint(int index) {
    assert(index >= 0 && index < 3);
    return points[index];
}

int Triangle::getColor(int index) {
    assert(index >= 0 && index < 3 && rgb[index] >= 0 && rgb[index] < 256);
    return rgb[index];
}

point Triangle::getMinYPoint() {
    point min = points[0];
    for (int i = 1; i < 3; i++) {
        if (points[i].getY() < min.getY()) {
            min = points[i];
        }
    }
    return min;
}

point Triangle::getMaxYPoint() {
    point max = points[0];
    for (int i = 1; i < 3; i++) {
        if (points[i].getY() > max.getY()) {
            max = points[i];
        }
    }
    return max;
}

point Triangle::getMinXPoint() {
    point min = points[0];
    for (int i = 1; i < 3; i++) {
        if (points[i].getX() < min.getX()) {
            min = points[i];
        }
    }
    return min;
}

point Triangle::getMaxXPoint() {
    point max = points[0];
    for (int i = 1; i < 3; i++) {
        if (points[i].getX() > max.getX()) {
            max = points[i];
        }
    }
    return max;
}