#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle {

private:
    point points[3];
    double color[3];

public:
    Triangle(point, point, point);
    Triangle(const Triangle&);
    Triangle& operator=(const Triangle&);

    point getPoint(int);
    double getColor(int);
};

#endif // TRIANGLE_H