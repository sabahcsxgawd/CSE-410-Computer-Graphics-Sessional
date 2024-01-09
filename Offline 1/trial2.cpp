#include <bits/stdc++.h>
#include <GL/glut.h>
#include "myVector.h"

using namespace std;

myVector cameraEye(2.0f, -2.0f, 2.0f);
myVector cameraAt(0.0f, 0.0f, 0.0f);
myVector cameraUp(0.0f, 0.0f, 1.0f);

myVector f(0.0f, 0.0f, 0.0f);
myVector s(0.0f, 0.0f, 0.0f);
myVector u(0.0f, 0.0f, 0.0f);

GLfloat octahedronColors[][3] = {
        {1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f}
};

GLfloat sphereSegmentColors[][3] = {
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f}
};

GLfloat cylinderSegmentColor[] = {1.0f, 1.0f, 0.0f};

GLfloat rotationAngle = 0.0f;
GLfloat sphereRadius = 0.0f;

void updateCameraFSU() {
    f = (cameraAt - cameraEye).normalize();
    s = f.getCrossProduct(cameraUp.normalize()).normalize();
    u = s.normalize().getCrossProduct(f).normalize();
}

void drawAxes() {
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

void drawTriangle(GLfloat *triangleColor) {
    glBegin(GL_TRIANGLES);
    {
        glColor3fv(triangleColor);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    }
    glEnd();
}

void drawUpperOctahedron() {
    GLfloat scalingFactor = 1 - sqrt(3.0f) * sphereRadius;
    GLfloat translationFactor = sphereRadius / sqrt(3.0f);
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(90.0f * (GLfloat) i, 0.0f, 0.0f, 1.0f);
        glTranslatef(translationFactor, translationFactor, translationFactor);
        glScalef(scalingFactor, scalingFactor, scalingFactor);
        drawTriangle(octahedronColors[i & 1]);
        glPopMatrix();
    }
}

void drawOctahedron() {
    drawUpperOctahedron();
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    drawUpperOctahedron();
    glPopMatrix();
}

void drawCylinderSegment(GLfloat radius, GLfloat height, GLint subDivision) {
    GLfloat g = 1.0f - radius * sqrt(3.0f);
    GLfloat c = sqrt(radius*radius + g*g + sqrt(2.0f) * radius * g);
    GLfloat alpha = atan(radius / (radius + sqrt(2.0f) * g));
    height = 2.0f * c * sin((PI / 4.0f) - alpha);
    GLfloat l = -radius + (height / (2.0f * tan(PI / 4.0f - alpha)));

    GLint pointsPerRow = 1 + (1 << subDivision);
    GLfloat const1 = PI / 4.0f;
    GLfloat const2 = PI / (GLfloat) (1 << (subDivision + 1));

    glPushMatrix();
    glRotatef(45.0f, 0.0f, -1.0f, 0.0f);
    glTranslatef(l, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(cylinderSegmentColor);
    for(int i = 0; i < pointsPerRow; i++) {
        GLfloat theta = const1 - const2 * (GLfloat) i;
        GLfloat x = radius * cos(theta), y = radius * sin(theta);
        glVertex3f(x, y, height / 2.0f);
        glVertex3f(x, y, -height / 2.0f);
    }
    glEnd();

    glPopMatrix();
}

void drawUpperCylinderSegments() {
    glPushMatrix();
    for(int i = 0; i < 4; i++) {
        glRotatef(90.0f * (GLfloat) i, 0.0f, 0.0f, 1.0f);
        drawCylinderSegment(sphereRadius, 1.0f, 5);
    }
    glPopMatrix();
}

void drawMiddleCylinderSegments() {
    glPushMatrix();
    glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; i++) {
        glRotatef(90.0f * (GLfloat) i, 0.0f, -1.0f, 0.0f);
        drawCylinderSegment(sphereRadius, 1.0f, 5);
    }
    glPopMatrix();
}

void drawLowerCylinderSegments() {
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    for(int i = 0; i < 4; i++) {
        glRotatef(90.0f * (GLfloat) i, 0.0f, 0.0f, 1.0f);
        drawCylinderSegment(sphereRadius, 1.0f, 5);
    }
    glPopMatrix();
}

void drawCylinders() {
    drawUpperCylinderSegments();
    drawMiddleCylinderSegments();
    drawLowerCylinderSegments();
}

void drawSphereSegment(GLfloat radius, GLint subDivision, GLfloat *SSColor) {
    GLint pointsPerRow = 1 + (1 << subDivision);
    GLfloat const1 = PI / 4.0f;
    GLfloat const2 = PI / (GLfloat) (1 << (subDivision + 1));

    myVector n1, n2, k1, k2, k11, k21;
    vector<myVector> points[pointsPerRow];

    for (int i = 0; i < pointsPerRow; i++) {
        GLfloat theta = const1 - const2 * (GLfloat) i;
        GLfloat x = sin(theta), y = -cos(theta);
        n1 = myVector(x, y, 0.0f);

        for (int j = 0; j < pointsPerRow; j++) {
            GLfloat phi = const1 - const2 * (GLfloat) j;
            x = sin(phi);
            GLfloat z = -cos(phi);
            n2 = myVector(x, 0.0f, z);
            myVector p = n1.getCrossProduct(n2).normalize() * radius;
            points[i].emplace_back(p);
        }
    }

    glBegin(GL_TRIANGLES);
    glColor3fv(SSColor);
    for (int i = 0; i < pointsPerRow - 1; i++) {
        for (int j = 0; j < pointsPerRow - 1; j++) {
            k1 = myVector(points[i][j]);
            k2 = myVector(points[i + 1][j]);
            k11 = myVector(points[i][j + 1]);
            k21 = myVector(points[i + 1][j + 1]);

            glVertex3f(k1.getX(), k1.getY(), k1.getZ());
            glVertex3f(k2.getX(), k2.getY(), k2.getZ());
            glVertex3f(k11.getX(), k11.getY(), k11.getZ());
            glVertex3f(k11.getX(), k11.getY(), k11.getZ());
            glVertex3f(k2.getX(), k2.getY(), k2.getZ());
            glVertex3f(k21.getX(), k21.getY(), k21.getZ());
        }
    }
    glEnd();
}

