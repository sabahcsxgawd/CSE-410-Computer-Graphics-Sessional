#include <bits/stdc++.h>
#include <GL/glut.h>
#include "1905118_classes.h"

using namespace std;

Vector3D cameraEye(2.0f, -2.0f, 2.0f);
Vector3D cameraCenter(0.0f, 0.0f, 0.0f);
Vector3D cameraUp(0.0f, 0.0f, 1.0f);

Vector3D f(0.0f, 0.0f, 0.0f);
Vector3D s(0.0f, 0.0f, 0.0f);
Vector3D u(0.0f, 0.0f, 0.0f);

void updateCameraFSU()
{
    f = (cameraCenter - cameraEye).normalize();
    s = f.cross(cameraUp.normalize()).normalize();
    cameraUp = u = s.normalize().cross(f).normalize();
}

void drawAxes()
{
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    updateCameraFSU();
}

void displayHandler()
{
    updateCameraFSU();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cameraEye.x, cameraEye.y, cameraEye.z,
              cameraCenter.x, cameraCenter.y, cameraCenter.z,
              cameraUp.x, cameraUp.y, cameraUp.z);

    glPushMatrix();
    drawAxes();
    glPopMatrix();

    glutSwapBuffers();
}

void reshapeHandler(int w, int h)
{
    h = (h == 0) ? 1 : h;
    w = (w == 0) ? 1 : w;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.01f, 100.0f);
    gluPerspective(80.0f, (GLfloat)w / (GLfloat)h, 0.01f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardHandler(unsigned char key, int x, int y)
{   
    GLfloat cameraRotationChangeAngle = 0.8f * M_PI / 180.0f;
    switch (key)
    {
    case '1':
        cameraCenter = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, u).normalize();
        break;
    case '2':
        cameraCenter = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, u).normalize();
        break;
    case '3':
        cameraCenter = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, s).normalize();
        break;
    case '4':
        cameraCenter = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, s).normalize();
        break;
    case '5':
        cameraUp = cameraUp.rotateAroundAxis(cameraRotationChangeAngle, f).normalize();
        break;
    case '6':
        cameraUp = cameraUp.rotateAroundAxis(-cameraRotationChangeAngle, f).normalize();
        break;   
    default:
        cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

void specialKeyboardHandler(int key, int x, int y)
{
    GLfloat cameraMovementSpeed = 0.5f;
    switch (key)
    {
    case GLUT_KEY_UP:
        cameraEye = cameraEye + f * cameraMovementSpeed;
        cameraCenter = cameraCenter + f * cameraMovementSpeed;
        break;
    case GLUT_KEY_DOWN:
        cameraEye = cameraEye - f * cameraMovementSpeed;
        cameraCenter = cameraCenter - f * cameraMovementSpeed;
        break;
    case GLUT_KEY_RIGHT:
        cameraEye = cameraEye + s * cameraMovementSpeed;
        cameraCenter = cameraCenter + s * cameraMovementSpeed;
        break;
    case GLUT_KEY_LEFT:
        cameraEye = cameraEye - s * cameraMovementSpeed;
        cameraCenter = cameraCenter - s * cameraMovementSpeed;
        break;
    case GLUT_KEY_PAGE_UP:
        cameraEye = cameraEye + u * cameraMovementSpeed;
        cameraCenter = cameraCenter + u * cameraMovementSpeed;
        break;
    case GLUT_KEY_PAGE_DOWN:
        cameraEye = cameraEye - u * cameraMovementSpeed;
        cameraCenter = cameraCenter - u * cameraMovementSpeed;
        break;
    default:
        cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(720, 720);
    glutCreateWindow("1905118 Ray Tracing");
    glutDisplayFunc(displayHandler);
    glutReshapeFunc(reshapeHandler);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyboardHandler);
    initGL();
    glutMainLoop();
    return 0;
}