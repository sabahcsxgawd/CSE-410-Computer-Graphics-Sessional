#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class Triangle {

private:
    point points[3];
    int rgb[3];

public:
    Triangle(point&, point&, point&);

    point getPoint(int);
    int getColor(int);
};

#endif // TRIANGLE_H