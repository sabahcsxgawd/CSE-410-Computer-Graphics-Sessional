#include <iostream>
#include <cassert>
#include <cmath>

#include "matrix.h"
#include "point.h"

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

matrix& matrix::operator=(const matrix& other) {
    assert(other.rows > 0 && other.cols > 0);

    if (this == &other) {
        return *this;
    }

    for (int i = 0; i < this->rows; i++) {
        delete[] this->data[i];
    }
    delete[] this->data;

    this->rows = other.rows;
    this->cols = other.cols;
    this->data = new double*[this->rows];

    for (int i = 0; i < this->rows; i++) {
        this->data[i] = new double[this->cols];
        for (int j = 0; j < this->cols; j++) {
            this->data[i][j] = other.data[i][j];
        }
    }

    return *this;
}

matrix::~matrix() {
    for (int i = 0; i < this->rows; i++) {
        delete[] this->data[i];
    }
    delete[] this->data;
}


double matrix::get(int row, int col) {
    assert(row >= 0 && row < this->rows);
    assert(col >= 0 && col < this->cols);

    return this->data[row][col];
}

void matrix::set(int row, int col, double value) {
    assert(row >= 0 && row < this->rows);
    assert(col >= 0 && col < this->cols);

    this->data[row][col] = value;
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

matrix makeTranslationMatrix(double tx, double ty, double tz) {
    matrix result(4, 4);

    result.data[0][3] = tx;
    result.data[1][3] = ty;
    result.data[2][3] = tz;

    return result;
}

matrix makeScalingMatrix(double sx, double sy, double sz) {
    matrix result(4, 4);

    result.data[0][0] = sx;
    result.data[1][1] = sy;
    result.data[2][2] = sz;

    return result;
}

matrix makeRotationMatrix(double angle, double ax, double ay, double az) {
    matrix result(4, 4);

    double c = cos(angle);
    double s = sin(angle);
    double t = 1.0 - c;

    point axis(ax, ay, az, 1.0);
    axis.normalize();

    point I(1.0, 0.0, 0.0, 1.0);
    point J(0.0, 1.0, 0.0, 1.0);
    point K(0.0, 0.0, 1.0, 1.0);

    point c1 = rotateRodrigues(I, axis, angle);
    point c2 = rotateRodrigues(J, axis, angle);
    point c3 = rotateRodrigues(K, axis, angle);

    result.data[0][0] = c1.getX();
    result.data[0][1] = c2.getX();
    result.data[0][2] = c3.getX();
    result.data[1][0] = c1.getY();
    result.data[1][1] = c2.getY();
    result.data[1][2] = c3.getY();
    result.data[2][0] = c1.getZ();
    result.data[2][1] = c2.getZ();
    result.data[2][2] = c3.getZ();

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