#pragma once

#include <bits/stdc++.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"

#define EPSILON 1e-7

extern bitmap_image image;
extern int recursionLevel;

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
        assert(fabs(s) > EPSILON);
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

    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3D &normalize()
    {
        double l = length();
        assert(fabs(l) > EPSILON);
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

    PointLight(const Vector3D &position, const double *color)
    {
        this->position = position;
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
        this->direction.normalize();
        this->cutOffAngle = cutOffAngle;
        copy(color, color + 3, this->color);
    }
};

class Object;
extern vector<Object *> objects;
extern vector<PointLight> pointLights;
extern vector<SpotLight> spotLights;

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
        if (level == 0)
        {
            return tMin;
        }

        Vector3D intersectionPoint = ray.start + ray.dir * tMin;
        double *intersectionPointColor = this->getColor(intersectionPoint);

        color[0] = intersectionPointColor[0] * this->coEfficients[0];
        color[1] = intersectionPointColor[1] * this->coEfficients[0];
        color[2] = intersectionPointColor[2] * this->coEfficients[0];

        for (PointLight pointLight : pointLights)
        {
            Vector3D originL = pointLight.position;
            Vector3D directionL = (intersectionPoint - originL).normalize();
            Ray L(originL, directionL);
            bool isObscured = false;
            double len = (intersectionPoint - originL).length();

            if (len < EPSILON)
            {
                continue;
            }

            for (Object *object : objects)
            {
                double t = object->intersect(L);
                if (t > EPSILON && (t - len) < -EPSILON)
                {
                    isObscured = true;
                    break;
                }
            }
            if (!isObscured)
            {
                Vector3D normal = this->getNormal(L, intersectionPoint);
                Vector3D directionV = (intersectionPoint - ray.start).normalize();
                Ray reflectedRay = this->getReflectedRay(L, intersectionPoint);
                double lambertValue = max(0.0, normal.dot(directionL) * -1.0);
                double phongValue = pow(max(0.0, reflectedRay.dir.dot(directionV) * -1.0), this->shine);

                color[0] += (pointLight.color[0] * lambertValue * this->coEfficients[1]) + (pointLight.color[0] * phongValue * this->coEfficients[2]);
                color[1] += (pointLight.color[1] * lambertValue * this->coEfficients[1]) + (pointLight.color[1] * phongValue * this->coEfficients[2]);
                color[2] += (pointLight.color[2] * lambertValue * this->coEfficients[1]) + (pointLight.color[2] * phongValue * this->coEfficients[2]);
            }
        }

        for (SpotLight spotLight : spotLights)
        {
            Vector3D originL = spotLight.position;
            Vector3D directionL = (intersectionPoint - originL).normalize();
            double cosBeta = directionL.dot(spotLight.direction);
            double beta = acos(cosBeta) * 180.0 / M_PI;
            double Xm = 0;
            double len = (intersectionPoint - originL).length();
            if (beta > spotLight.cutOffAngle || len < EPSILON)
            {
                continue;
            }
            else
            {
                // Xm = pow(cosBeta, 0.01); // TODO test with different power

                double X = cos(spotLight.cutOffAngle * M_PI / 180.0);
                assert(X > 0);
                double d = 1.0 / (1.0 - X);
                if (X <= cosBeta && cosBeta <= 1.0)
                {
                    Xm = 1.0 - (1.0 - cosBeta) * d;
                }
            }
            Ray L(originL, directionL);
            bool isObscured = false;
            for (Object *object : objects)
            {
                double t = object->intersect(L);
                if (t > 0 && (t - len) < -EPSILON)
                {
                    isObscured = true;
                    break;
                }
            }
            if (!isObscured)
            {
                Vector3D normal = this->getNormal(L, intersectionPoint);
                Vector3D directionV = (intersectionPoint - ray.start).normalize();
                Ray reflectedRay = this->getReflectedRay(L, intersectionPoint);
                double lambertValue = max(0.0, normal.dot(directionL) * -1.0);
                double phongValue = pow(max(0.0, reflectedRay.dir.dot(directionV) * -1.0), this->shine);

                color[0] += (spotLight.color[0] * Xm * lambertValue * this->coEfficients[1]) + (spotLight.color[0] * Xm * phongValue * this->coEfficients[2]);
                color[1] += (spotLight.color[1] * Xm * lambertValue * this->coEfficients[1]) + (spotLight.color[1] * Xm * phongValue * this->coEfficients[2]);
                color[2] += (spotLight.color[2] * Xm * lambertValue * this->coEfficients[1]) + (spotLight.color[2] * Xm * phongValue * this->coEfficients[2]);
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
            if (t > 0 && t < tMin)
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

    // add proper destructor
    virtual ~Object() {}
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

    double intersect(Ray &ray) override
    {
        Vector3D q = ray.start - this->referencePoint;
        double b = 2.0 * (q.dot(ray.dir));
        double c = q.dot(q) - this->length * this->length;
        double D = b * b - 4 * c; // as a = 1 due to ray.dir.length() = 1

        double t;

        if (D < 0.0)
        {
            t = -1;
        }

        else if (D > 0.0)
        {
            double t1, t2;
            D = sqrt(D);
            t1 = (-b - D) / 2;
            t2 = (-b + D) / 2;

            if (t1 > 0)
            {
                t = t1;
            }

            else if (t2 > 0)
            {
                t = t2;
            }

            else
            {
                t = -1;
            }
        }

        else
        {
            t = -b / 2;

            if (t < 0)
            {
                t = -1;
            }
        }

        return t;
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

    double getDeterminant(double mat[3][3])
    {
        double det = 0;
        det = mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) - mat[0][1] * (mat[1][0] * mat[2][2] - mat[2][0] * mat[1][2]) + mat[0][2] * (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]);
        return det;
    }

    double intersect(Ray &ray) override
    {
        double matA[3][3] = {
            {a.x - b.x, a.x - c.x, ray.dir.x},
            {a.y - b.y, a.y - c.y, ray.dir.y},
            {a.z - b.z, a.z - c.z, ray.dir.z}};

        double detA = this->getDeterminant(matA);

        assert(detA != 0.0);

        double matBeta[3][3] = {
            {a.x - ray.start.x, a.x - c.x, ray.dir.x},
            {a.y - ray.start.y, a.y - c.y, ray.dir.y},
            {a.z - ray.start.z, a.z - c.z, ray.dir.z}};

        double beta = this->getDeterminant(matBeta) / detA;

        double matGamma[3][3] = {
            {a.x - b.x, a.x - ray.start.x, ray.dir.x},
            {a.y - b.y, a.y - ray.start.y, ray.dir.y},
            {a.z - b.z, a.z - ray.start.z, ray.dir.z}};

        double gamma = this->getDeterminant(matGamma) / detA;

        double matT[3][3] = {
            {a.x - b.x, a.x - c.x, a.x - ray.start.x},
            {a.y - b.y, a.y - c.y, a.y - ray.start.y},
            {a.z - b.z, a.z - c.z, a.z - ray.start.z}};

        double t = this->getDeterminant(matT) / detA;

        if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0)
        {
            return t;
        }
        else
        {
            return -1;
        }
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
    }

    Vector3D getNormal(const Ray &incidentRay, const Vector3D &point)
    {
        double x = point.x;
        double y = point.y;
        double z = point.z;

        double nx = 2 * A * x + D * y + E * z + G;
        double ny = 2 * B * y + D * x + F * z + H;
        double nz = 2 * C * z + E * x + F * y + I;

        return Vector3D(nx, ny, nz).normalize();
    }

    bool isInsideBox(const Vector3D &point)
    {
        if (this->length > EPSILON)
        {
            if (point.x < this->referencePoint.x || point.x > this->referencePoint.x + this->length)
            {
                return false;
            }
        }
        if (this->width > EPSILON)
        {
            if (point.y < this->referencePoint.y || point.y > this->referencePoint.y + this->width)
            {
                return false;
            }
        }
        if (this->height > EPSILON)
        {
            if (point.z < this->referencePoint.z || point.z > this->referencePoint.z + this->height)
            {
                return false;
            }
        }
        return true;
    }

    double intersect(Ray &ray) override
    {
        double x0 = ray.start.x;
        double y0 = ray.start.y;
        double z0 = ray.start.z;

        double xd = ray.dir.x;
        double yd = ray.dir.y;
        double zd = ray.dir.z;

        double a = A * xd * xd + B * yd * yd + C * zd * zd + D * xd * yd + E * xd * zd + F * yd * zd;
        double b = 2 * (A * x0 * xd + B * y0 * yd + C * z0 * zd) + D * (x0 * yd + y0 * xd) + E * (x0 * zd + z0 * xd) + F * (y0 * zd + z0 * yd) + G * xd + H * yd + I * zd;
        double c = A * x0 * x0 + B * y0 * y0 + C * z0 * z0 + D * x0 * y0 + E * x0 * z0 + F * y0 * z0 + G * x0 + H * y0 + I * z0 + J;

        double D = b * b - 4 * a * c;

        Vector3D intersectionPoint;

        if (fabs(a) < EPSILON)
        {
            double t = -c / b;
            if (t > 0)
            {
                intersectionPoint = ray.start + ray.dir * t;
                if (this->isInsideBox(intersectionPoint))
                {
                    return t;
                }
            }
        }

        if (D < 0)
        {
            return -1;
        }

        else if (D > 0.0)
        {
            D = sqrt(D);
            double t1 = (-b - D) / (2 * a);
            double t2 = (-b + D) / (2 * a);

            if (t1 > t2)
            {
                swap(t1, t2);
            }

            if (t1 > 0)
            {
                intersectionPoint = ray.start + ray.dir * t1;
                if (this->isInsideBox(intersectionPoint))
                {
                    return t1;
                }
            }

            if (t2 > 0)
            {
                intersectionPoint = ray.start + ray.dir * t2;
                if (this->isInsideBox(intersectionPoint))
                {
                    return t2;
                }
            }
            return -1;
        }

        else
        {
            double t = -b / (2 * a);
            if (t > 0)
            {
                intersectionPoint = ray.start + ray.dir * t;
                if (this->isInsideBox(intersectionPoint))
                {
                    return t;
                }
            }
            return -1;
        }
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

    double intersect(Ray &ray) override
    {
        assert(ray.dir.length() != 0);

        if (fabs(ray.dir.z) < EPSILON)
        {
            // The ray is parallel to the floor, no intersection
            return -1;
        }

        Vector3D normal = this->getNormal(ray, this->referencePoint);
        double t = -(normal.dot(ray.start)) / (normal.dot(ray.dir));
        if (t < 0.0)
        {
            t = -1;
        }
        else
        {
            // Compute intersection point
            Vector3D intersectionPoint = ray.start + ray.dir * t;

            // Check if the intersection point is within the floor boundaries
            int tileCount = round(-this->referencePoint.x * 2 / this->length);
            double minX = this->referencePoint.x;
            double minY = this->referencePoint.y;
            double maxX = minX + tileCount * this->length;
            double maxY = minY + tileCount * this->length;

            if (intersectionPoint.x >= minX && intersectionPoint.x <= maxX &&
                intersectionPoint.y >= minY && intersectionPoint.y <= maxY)
            {
                return t;
            }
            else
            {
                t = -1;
            }
        }
        return t;
    }
};
