#ifndef MATRIX_H
#define MATRIX_H

class matrix {

private:
    int rows;
    int cols;    

public:
    double** data;
    matrix(int rows, int cols);
    matrix(const matrix&);
    matrix& operator=(const matrix&);
    ~matrix();

    double get(int row, int col);
    void set(int row, int col, double value);

    matrix operator*(const matrix& other);

    void print();

};


matrix makeTranslationMatrix(double tx, double ty, double tz);
matrix makeScalingMatrix(double sx, double sy, double sz);
matrix makeRotationMatrix(double angle, double ax, double ay, double az);

#endif // MATRIX_H