void drawSphere(GLfloat radius) {
    GLfloat g = 1.0f - radius * sqrt(3.0f);

    for(int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(90.0f * (GLfloat) i, 0.0f, 0.0f, 1.0f);
        glTranslatef(g, 0.0f, 0.0f);
        drawSphereSegment(radius, 5, sphereSegmentColors[i & 1]);
        glPopMatrix();
    }

    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(g, 0.0f, 0.0f);
    drawSphereSegment(radius, 5, sphereSegmentColors[2]);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(g, 0.0f, 0.0f);
    drawSphereSegment(radius, 5, sphereSegmentColors[2]);
    glPopMatrix();
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    updateCameraFSU();
}

void displayHandler() {
    updateCameraFSU();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cameraEye.getX(), cameraEye.getY(), cameraEye.getZ(),
              cameraAt.getX(), cameraAt.getY(), cameraAt.getZ(),
              cameraUp.getX(), cameraUp.getY(), cameraUp.getZ());

    glPushMatrix();
    glRotatef(rotationAngle, 0.0f, 0.0f, -1.0f);
//    drawAxes();
    drawSphere(sphereRadius);
    drawOctahedron();
    drawCylinders();
    glPopMatrix();

    glutSwapBuffers();
}

void reshapeHandler(int w, int h) {
    h = (h == 0) ? 1 : h;
    w = (w == 0) ? 1 : w;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) w / (GLfloat) h, 1.0f, 100.0f);
    // gluPerspective(80.0f, 1.0f, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardHandler(unsigned char key, int x, int y) {
    GLfloat cameraRotationChangeAngle = 0.8f, cameraMovementSpeed = 0.4f;
    switch (key) {
        case '1':
            cameraUp = u;
            cameraAt = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, u).normalize();
            break;
        case '2':
            cameraUp = u;
            cameraAt = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, u).normalize();
            break;
        case '3':
            cameraUp = u;
            cameraAt = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, s).normalize();
            break;
        case '4':
            cameraUp = u;
            cameraAt = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, s).normalize();
            break;
        case '5':
            cameraUp = u.rotateAroundAxis(cameraRotationChangeAngle, f).normalize();
            break;
        case '6':
            cameraUp = u.rotateAroundAxis(-cameraRotationChangeAngle, f).normalize();
            break;
        case 'w':
            // cameraEye.setZ(cameraEye.getZ() + cameraMovementSpeed);
            cameraEye += cameraUp * cameraMovementSpeed;
            break;
        case 's':
            cameraEye -= cameraUp * cameraMovementSpeed;
            break;
        case 'a':
            rotationAngle += 5.0f;
            break;
        case 'd':
            rotationAngle -= 5.0f;
            break;
        case ',':
            sphereRadius = min(1.0f / sqrt(3.0f), sphereRadius + 0.02f);
            break;
        case '.':
            sphereRadius = max(0.0f, sphereRadius - 0.02f);
            break;
        default:
            cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

void specialKeyboardHandler(int key, int x, int y) {
    GLfloat cameraMovementSpeed = 0.8f;
    switch (key) {
        case GLUT_KEY_UP:
        cameraUp = u;
            cameraEye += f * cameraMovementSpeed;
            cameraAt += f * cameraMovementSpeed;
            break;
        case GLUT_KEY_DOWN:
        cameraUp = u;
            cameraEye -= f * cameraMovementSpeed;
            cameraAt -= f * cameraMovementSpeed;
            break;
        case GLUT_KEY_RIGHT:
        cameraUp = u;
            cameraEye += s * cameraMovementSpeed;
            cameraAt += s * cameraMovementSpeed;
            break;
        case GLUT_KEY_LEFT:
        cameraUp = u;
            cameraEye -= s * cameraMovementSpeed;
            cameraAt -= s * cameraMovementSpeed;
            break;
        case GLUT_KEY_PAGE_UP:
        cameraUp = u;
            cameraEye += u * cameraMovementSpeed;
            cameraAt += u * cameraMovementSpeed;
            break;
        case GLUT_KEY_PAGE_DOWN:
        cameraUp = u;
            cameraEye -= u * cameraMovementSpeed;
            cameraAt -= u * cameraMovementSpeed;
            break;
        default:
            cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(720, 600);
    glutCreateWindow("Trial");
    glutDisplayFunc(displayHandler);
    glutReshapeFunc(reshapeHandler);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyboardHandler);
    initGL();
    glutMainLoop();
    return 0;
}