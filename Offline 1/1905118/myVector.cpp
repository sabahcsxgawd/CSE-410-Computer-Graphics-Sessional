#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "myVector.h"

myVector::myVector() {
    this->x = this->y = this->z = 0.0f;
}

myVector::myVector(GLfloat x, GLfloat y, GLfloat z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
myVector::myVector(const myVector &other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}

myVector::~myVector() {}

GLfloat myVector::getX() const
{
    return this->x;
}

GLfloat myVector::getY() const
{
    return this->y;
}

GLfloat myVector::getZ() const
{
    return this->z;
}

void myVector::setX(GLfloat x)
{
    this->x = x;
}

void myVector::setY(GLfloat y)
{
    this->y = y;
}

void myVector::setZ(GLfloat z)
{
    this->z = z;
}

GLfloat myVector::getLength() const
{
    return sqrt(x * x + y * y + z * z);
}

myVector myVector::normalize() const
{
    GLfloat len = getLength();
    if (len > 0.0f)
    {
        return myVector(x / len, y / len, z / len);
    }
    else
    {
        return *this; // length 0 means x, y, z all are 0 individually
    }
}

GLfloat myVector::getDotProduct(const myVector &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

myVector myVector::getCrossProduct(const myVector &other) const
{
    return myVector(y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x);
}

myVector myVector::operator+=(const myVector &other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

myVector myVector::operator-=(const myVector &other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;

    return *this;
}

myVector myVector::operator*=(GLfloat scalarConstant)
{
    this->x *= scalarConstant;
    this->y *= scalarConstant;
    this->z *= scalarConstant;

    return *this;
}

myVector myVector::operator/=(GLfloat scalarConstant)
{
    if (scalarConstant != 0.0f)
    {
        this->x /= scalarConstant;
        this->y /= scalarConstant;
        this->z /= scalarConstant;
    }
    else
    {
        std::cout << "Error Division By Zero\n";
    }
    return *this;
}

myVector myVector::operator+(const myVector &other) const
{
    return myVector(this->x + other.x, this->y + other.y, this->z + other.z);
}

myVector myVector::operator-(const myVector &other) const
{
    return myVector(this->x - other.x, this->y - other.y, this->z - other.z);
}

myVector myVector::operator*(GLfloat scalarConstant) const
{
    return myVector(this->x * scalarConstant, this->y * scalarConstant, this->z * scalarConstant);
}

myVector myVector::operator/(GLfloat scalarConstant) const
{
    if (scalarConstant != 0.0f)
    {
        return myVector(this->x / scalarConstant, this->y / scalarConstant, this->z / scalarConstant);
    }
    else
    {
        std::cout << "Error Division by Zero\n";
        return myVector(0, 0, 0);
    }
}

// http://www.songho.ca/opengl/gl_rotate.html
myVector myVector::rotateAroundAxis(GLfloat angleInDegree, const myVector &r)
{
    GLfloat angleInRadian = degreeToRadian(angleInDegree);
    myVector temp = r.getCrossProduct(*this);
    GLfloat c = cos(angleInRadian);
    GLfloat s = sin(angleInRadian);
    return ((r * ((1 - c) * (this->getDotProduct(r)))) + (*this * c) + (temp * s));
}

std::ostream& operator<<(std::ostream& os, const myVector& v) {
    os << "(" << v.getX() << ", " << v.getY() << ", " << v.getZ() << ")";
    return os;
}