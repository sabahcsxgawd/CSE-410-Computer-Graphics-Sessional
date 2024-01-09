#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stack>

#include "matrix.h"

using namespace std;

int main()
{

    ifstream fin("scene.txt");
    ofstream fout("stage1.txt");

    matrix M = matrix(4, 4);

    stack<matrix> transformations;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    fin >> eyeX >> eyeY >> eyeZ;
    fin >> lookX >> lookY >> lookZ;
    fin >> upX >> upY >> upZ;
    fin >> fovY >> aspectRatio >> near >> far;

    string line;

    while (true)
    {
        fin >> line;
        if (line == "triangle")
        {
            double x0, y0, z0;
            double x1, y1, z1;
            double x2, y2, z2;
            fin >> x0 >> y0 >> z0;
            fin >> x1 >> y1 >> z1;
            fin >> x2 >> y2 >> z2;

            matrix point0(4, 1);
            point0.set(0, 0, x0);
            point0.set(1, 0, y0);
            point0.set(2, 0, z0);
            point0.set(3, 0, 1.0);

            matrix point1(4, 1);
            point1.set(0, 0, x1);
            point1.set(1, 0, y1);
            point1.set(2, 0, z1);
            point1.set(3, 0, 1.0);

            matrix point2(4, 1);
            point2.set(0, 0, x2);
            point2.set(1, 0, y2);
            point2.set(2, 0, z2);
            point2.set(3, 0, 1.0);

            matrix point0Transformed = M * point0;
            matrix point1Transformed = M * point1;
            matrix point2Transformed = M * point2;
                        
            fout << fixed << setprecision(7) << point0Transformed.get(0, 0) << " " << point0Transformed.get(1, 0) << " " << point0Transformed.get(2, 0) << " \n";
            fout << fixed << setprecision(7) << point1Transformed.get(0, 0) << " " << point1Transformed.get(1, 0) << " " << point1Transformed.get(2, 0) << " \n";
            fout << fixed << setprecision(7) << point2Transformed.get(0, 0) << " " << point2Transformed.get(1, 0) << " " << point2Transformed.get(2, 0) << " \n";
            fout << '\n';
        }
        if (line == "translate")
        {
            double tx, ty, tz;
            fin >> tx >> ty >> tz;
            M = M * makeTranslationMatrix(tx, ty, tz);
        }
        if (line == "scale")
        {
            double sx, sy, sz;
            fin >> sx >> sy >> sz;
            M = M * makeScalingMatrix(sx, sy, sz);
        }
        if (line == "rotate")
        {
            double angle, ax, ay, az;
            fin >> angle >> ax >> ay >> az;
            M = M * makeRotationMatrix(angle, ax, ay, az);
        }
        if (line == "push")
        {
            transformations.push(M);
        }
        if (line == "pop")
        {
            M = transformations.top();
            transformations.pop();
        }
        if (line == "end")
        {
            break;
        }
    }

    fin.close();
    fout.close();
    return 0;
}