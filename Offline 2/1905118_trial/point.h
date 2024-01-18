#ifndef POINT_H
#define POINT_H

class point {

private:
    double x, y, z, w;

public:
    point();
    point(double x, double y, double z, double w);
    point(const point& other);
    point& operator=(const point& other);

    double getX();
    double getY();
    double getZ();
    double getW();

    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setW(double w);

    void divideByW();
    void normalize();

    // add point [vector]
    point operator+(const point& other);

    // scalar multiplication
    point operator*(double scalar);

    // dot product
    double operator*(const point& other);

    // cross product
    point operator^(const point& other);

    friend std::ostream& operator<<(std::ostream&, const point&);

};

point rotateRodrigues(point &, point &, double);

#endif // POINT_H