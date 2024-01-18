#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stack>

#include "bitmap_image.hpp"
#include "matrix.h"
#include "point.h"
#include "triangle.h"

using namespace std;

#define EQ(x, y) (fabs((x) - (y)) < __DBL_EPSILON__)

int main(int argc, char** argv)
{
    assert(argc == 3 && argv[1] == string("scene.txt") && argv[2] == string("config.txt"));
    
    ifstream fin(argv[1]);
    ofstream fout1("stage1.txt");
    ofstream fout2("stage2.txt");
    ofstream fout3("stage3.txt");

    matrix M = matrix(4, 4), T = matrix(4, 4), R = matrix(4, 4), P = matrix(4, 4);

    stack<matrix> transformations;
    vector<Triangle> triangles;

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
    assert(l != 0.0);
    lX /= l;
    lY /= l;
    lZ /= l;

    double rX = lY * upZ - lZ * upY;
    double rY = lZ * upX - lX * upZ;
    double rZ = lX * upY - lY * upX;
    double len_r = sqrt(rX * rX + rY * rY + rZ * rZ);
    assert(len_r != 0.0);
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
    // check for n*M_PI/2.0 ???
    double t = near * tan(fovY * M_PI / 360.0);
    double r = near * tan(fovX * M_PI / 360.0);

    P.set(0, 0, near / r);
    P.set(1, 1, near / t);
    P.set(2, 2, -(far + near) / (far - near));
    P.set(2, 3, -(2.0 * far * near) / (far - near));
    P.set(3, 2, -1.0);
    P.set(3, 3, 0.0);

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

            point point0TransformedPoint(point0Transformed.get(0, 0), point0Transformed.get(1, 0), point0Transformed.get(2, 0), point0Transformed.get(3, 0));
            point point1TransformedPoint(point1Transformed.get(0, 0), point1Transformed.get(1, 0), point1Transformed.get(2, 0), point1Transformed.get(3, 0));
            point point2TransformedPoint(point2Transformed.get(0, 0), point2Transformed.get(1, 0), point2Transformed.get(2, 0), point2Transformed.get(3, 0));

            fout1 << point0TransformedPoint << '\n';
            fout1 << point1TransformedPoint << '\n';
            fout1 << point2TransformedPoint << '\n';

            matrix point0Viewed = V * point0Transformed;
            matrix point1Viewed = V * point1Transformed;
            matrix point2Viewed = V * point2Transformed;

            point point0ViewedPoint(point0Viewed.get(0, 0), point0Viewed.get(1, 0), point0Viewed.get(2, 0), point0Viewed.get(3, 0));
            point point1ViewedPoint(point1Viewed.get(0, 0), point1Viewed.get(1, 0), point1Viewed.get(2, 0), point1Viewed.get(3, 0));
            point point2ViewedPoint(point2Viewed.get(0, 0), point2Viewed.get(1, 0), point2Viewed.get(2, 0), point2Viewed.get(3, 0));

            fout2 << point0ViewedPoint << '\n';
            fout2 << point1ViewedPoint << '\n';
            fout2 << point2ViewedPoint << '\n';

            matrix point0Projected = P * point0Viewed;
            matrix point1Projected = P * point1Viewed;
            matrix point2Projected = P * point2Viewed;

            point point0ProjectedPoint(point0Projected.get(0, 0), point0Projected.get(1, 0), point0Projected.get(2, 0), point0Projected.get(3, 0));
            point point1ProjectedPoint(point1Projected.get(0, 0), point1Projected.get(1, 0), point1Projected.get(2, 0), point1Projected.get(3, 0));
            point point2ProjectedPoint(point2Projected.get(0, 0), point2Projected.get(1, 0), point2Projected.get(2, 0), point2Projected.get(3, 0));
            point0ProjectedPoint.divideByW();
            point1ProjectedPoint.divideByW();
            point2ProjectedPoint.divideByW();

            fout3 << point0ProjectedPoint << '\n';
            fout3 << point1ProjectedPoint << '\n';
            fout3 << point2ProjectedPoint << '\n';

            triangles.emplace_back(point0ProjectedPoint, point1ProjectedPoint, point2ProjectedPoint);

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

    fin.open(argv[2]);
    ofstream fout4("z_buffer.txt");

    int screenWidth, screenHeight;
    fin >> screenWidth >> screenHeight;

    double leftLimit = -1.0, rightLimit = 1.0;
    double bottomLimit = -1.0, topLimit = 1.0;
    double zMin = -1.0, zMax = 1.0;

    double dx = (rightLimit - leftLimit) / screenWidth;
    double dy = (topLimit - bottomLimit) / screenHeight;

    double topY = topLimit - dy / 2.0;
    double bottomY = bottomLimit + dy / 2.0;

    double leftX = leftLimit + dx / 2.0;
    double rightX = rightLimit - dx / 2.0;

    double **zBuffer = new double *[screenHeight];
    for (int i = 0; i < screenHeight; i++)
    {
        zBuffer[i] = new double[screenWidth];
        for (int j = 0; j < screenWidth; j++)
        {
            zBuffer[i][j] = zMax;
        }
    }

    bitmap_image image(screenWidth, screenHeight);
    image.set_all_channels(0, 0, 0);

    for (Triangle t : triangles)
    {
        double minY = max(bottomY, t.getMinYPoint().getY());
        double maxY = min(topY, t.getMaxYPoint().getY());

        int topScanLine = ceil((topY - maxY) / dy);
        int bottomScanLine = floor((topY - minY) / dy);

        for (int i = topScanLine; i <= bottomScanLine; i++)
        {
            double y = topY - i * dy;
            double xa, xb, za, zb;

            if (EQ(t.getMinYPoint().getY(), t.getMaxYPoint().getY()))
            {
                xa = t.getMinXPoint().getX();
                za = t.getMinXPoint().getZ();
                xb = t.getMaxXPoint().getX();
                zb = t.getMaxXPoint().getZ();        
            }
            else
            {
                point p0 = t.getPoint(0);
                point p1 = t.getPoint(1);
                point p2 = t.getPoint(2);

                if (min(p0.getY(), p1.getY()) < y && y < max(p0.getY(), p1.getY()))
                {
                    xa = p0.getX() + (p1.getX() - p0.getX()) * (y - p0.getY()) / (p1.getY() - p0.getY());
                    za = p0.getZ() + (p1.getZ() - p0.getZ()) * (y - p0.getY()) / (p1.getY() - p0.getY());
                }
                else if (min(p1.getY(), p2.getY()) < y && y < max(p1.getY(), p2.getY()))
                {
                    xa = p1.getX() + (p2.getX() - p1.getX()) * (y - p1.getY()) / (p2.getY() - p1.getY());
                    za = p1.getZ() + (p2.getZ() - p1.getZ()) * (y - p1.getY()) / (p2.getY() - p1.getY());
                }
                else
                {
                    xa = p0.getX() + (p2.getX() - p0.getX()) * (y - p0.getY()) / (p2.getY() - p0.getY());
                    za = p0.getZ() + (p2.getZ() - p0.getZ()) * (y - p0.getY()) / (p2.getY() - p0.getY());
                }

                if (min(p0.getY(), p2.getY()) < y && y < max(p0.getY(), p2.getY()))
                {
                    xb = p0.getX() + (p2.getX() - p0.getX()) * (y - p0.getY()) / (p2.getY() - p0.getY());
                    zb = p0.getZ() + (p2.getZ() - p0.getZ()) * (y - p0.getY()) / (p2.getY() - p0.getY());
                }
                else if (min(p1.getY(), p2.getY()) < y && y < max(p1.getY(), p2.getY()))
                {
                    xb = p1.getX() + (p2.getX() - p1.getX()) * (y - p1.getY()) / (p2.getY() - p1.getY());
                    zb = p1.getZ() + (p2.getZ() - p1.getZ()) * (y - p1.getY()) / (p2.getY() - p1.getY());
                }
                else
                {
                    xb = p0.getX() + (p1.getX() - p0.getX()) * (y - p0.getY()) / (p1.getY() - p0.getY());
                    zb = p0.getZ() + (p1.getZ() - p0.getZ()) * (y - p0.getY()) / (p1.getY() - p0.getY());
                }

                if (xa > xb)
                {
                    swap(xa, xb);
                    swap(za, zb);
                }

                double minX = max(leftX, xa);
                double maxX = min(rightX, xb);

                int leftIntersectingColumn = round((minX - leftX) / dx);
                int rightIntersectingColumn = round((maxX - leftX) / dx);

                if(EQ(xa, xb) && (leftIntersectingColumn == rightIntersectingColumn))
                {
                    if(za > zb)
                    {
                        swap(za, zb);
                    }

                    int j = leftIntersectingColumn;

                    if(((za < zMin) || EQ(za, zMin)) && zMin < zb) {
                        zBuffer[i][j] = zMin + 2.0 * __DBL_EPSILON__;
                        image.set_pixel(j, i, t.getColor(0), t.getColor(1), t.getColor(2));
                    }

                    else if(zMin < za && za < zBuffer[i][j] && zMin < zb) {
                        zBuffer[i][j] = za;
                        image.set_pixel(j, i, t.getColor(0), t.getColor(1), t.getColor(2));
                    }

                    continue;
                }

                if(EQ(xa, xb) && (leftIntersectingColumn < rightIntersectingColumn)) {
                    continue;
                }

                for (int j = leftIntersectingColumn; j <= rightIntersectingColumn; j++)
                {
                    double x = leftX + j * dx;
                    double z = za + (zb - za) * (x - xa) / (xb - xa);

                    if (zMin < z && z < zBuffer[i][j])
                    {
                        zBuffer[i][j] = z;
                        image.set_pixel(j, i, t.getColor(0), t.getColor(1), t.getColor(2));
                    }
                }        
            }
        }
    }

    image.save_image("out.bmp");

    for (int i = 0; i < screenHeight; i++)
    {
        for (int j = 0; j < screenWidth; j++)
        {
            if (zBuffer[i][j] < zMax)
            {
                fout4 << fixed << setprecision(6) << zBuffer[i][j] << '\t';
            }
        }
        fout4 << '\n';
    }

    for (int i = 0; i < screenHeight; i++)
    {
        delete[] zBuffer[i];
    }

    delete[] zBuffer;

    fin.close();
    fout4.close();

    return 0;
}

/*
TODO: Check the DO's and DON'Ts of the assignment


*/