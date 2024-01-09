#include <cmath>
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
    ofstream fout1("stage1.txt");
    ofstream fout2("stage2.txt");
    ofstream fout3("stage3.txt");

    matrix M = matrix(4, 4), T = matrix(4, 4), R = matrix(4, 4), P = matrix(4, 4);

    stack<matrix> transformations;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovY, aspectRatio, near, far;

    fin >> eyeX >> eyeY >> eyeZ;
    fin >> lookX >> lookY >> lookZ;
    fin >> upX >> upY >> upZ;
    fin >> fovY >> aspectRatio >> near >> far;

    double lX = lookX - eyeX;
    double lY = lookY - eyeY;
    double lZ = lookZ - eyeZ;
    double l = sqrt(lX * lX + lY * lY + lZ * lZ);
    lX /= l;
    lY /= l;
    lZ /= l;

    double rX = lY * upZ - lZ * upY;
    double rY = lZ * upX - lX * upZ;
    double rZ = lX * upY - lY * upX;
    double len_r = sqrt(rX * rX + rY * rY + rZ * rZ);
    rX /= len_r;
    rY /= len_r;
    rZ /= len_r;

    double uX = rY * lZ - rZ * lY;
    double uY = rZ * lX - rX * lZ;
    double uZ = rX * lY - rY * lX;

    T.set(0, 3, -eyeX);
    T.set(1, 3, -eyeY);
    T.set(2, 3, -eyeZ);

    R.set(0, 0, rX);
    R.set(0, 1, rY);
    R.set(0, 2, rZ);
    R.set(1, 0, uX);
    R.set(1, 1, uY);
    R.set(1, 2, uZ);
    R.set(2, 0, -lX);
    R.set(2, 1, -lY);
    R.set(2, 2, -lZ);

    matrix V = R * T;

    double fovX = fovY * aspectRatio;
    double t = near * tan(fovY * M_PI / 360.0);
    double r = near * tan(fovX * M_PI / 360.0);

    P.set(0, 0, near / r);
    P.set(1, 1, near / t);
    P.set(2, 2, -(far + near) / (far - near));
    P.set(2, 3, -(2.0 * far * near) / (far - near));
    P.set(3, 2, -1.0);

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

            matrix point0Viewed = V * point0Transformed;
            matrix point1Viewed = V * point1Transformed;
            matrix point2Viewed = V * point2Transformed;

            matrix point0Projected = P * point0Viewed;
            matrix point1Projected = P * point1Viewed;
            matrix point2Projected = P * point2Viewed;
            point0Projected.divideByW();
            point1Projected.divideByW();
            point2Projected.divideByW();
                        
            fout1 << fixed << setprecision(7) << point0Transformed.get(0, 0) << " " << point0Transformed.get(1, 0) << " " << point0Transformed.get(2, 0) << " \n";
            fout1 << fixed << setprecision(7) << point1Transformed.get(0, 0) << " " << point1Transformed.get(1, 0) << " " << point1Transformed.get(2, 0) << " \n";
            fout1 << fixed << setprecision(7) << point2Transformed.get(0, 0) << " " << point2Transformed.get(1, 0) << " " << point2Transformed.get(2, 0) << " \n";

            fout2 << fixed << setprecision(7) << point0Viewed.get(0, 0) << " " << point0Viewed.get(1, 0) << " " << point0Viewed.get(2, 0) << " \n";
            fout2 << fixed << setprecision(7) << point1Viewed.get(0, 0) << " " << point1Viewed.get(1, 0) << " " << point1Viewed.get(2, 0) << " \n";
            fout2 << fixed << setprecision(7) << point2Viewed.get(0, 0) << " " << point2Viewed.get(1, 0) << " " << point2Viewed.get(2, 0) << " \n";

            fout3 << fixed << setprecision(7) << point0Projected.get(0, 0) << " " << point0Projected.get(1, 0) << " " << point0Projected.get(2, 0) << " \n";
            fout3 << fixed << setprecision(7) << point1Projected.get(0, 0) << " " << point1Projected.get(1, 0) << " " << point1Projected.get(2, 0) << " \n";
            fout3 << fixed << setprecision(7) << point2Projected.get(0, 0) << " " << point2Projected.get(1, 0) << " " << point2Projected.get(2, 0) << " \n";

            fout1 << '\n';
            fout2 << '\n';
            fout3 << '\n';
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
    fout1.close();
    fout2.close();
    fout3.close();

    return 0;
}