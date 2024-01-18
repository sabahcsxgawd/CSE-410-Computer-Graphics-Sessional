#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle {

private:
    point points[3];
    int rgb[3];

public:
    Triangle(point&, point&, point&);
    Triangle(const Triangle&);
    Triangle& operator=(const Triangle&);

    point getPoint(int);
    int getColor(int);

    point getMinYPoint();
    point getMaxYPoint();
    point getMinXPoint();
    point getMaxXPoint();
};

#endif // TRIANGLE_H