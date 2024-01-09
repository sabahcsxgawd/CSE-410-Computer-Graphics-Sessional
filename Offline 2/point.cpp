#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "point.h"

point::point() {
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->w = 1.0;
}

point::point(double x, double y, double z, double w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

double point::getX() {
    return this->x;
}

double point::getY() {
    return this->y;
}

double point::getZ() {
    return this->z;
}

double point::getW() {
    return this->w;
}

void point::setX(double x) {
    this->x = x;
}

void point::setY(double y) {
    this->y = y;
}

void point::setZ(double z) {
    this->z = z;
}

void point::setW(double w) {
    this->w = w;
}

void point::divideByW() {
    assert(this->w != 0.0);

    this->x /= this->w;
    this->y /= this->w;
    this->z /= this->w;
    this->w /= this->w;
}

void point::normalize() {
    double len = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    assert(len != 0.0 && this->w == 1.0);
    this->x /= len;
    this->y /= len;
    this->z /= len;
}

point point::operator+(const point& other) {
    double x = this->x + other.x;
    double y = this->y + other.y;
    double z = this->z + other.z;
    double w = 1.0;

    return point(x, y, z, w);
}

point point::operator*(double scalar) {
    double x = this->x * scalar;
    double y = this->y * scalar;
    double z = this->z * scalar;
    double w = 1.0;

    return point(x, y, z, w);
}

double point::operator*(const point& other) {
    return this->x * other.x + this->y * other.y + this->z * other.z;
}

point point::operator^(const point& other) {
    double x = this->y * other.z - this->z * other.y;
    double y = this->z * other.x - this->x * other.z;
    double z = this->x * other.y - this->y * other.x;
    double w = 1.0;

    return point(x, y, z, w);
}

void point::print(std::ofstream& fout, int precision) {
    fout << std::fixed << std::setprecision(precision) << this->x << " " << this->y << " " << this->z << " \n";
}

point rotateRodrigues(point &x, point &axis, double angle) {
    angle *= M_PI / 180.0;
    double c = cos(angle);
    double s = sin(angle);
    double t = 1.0 - c;

    return (x * c) + (axis * (t * (axis * x))) + ((axis ^ x) * s);
}