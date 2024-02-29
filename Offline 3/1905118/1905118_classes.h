#pragma once

#include <bits/stdc++.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"

using namespace std;

class Vector3D
{
public:
    double x, y, z;

    Vector3D()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3D(const Vector3D &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3D(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3D &operator=(const Vector3D &v)
    {
        if (this != &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return *this;
    }

    Vector3D operator+(const Vector3D &v)
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    Vector3D operator-(const Vector3D &v)
    {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    Vector3D operator*(double s)
    {
        return Vector3D(x * s, y * s, z * s);
    }

    Vector3D operator/(double s)
    {
        assert(s != 0.0);
        return Vector3D(x / s, y / s, z / s);
    }

    double dot(const Vector3D &v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(const Vector3D &v)
    {
        return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    double length() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3D &normalize()
    {
        double l = length();
        assert(l != 0.0);
        x /= l;
        y /= l;
        z /= l;
        return *this;
    }

    Vector3D rotateAroundAxis(double angle, Vector3D &axis)
    {
        double s = sin(angle);
        double c = cos(angle);
        double x = this->x;
        double y = this->y;
        double z = this->z;
        double u = axis.x;
        double v = axis.y;
        double w = axis.z;
        double xPrime = (u * (u * x + v * y + w * z) * (1 - c) + x * c + (-w * y + v * z) * s);
        double yPrime = (v * (u * x + v * y + w * z) * (1 - c) + y * c + (w * x - u * z) * s);
        double zPrime = (w * (u * x + v * y + w * z) * (1 - c) + z * c + (-v * x + u * y) * s);
        return Vector3D(xPrime, yPrime, zPrime);
    }
};

class Ray
{
public:
    Vector3D start, dir;
    Ray(Vector3D &start, Vector3D &dir)
    {
        this->start = start;
        this->dir = dir.normalize();
    }
};

class Color
{
public:
    double r, g, b;
    Color()
    {
        r = 0;
        g = 0;
        b = 0;
    }
    Color(double r, double g, double b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color operator*(double s)
    {
        return Color(r * s, g * s, b * s);
    }
    Color operator+(Color &c)
    {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

class PointLight
{
public:
    Vector3D position;
    double color[3];

    // Use const reference for Vector3D
    PointLight(const Vector3D &position, const double *color)
        : position(position)
    {
        // Copy color array using std::copy for better safety
        copy(color, color + 3, this->color);
    }
};

class SpotLight
{
public:
    Vector3D position;
    Vector3D direction;
    double cutOffAngle;
    double color[3];
    SpotLight(const Vector3D &position, const Vector3D &direction, double cutOffAngle, double *color)
    {
        this->position = position;
        this->direction = direction;
        this->cutOffAngle = cutOffAngle;
        copy(color, color + 3, this->color);
    }
};

class Object
{
public:
    Vector3D referencePoint;
    int shine;
    double height, width, length;
    double color[3];
    double coEfficients[4];

    virtual void draw() = 0;
    virtual void setColor(double *color) = 0;
    virtual void setShine(int shine) = 0;
    virtual void setCoEfficients(double *coEfficients) = 0;
    virtual double intersect(Ray &ray, double *color, int level = 0) = 0;
    virtual Vector3D getNormal(Vector3D &point) = 0;
    virtual Color getColor(Vector3D &point) = 0;
};

class Sphere : public Object
{
public:
    Sphere(const Vector3D &center, double radius)
    {
        this->referencePoint = center;
        this->height = radius;
        this->width = radius;
        this->length = radius;
    }

    void draw() override
    {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();
        glTranslatef(this->referencePoint.x, this->referencePoint.y, this->referencePoint.z);
        glutSolidSphere(this->length, 100, 100);
        glPopMatrix();
    }

    void setColor(double *color)
    {
        copy(color, color + 3, this->color);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double *coEfficients)
    {
        copy(coEfficients, coEfficients + 4, this->coEfficients);
    }

    double intersect(Ray &ray, double *color, int level = 0)
    {
        Vector3D l = this->referencePoint - ray.start;
        double tca = l.dot(ray.dir);
        if (tca < 0)
            return -1;
        double d2 = l.dot(l) - tca * tca;
        double radius2 = this->length * this->length;
        if (d2 > radius2)
            return -1;
        double thc = sqrt(radius2 - d2);
        double t0 = tca - thc;
        double t1 = tca + thc;
        if (t0 > t1)
            swap(t0, t1);
        if (t0 < 0)
        {
            t0 = t1;
            if (t0 < 0)
                return -1;
        }
        color[0] = this->color[0];
        color[1] = this->color[1];
        color[2] = this->color[2];
        return t0;
    }

    Vector3D getNormal(Vector3D &point)
    {
        return (point - this->referencePoint).normalize();
    }

    Color getColor(Vector3D &point)
    {
        return Color(this->color[0], this->color[1], this->color[2]);
    }
};

class Floor : public Object
{
public:
    Floor(double floorWidth, double tileWidth)
    {
        this->referencePoint = Vector3D(-floorWidth / 2, -floorWidth / 2, 0);
        this->length = tileWidth;
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(this->referencePoint.x, this->referencePoint.y, this->referencePoint.z);
        int tileCount = round(this->referencePoint.x * 2 / this->length);
        for (int i = 0; i < tileCount; i++)
        {
            for (int j = 0; j < tileCount; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3f(1, 1, 1);
                }
                else
                {
                    glColor3f(0, 0, 0);
                }
                glBegin(GL_QUADS);
                glVertex3f(i * this->length, j * this->length, 0);
                glVertex3f(i * this->length + this->length, j * this->length, 0);
                glVertex3f(i * this->length + this->length, j * this->length + this->length, 0);
                glVertex3f(i * this->length, j * this->length + this->length, 0);
                glEnd();
            }
        }
        glPopMatrix();
    }

    void setColor(double *color)
    {
        copy(color, color + 3, this->color);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double *coEfficients)
    {
        copy(coEfficients, coEfficients + 4, this->coEfficients);
    }

    double intersect(Ray &ray, double *color, int level = 0)
    {
        double t = -ray.start.z / ray.dir.z;
        if (t > 0)
        {
            color[0] = this->color[0];
            color[1] = this->color[1];
            color[2] = this->color[2];
            return t;
        }
        return -1;
    }

    Vector3D getNormal(Vector3D &point)
    {
        return Vector3D(0, 0, 1);
    }

    Color getColor(Vector3D &point)
    {
        int tileX = (point.x - this->referencePoint.x) / this->length;
        int tileY = (point.y - this->referencePoint.y) / this->length;
        if ((tileX + tileY) % 2 == 0)
        {
            return Color(1, 1, 1);
        }
        else
        {
            return Color(0, 0, 0);
        }
    }
};

class Triangle : public Object
{
private:
    Vector3D a, b, c;

public:
    Triangle(const Vector3D &a, const Vector3D &b, const Vector3D &c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
    }

    void setColor(double *color)
    {
        copy(color, color + 3, this->color);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double *coEfficients)
    {
        copy(coEfficients, coEfficients + 4, this->coEfficients);
    }

    double intersect(Ray &ray, double *color, int level = 0)
    {
        Vector3D e1 = b - a;
        Vector3D e2 = c - a;
        Vector3D h = ray.dir.cross(e2);
        double a = e1.dot(h);
        if (a > -0.00001 && a < 0.00001)
            return -1;
        double f = 1 / a;
        Vector3D s = ray.start - this->a;
        double u = f * s.dot(h);
        if (u < 0.0 || u > 1.0)
            return -1;
        Vector3D q = s.cross(e1);
        double v = f * ray.dir.dot(q);
        if (v < 0.0 || u + v > 1.0)
            return -1;
        double t = f * e2.dot(q);
        if (t > 0.00001)
        {
            color[0] = this->color[0];
            color[1] = this->color[1];
            color[2] = this->color[2];
            return t;
        }
        return -1;
    }

    Vector3D getNormal(Vector3D &point)
    {
        return ((b - a).cross(c - a)).normalize();
    }

    Color getColor(Vector3D &point)
    {
        return Color(this->color[0], this->color[1], this->color[2]);
    }
};

class General : public Object
{
private:
    double A, B, C, D, E, F, G, H, I, J;

public:
    General(double A, double B, double C, double D, double E, double F, double G, double H, double I, double J,
            const Vector3D &referencePoint, double length, double width, double height)
    {
        this->A = A;
        this->B = B;
        this->C = C;
        this->D = D;
        this->E = E;
        this->F = F;
        this->G = G;
        this->H = H;
        this->I = I;
        this->J = J;

        this->referencePoint = referencePoint;

        this->length = length;
        this->width = width;
        this->height = height;
    }

    void draw()
    {
        // TODO
    }

    void setColor(double *color)
    {
        copy(color, color + 3, this->color);
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double *coEfficients)
    {
        copy(coEfficients, coEfficients + 4, this->coEfficients);
    }

    double intersect(Ray &ray, double *color, int level = 0)
    {
        double a = A * ray.dir.x * ray.dir.x + B * ray.dir.y * ray.dir.y + C * ray.dir.z * ray.dir.z + D * ray.dir.x * ray.dir.y + E * ray.dir.y * ray.dir.z + F * ray.dir.z * ray.dir.x;
        double b = 2 * (A * ray.start.x * ray.dir.x + B * ray.start.y * ray.dir.y + C * ray.start.z * ray.dir.z) + D * (ray.start.x * ray.dir.y + ray.start.y * ray.dir.x) + E * (ray.start.y * ray.dir.z + ray.start.z * ray.dir.y) + F * (ray.start.z * ray.dir.x + ray.start.x * ray.dir.z) + G * ray.dir.x + H * ray.dir.y + I * ray.dir.z;
        double c = A * ray.start.x * ray.start.x + B * ray.start.y * ray.start.y + C * ray.start.z * ray.start.z + D * ray.start.x * ray.start.y + E * ray.start.y * ray.start.z + F * ray.start.z * ray.start.x + G * ray.start.x + H * ray.start.y + I * ray.start.z + J;
        double d = b * b - 4 * a * c;
        if (d < 0)
            return -1;
        double t1 = (-b + sqrt(d)) / (2 * a);
        double t2 = (-b - sqrt(d)) / (2 * a);
        if (t1 < 0 && t2 < 0)
            return -1;
        if (t1 < 0)
        {
            color[0] = this->color[0];
            color[1] = this->color[1];
            color[2] = this->color[2];
            return t2;
        }
        if (t2 < 0)
        {
            color[0] = this->color[0];
            color[1] = this->color[1];
            color[2] = this->color[2];
            return t1;
        }
        if (t1 < t2)
        {
            color[0] = this->color[0];
            color[1] = this->color[1];
            color[2] = this->color[2];
            return t1;
        }
        color[0] = this->color[0];
        color[1] = this->color[1];
        color[2] = this->color[2];
        return t2;
    }

    Vector3D getNormal(Vector3D &point)
    {
        double x = 2 * A * point.x + D * point.y + F * point.z + G;
        double y = 2 * B * point.y + D * point.x + E * point.z + H;
        double z = 2 * C * point.z + E * point.y + F * point.x + I;
        return Vector3D(x, y, z).normalize();
    }

    Color getColor(Vector3D &point)
    {
        return Color(this->color[0], this->color[1], this->color[2]);
    }
};