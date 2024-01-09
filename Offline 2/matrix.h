#ifndef MATRIX_H
#define MATRIX_H

class matrix {

private:
    int rows;
    int cols;
    double** data;

public:
    matrix(int rows, int cols);
    matrix(const matrix&);
    matrix& operator=(const matrix&) = delete;
    ~matrix();

    matrix operator*(const matrix& other);
    matrix makeTranslationMatrix(double tx, double ty, double tz);
    matrix makeScalingMatrix(double sx, double sy, double sz);
    // angle must be in radians && axis must be a unit vector
    matrix makeRotationMatrix(double angle, double ax, double ay, double az);

    void print();

};

#endif // MATRIX_H