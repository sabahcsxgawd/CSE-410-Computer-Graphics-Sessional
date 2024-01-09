#include "matrix.h"
#include <iostream>
#include <cassert>
#include <cmath>

matrix::matrix(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    this->data = new double*[rows];

    for (int i = 0; i < rows; i++) {
        this->data[i] = new double[cols];
        for (int j = 0; j < cols; j++) {
            this->data[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

matrix::~matrix() {
    for (int i = 0; i < this->rows; i++) {
        delete[] this->data[i];
    }
    delete[] this->data;
}

matrix::matrix(const matrix& other) {
    assert(other.rows > 0 && other.cols > 0);

    this->rows = other.rows;
    this->cols = other.cols;
    this->data = new double*[this->rows];

    for (int i = 0; i < this->rows; i++) {
        this->data[i] = new double[this->cols];
        for (int j = 0; j < this->cols; j++) {
            this->data[i][j] = other.data[i][j];
        }
    }
}

matrix matrix::operator*(const matrix& other) {
    assert(this->cols == other.rows);

    matrix result(this->rows, other.cols);

    for (int i = 0; i < this->rows; i++) {
        for (int j = 0; j < other.cols; j++) {
            result.data[i][j] = 0.0;
            for (int k = 0; k < this->cols; k++) {
                result.data[i][j] += this->data[i][k] * other.data[k][j];
            }
        }
    }

    return result;
}

matrix matrix::makeTranslationMatrix(double tx, double ty, double tz) {
    matrix result(4, 4);

    result.data[0][3] = tx;
    result.data[1][3] = ty;
    result.data[2][3] = tz;

    return result;
}

matrix matrix::makeScalingMatrix(double sx, double sy, double sz) {
    matrix result(4, 4);

    result.data[0][0] = sx;
    result.data[1][1] = sy;
    result.data[2][2] = sz;

    return result;
}

matrix matrix::makeRotationMatrix(double angle, double ax, double ay, double az) {
    matrix result(4, 4);

    double c = cos(angle);
    double s = sin(angle);
    double t = 1.0 - c;

    double len = sqrt(ax * ax + ay * ay + az * az);
    ax /= len;
    ay /= len;
    az /= len;

    result.data[0][0] = t * ax * ax + c;
    result.data[0][1] = t * ax * ay + s * az;
    result.data[0][2] = t * ax * az - s * ay;
    result.data[1][0] = t * ax * ay - s * az;
    result.data[1][1] = t * ay * ay + c;
    result.data[1][2] = t * ay * az + s * ax;
    result.data[2][0] = t * ax * az + s * ay;
    result.data[2][1] = t * ay * az - s * ax;
    result.data[2][2] = t * az * az + c;

    return result;
}

void matrix::print() {
    for (int i = 0; i < this->rows; i++) {
        for (int j = 0; j < this->cols; j++) {
            std::cout << this->data[i][j] << " ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}