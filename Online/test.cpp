#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

struct Color
{
    double r, g, b;
};

struct Point
{
    double x, y;
};

unsigned long frameCount = 0;

// B1
// double radius0 = 0.5;
// double radius1 = 0.25;
// double radius2 = 0.1;

// Point center0 = {0, 0};
// Point center1, center2;

// Color red = {1, 0, 0};
// Color blue = {0, 0, 1};
// Color yellow = {1, 1, 0};
// Color white = {1, 1, 1};

// A1
double radius = 0.4;

Point center = {-0.3, 0};
Point radiusEnd, lineEnd = {0.4, 0};

Color blue = {0, 0, 1};
Color white = {1, 1, 1};

void drawLine(Point &p1, Point &p2, struct Color &color)
{
    glPushMatrix();

    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(p1.x, p1.y);
    glVertex2f(p2.x, p2.y);
    glEnd();

    glPopMatrix();
}

void drawCircle(struct Point &center, double radius, struct Color &color)
{
    int i;
    int pointCount = 100;
    GLfloat twicePi = 2.0f * M_PI;

    glPushMatrix();

    glBegin(GL_LINE_LOOP);
    glColor3f(color.r, color.g, color.b);
    for (i = 0; i <= pointCount; i++)
    {
        glVertex2f(
            center.x + (radius * cos(i * twicePi / pointCount)),
            center.y + (radius * sin(i * twicePi / pointCount)));
    }
    glEnd();

    glPopMatrix();
}

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
}

void displayHandlerB1()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    radiusEnd.x = center.x + cos(frameCount * 0.25 * 2.0 * M_PI / 60) * radius;
    lineEnd.y = radiusEnd.y = center.y + sin(frameCount * 0.25 * 2.0 * M_PI / 60) * radius;

    drawCircle(center, radius, blue);
    drawLine(center, radiusEnd, white);
    drawLine(radiusEnd, lineEnd, white);

    glutSwapBuffers();
    glutPostRedisplay();

    frameCount++;
}

void displayHandlerA1() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   

    glutSwapBuffers();
    glutPostRedisplay();

    frameCount++;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("1905118 Test");
    glutDisplayFunc(displayHandlerB1);
    initGL();
    glutMainLoop();
    return 0;
}