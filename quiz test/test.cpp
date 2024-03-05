#include <bits/stdc++.h>
#include <GL/glut.h>

using namespace std;

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1, 0.01, 1000);
}

void drawSquare()
{
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glEnd();
    glPopMatrix();
}

void drawFace1()
{
    glColor3f(1, 0, 0);
    glPushMatrix();
    drawSquare();
    glPopMatrix();
}

void drawFace2()
{
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(0, 0, 1);
    drawSquare();
    glPopMatrix();
}

void drawFace3()
{
    glColor3f(0, 1, 0);
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    drawSquare();
    glPopMatrix();
}

void drawFace4()
{
    glColor3f(0, 1, 0);
    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotatef(90, 1, 0, 0);
    drawSquare();
    glPopMatrix();
}

void drawFace5()
{
    glColor3f(0, 0, 1);
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    drawSquare();
    glPopMatrix();
}

void drawFace6()
{
    glColor3f(0, 0, 1);
    glPushMatrix();
    glTranslatef(1, 0, 0);
    glRotatef(-90, 0, 1, 0);
    drawSquare();
    glPopMatrix();
}

void drawCube()
{
    drawFace1();
    drawFace2();
    drawFace3();
    drawFace4();
    drawFace5();
    drawFace6();
}

void displayHandler()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(6, 6, 6,
              0, 0, 0,
              0, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawCube();

    glutSwapBuffers();
}

void mouseHandler(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        cout << x << ' ' << y << '\n';
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 640);
    glutInitWindowPosition(500, 200);
    glutCreateWindow("test quiz");
    glutDisplayFunc(displayHandler);
    glutMouseFunc(mouseHandler);
    initGL();
    glutMainLoop();
    return 0;
}