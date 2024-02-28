#include <bits/stdc++.h>
#include <GL/glut.h>
#include "bitmap_image.hpp"

using namespace std;

class Vector3D
{
private:
    double x, y, z, w;

public:
    Vector3D()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 1;
    }

    Vector3D(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    Vector3D(double x, double y, double z, double w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    double getX()
    {
        return x;
    }

    double getY()
    {
        return y;
    }

    double getZ()
    {
        return z;
    }

    double getW()
    {
        return w;
    }

    void setX(double x)
    {
        this->x = x;
    }

    void setY(double y)
    {
        this->y = y;
    }

    void setZ(double z)
    {
        this->z = z;
    }

    void setW(double w)
    {
        this->w = w;
    }

    Vector3D operator+(Vector3D &v)
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    Vector3D operator-(Vector3D &v)
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

    double dot(Vector3D &v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(Vector3D &v)
    {
        return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3D normalize()
    {
        double l = length();
        return Vector3D(
            x / l,
            y / l,
            z / l);
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

class Object
{
public:
    Vector3D referencePoint;
    double height, width, length;
    double color[3];
    double coEfficients[4];
    int shine;
    virtual void draw(){};
    void setColor() {}
    void setShine() {}
    void setCoEfficients() {}
};

class PointLight
{
public:
    Vector3D position;
    double color[3];
    PointLight(Vector3D &position, double color[3])
    {
        this->position = position;
        for (int i = 0; i < 3; i++)
        {
            this->color[i] = color[i];
        }
    }
};

class SpotLight
{
public:
    Vector3D position;
    Vector3D direction;
    double angle;
    double color[3];
    SpotLight(Vector3D &position, Vector3D &direction, double angle, double color[3])
    {
        this->position = position;
        this->direction = direction;
        this->angle = angle;
        for (int i = 0; i < 3; i++)
        {
            this->color[i] = color[i];
        }
    }
};

class Sphere : public Object
{
public:
    Sphere(Vector3D &center, double radius)
    {
        this->referencePoint = center;
        this->height = radius;
        this->width = radius;
        this->length = radius;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glPushMatrix();
        glTranslatef(this->referencePoint.getX(), this->referencePoint.getY(), this->referencePoint.getZ());
        glutSolidSphere(this->length, 100, 100);
        glPopMatrix();
    }

    void setColor(double color[3])
    {
        for (int i = 0; i < 3; i++)
        {
            this->color[i] = color[i];
        }
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double coEfficients[4])
    {
        for (int i = 0; i < 4; i++)
        {
            this->coEfficients[i] = coEfficients[i];
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
        glTranslatef(this->referencePoint.getX(), this->referencePoint.getY(), this->referencePoint.getZ());
        int tileCount = round(this->referencePoint.getX() * 2 / this->length);
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

    void setColor(double color[3])
    {
        for (int i = 0; i < 3; i++)
        {
            this->color[i] = color[i];
        }
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double coEfficients[4])
    {
        for (int i = 0; i < 4; i++)
        {
            this->coEfficients[i] = coEfficients[i];
        }
    }
};

class Triangle : public Object
{
public:
    Vector3D a, b, c;
    Triangle(Vector3D &a, Vector3D &b, Vector3D &c)
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw()
    {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(a.getX(), a.getY(), a.getZ());
        glVertex3f(b.getX(), b.getY(), b.getZ());
        glVertex3f(c.getX(), c.getY(), c.getZ());
        glEnd();
    }

    void setColor(double color[3])
    {
        for (int i = 0; i < 3; i++)
        {
            this->color[i] = color[i];
        }
    }

    void setShine(int shine)
    {
        this->shine = shine;
    }

    void setCoEfficients(double coEfficients[4])
    {
        for (int i = 0; i < 4; i++)
        {
            this->coEfficients[i] = coEfficients[i];
        }
    }
};

class Ray
{
public:
    Vector3D start, dir;
    Ray(Vector3D &start, Vector3D &dir)
    {
        this->start = start;
        this->dir = dir;
    }
};