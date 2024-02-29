#pragma once

#include <bits/stdc++.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"

#define EPSILON std::numeric_limits<double>::epsilon()

extern bitmap_image image;
extern int recursionLevel;
extern vector<Object *> objects;
extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;

double whiteColor[] = {1, 1, 1};
double blackColor[] = {0, 0, 0};

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

    Vector3D operator+(double s)
    {
        return Vector3D(x + s, y + s, z + s);
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
    Ray(const Vector3D &start, const Vector3D &dir)
    {
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
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

    virtual void setShine(int shine)
    {
        this->shine = shine;
    };

    virtual void setColor(double *color)
    {
        copy(color, color + 3, this->color);
    };

    virtual void setCoEfficients(double *coEfficients)
    {
        copy(coEfficients, coEfficients + 4, this->coEfficients);
    };

    virtual double *getColor(Vector3D &point)
    {
        return this->color;
    }

    virtual Ray getReflectedRay(const Ray &incidentRay, const Vector3D &intersectionPoint)
    {
        Vector3D normal = this->getNormal(incidentRay, intersectionPoint);
        Vector3D reflectedDir = (normal * (2 * normal.dot(incidentRay.dir))) * -1.0 + incidentRay.dir;
        Vector3D reflectedStart = intersectionPoint;
        reflectedStart = reflectedStart + reflectedDir * EPSILON * 2.0;
        return Ray(reflectedStart, reflectedDir);
    }

    virtual double intersect(Ray &ray, double *color, int level)
    {
        double tMin = this->intersect(ray);
        cout << "tMin: " << tMin << '\n';
        if (level == 0 || tMin < 0.0)
        {
            return tMin;
        }

        Vector3D intersectionPoint = ray.start + ray.dir * tMin;
        double *intersectionPointColor = this->getColor(intersectionPoint);
        cout << "Intersection Point Color: " << intersectionPointColor[0] << " " << intersectionPointColor[1] << " " << intersectionPointColor[2] << '\n';

        color[0] = intersectionPointColor[0] * this->coEfficients[0];
        color[1] = intersectionPointColor[1] * this->coEfficients[0];
        color[2] = intersectionPointColor[2] * this->coEfficients[0];

        for (PointLight *pointLight : pointLights)
        {
            Vector3D originL = pointLight->position;
            Vector3D directionL = (intersectionPoint - originL).normalize();
            Ray L(originL, directionL);
            bool isObscured = false;
            for (Object *object : objects)
            {
                double t = object->intersect(L);
                if (t >= 0 && t <= 1.0)
                {
                    isObscured = true;
                    break;
                }
            }
            if (!isObscured)
            {
                Vector3D normal = this->getNormal(L, intersectionPoint);
                Vector3D directionV = (ray.start - intersectionPoint).normalize();
                Ray reflectedRay = this->getReflectedRay(L, intersectionPoint);
                double lambertValue = max(0.0, normal.dot(directionL) * -1.0);
                double phongValue = pow(max(0.0, reflectedRay.dir.dot(directionV) * -1.0), this->shine);

                color[0] += (pointLight->color[0] * lambertValue * this->coEfficients[1]) + (pointLight->color[0] * phongValue * this->coEfficients[2]);
                color[1] += (pointLight->color[1] * lambertValue * this->coEfficients[1]) + (pointLight->color[1] * phongValue * this->coEfficients[2]);
                color[2] += (pointLight->color[2] * lambertValue * this->coEfficients[1]) + (pointLight->color[2] * phongValue * this->coEfficients[2]);
            }
        }

        for (SpotLight *spotLight : spotLights)
        {
            Vector3D originL = spotLight->position;
            Vector3D directionL = (intersectionPoint - originL).normalize();
            double cosBeta = directionL.dot(spotLight->direction);
            double beta = acos(cosBeta) * 180.0 / M_PI;
            if (beta > spotLight->cutOffAngle)
            {
                continue;
            }
            else
            {
                cosBeta = pow(cosBeta, 0.1); // TODO test with different power
                spotLight->color[0] *= cosBeta;
                spotLight->color[1] *= cosBeta;
                spotLight->color[2] *= cosBeta;
            }
            Ray L(originL, directionL);
            bool isObscured = false;
            for (Object *object : objects)
            {
                double t = object->intersect(L);
                if (t >= 0 && t <= 1.0)
                {
                    isObscured = true;
                    break;
                }
            }
            if (!isObscured)
            {
                Vector3D normal = this->getNormal(L, intersectionPoint);
                Vector3D directionV = (ray.start - intersectionPoint).normalize();
                Ray reflectedRay = this->getReflectedRay(L, intersectionPoint);
                double lambertValue = max(0.0, normal.dot(directionL) * -1.0);
                double phongValue = pow(max(0.0, reflectedRay.dir.dot(directionV) * -1.0), this->shine);

                color[0] += (spotLight->color[0] * lambertValue * this->coEfficients[1]) + (spotLight->color[0] * phongValue * this->coEfficients[2]);
                color[1] += (spotLight->color[1] * lambertValue * this->coEfficients[1]) + (spotLight->color[1] * phongValue * this->coEfficients[2]);
                color[2] += (spotLight->color[2] * lambertValue * this->coEfficients[1]) + (spotLight->color[2] * phongValue * this->coEfficients[2]);
            }
        }

        if (level >= recursionLevel)
        {
            return tMin;
        }

        Ray reflectedRay = this->getReflectedRay(ray, intersectionPoint);

        tMin = DBL_MAX;
        Object *nearestObject = nullptr;

        for (Object *object : objects)
        {
            double t = object->intersect(reflectedRay);
            if (t >= 0 && t < tMin)
            {
                tMin = t;
                nearestObject = object;
            }
        }

        if (nearestObject != nullptr)
        {
            double *colorReflected = new double[3];
            tMin = nearestObject->intersect(reflectedRay, colorReflected, level + 1);
            color[0] += colorReflected[0] * this->coEfficients[3];
            color[1] += colorReflected[1] * this->coEfficients[3];
            color[2] += colorReflected[2] * this->coEfficients[3];
            delete[] colorReflected;
        }

        return tMin;
    }

    virtual double intersect(Ray &ray) = 0;
    virtual void draw() = 0;
    virtual Vector3D getNormal(const Ray &incidentRay, const Vector3D &point) = 0;
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

    Vector3D getNormal(const Ray &incidentRay, const Vector3D &point)
    {
        return ((this->referencePoint * -1.0) + point).normalize();
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

    Vector3D getNormal(const Ray &incidentRay, const Vector3D &point)
    {
        Vector3D normal = ((b - a).cross(c - a)).normalize();
        if (normal.dot(incidentRay.dir) > 0.0)
        {
            normal = normal * -1.0;
        }
        return normal;
    }
};

class General : public Object
{
private:
    double A, B, C, D, E, F, G, H, I, J;

public:
    // Equation: F(x,y,z) = Ax2+By2+Cz2+Dxy+Exz+Fyz+Gx+Hy+Iz+J = 0
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

    Vector3D getNormal(const Ray &incidentRay, const Vector3D &point) override
    {
        double x = 2 * A * point.x + D * point.y + E * point.z + G;
        double y = 2 * B * point.y + D * point.x + F * point.z + H;
        double z = 2 * C * point.z + E * point.x + F * point.y + I;
        return Vector3D(x, y, z).normalize();
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
        int tileCount = round(-this->referencePoint.x * 2 / this->length);
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

    Vector3D getNormal(const Ray &incidentRay, const Vector3D &point)
    {
        if (incidentRay.dir.z < 0.0)
        {
            return Vector3D(0, 0, 1);
        }
        return Vector3D(0, 0, -1);
    }

    double *getColor(Vector3D &point) override
    {
        int tileX = (point.x - this->referencePoint.x) / this->length;
        int tileY = (point.y - this->referencePoint.y) / this->length;
        if ((tileX + tileY) % 2 == 0)
        {
            return whiteColor;
        }
        else
        {
            return blackColor;
        }
    }
};
