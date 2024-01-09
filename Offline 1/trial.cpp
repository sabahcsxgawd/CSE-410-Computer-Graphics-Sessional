#include <bits/stdc++.h>
#include <GL/glut.h>
#include "myVector.h"

using namespace std;

class CollisionEvent {
public:
    int colsnCnt;
    float colsnTime;

    CollisionEvent(int colsnCnt, float colsnTime) {
        this->colsnCnt = colsnCnt;
        this->colsnTime = colsnTime;
    }

    bool operator < (const CollisionEvent& other) const {
        return this->colsnTime > other.colsnTime;
    }
};


priority_queue<CollisionEvent> colsnEvents;

myVector cameraEye(30.0f, -30.0f, 35.0f);
myVector cameraAt(0.0f, 0.0f, 0.0f);
myVector cameraUp(0.0f, 0.0f, 1.0f);

myVector f, s, u;

int isAutoSimulation = 0;

GLfloat ballMovementSpeed = 0.5f;
GLfloat ballMoveAngle = 0.0f;
GLfloat ballRadius = 2.0f;

GLfloat centerToBoundaryWallLength = 8.0f * ballRadius;

GLint totalColsnCnt = 0, segmentCount = 4, stackCount = 18, sectorCount = 36, ballAutoRotateRefreshMills = 80;

myVector ballMoveDirection(1.0f, -1.0f, 0.0f);
myVector ballCenterPos(0.0f, 0.0f, ballRadius);
vector<myVector> spherePoints[100];

void changeBallMoveDirectionXY(GLfloat angle) {
    angle = degreeToRadian(angle);
    GLfloat x = ballMoveDirection.getX() * cos(angle) - ballMoveDirection.getY() * sin(angle);
    GLfloat y = ballMoveDirection.getX() * sin(angle) + ballMoveDirection.getY() * cos(angle);
    ballMoveDirection.setX(x);
    ballMoveDirection.setY(y);
}

void updateCameraFSU() {
    f = (cameraAt - cameraEye).normalize();
    s = f.getCrossProduct(cameraUp.normalize()).normalize();
    cameraUp = u = s.normalize().getCrossProduct(f).normalize();
}

bool checkBallBoundaryCond() {
    bool rv = false;
    for(int i = 0; i < segmentCount; i++) {
        GLfloat theta = 2.0f * PI * (GLfloat)i / segmentCount;
        GLfloat a = cos(theta), b = sin(theta);
        GLfloat x2 = ballCenterPos.getX(), y2 = ballCenterPos.getY();
        GLfloat d = fabs(a*x2 + b*y2 - centerToBoundaryWallLength);
        if(d <= ballRadius) {
            rv = true;
            ballMoveDirection -=  myVector(a, b, 0.0f) * (2.0f * (a*ballMoveDirection.getX() + b*ballMoveDirection.getY()));
            ballCenterPos.setX(ballCenterPos.getX() + (d - ballRadius) * a);
            ballCenterPos.setY(ballCenterPos.getY() + (d - ballRadius) * b);
        }
    }
    return rv;
}

void drawXY(GLint step) {
    GLfloat isWhite = 1.0f;
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for (int i = 1000; i > -1000; i -= step) {
        isWhite = 1.0f - isWhite;
        for (int j = 1000; j > -1000; j -= step) {
            glColor3f(isWhite, isWhite, isWhite);
            glVertex2i(i, j - step);
            glVertex2i(i, j);
            glVertex2i(i + step, j - step);
            glVertex2i(i + step, j - step);
            glVertex2i(i, j);
            glVertex2i(i + step, j);

            isWhite = 1.0f - isWhite;
        }
    }
    glEnd();
    glPopMatrix();
}

void drawBoundarySegment(GLfloat length, GLint segmentCount) {
    GLfloat theta = PI / (GLfloat) segmentCount;
    GLfloat halfSide = length * tan(theta);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(length, -halfSide, 0.0f);
    glVertex3f(length, -halfSide, 1.2f * ballRadius);
    glVertex3f(length, halfSide, 1.2f * ballRadius);
    glVertex3f(length, halfSide, 0.0f);
    glEnd();
}

void drawBoundary(GLfloat length, GLint segmentCount) {
    for(int i = 0; i < segmentCount; i++) {
        glPushMatrix();
        glRotatef(360.0f * (GLfloat) i / (GLfloat) segmentCount, 0.0f, 0.0f, 1.0f);
        drawBoundarySegment(length, segmentCount);
        glPopMatrix();
    }
}

void makeInitialSpherePoints(GLfloat radius, GLint stackCount, GLint sectorCount) {
    GLfloat sectorConst = 2 * PI / (GLfloat) sectorCount;
    GLfloat stackConst = PI / (GLfloat) stackCount;

    spherePoints[0].emplace_back(0, 0, radius);
    for (int i = 1; i < stackCount; i++) {
        GLfloat phi = (PI / 2.0f) - ((GLfloat) i * stackConst);
        GLfloat z = radius * sin(phi);

        for (int j = 0; j < sectorCount; j++) {
            GLfloat theta = (GLfloat) j * sectorConst;
            GLfloat x = radius * cos(phi) * cos(theta);
            GLfloat y = radius * cos(phi) * sin(theta);
            spherePoints[i].emplace_back(x, y, z);
        }
    }
    spherePoints[stackCount].emplace_back(0, 0, -radius);

}

void rotateSpherePoints(GLfloat angle, GLint stackCount, GLint sectorCount) {
    for(int i = 0; i <= stackCount; i++) {
        for(myVector& point : spherePoints[i]) {
            point = point.rotateAroundAxis(angle, myVector(-ballMoveDirection.getY(), ballMoveDirection.getX(), 0.0f));
        }
    }
}

void drawSphere(GLfloat radius, GLint stackCount, GLint sectorCount) {
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    GLfloat isRed;

    for (int i = 0; i < stackCount; i++) {
        if (i & 1) {
            isRed = 0.0f;
        } else {
            isRed = 1.0f;
        }

        for (int j = 0; j < sectorCount; j++) {
            glColor3f(isRed, 1.0f - isRed, 0.0f);
            myVector k1, k2, k11, k21;

            if (i == 0) {
                k1 = spherePoints[i][0], k2 = spherePoints[i + 1][j];
                if (j == sectorCount - 1) {
                    k21 = spherePoints[i + 1][0];
                } else {
                    k21 = spherePoints[i + 1][j + 1];
                }
                glVertex3f(k1.getX(), k1.getY(), k1.getZ());
                glVertex3f(k2.getX(), k2.getY(), k2.getZ());
                glVertex3f(k21.getX(), k21.getY(), k21.getZ());
            } else if (i == stackCount - 1) {
                k1 = spherePoints[i][j], k2 = spherePoints[i + 1][0];
                if (j == sectorCount - 1) {
                    k11 = spherePoints[i][0];
                } else {
                    k11 = spherePoints[i][j + 1];
                }
                glVertex3f(k1.getX(), k1.getY(), k1.getZ());
                glVertex3f(k2.getX(), k2.getY(), k2.getZ());
                glVertex3f(k11.getX(), k11.getY(), k11.getZ());
            } else {
                k1 = spherePoints[i][j], k2 = spherePoints[i + 1][j];
                if (j == sectorCount - 1) {
                    k11 = spherePoints[i][0];
                    k21 = spherePoints[i + 1][0];
                } else {
                    k11 = spherePoints[i][j + 1];
                    k21 = spherePoints[i + 1][j + 1];
                }
                glVertex3f(k1.getX(), k1.getY(), k1.getZ());
                glVertex3f(k2.getX(), k2.getY(), k2.getZ());
                glVertex3f(k11.getX(), k11.getY(), k11.getZ());
                glVertex3f(k11.getX(), k11.getY(), k11.getZ());
                glVertex3f(k2.getX(), k2.getY(), k2.getZ());
                glVertex3f(k21.getX(), k21.getY(), k21.getZ());
            }

            isRed = 1.0f - isRed;
        }
    }
    glEnd();
    glPopMatrix();
}

void drawDirectionVector(GLfloat radius1, GLfloat radius2, GLfloat length) {
    GLfloat rotationAngle = atan(ballMoveDirection.getY() / ballMoveDirection.getX());
    if(ballMoveDirection.getX() < 0.0f) {
        if(ballMoveDirection.getY() > 0.0f) {
            rotationAngle += PI;
        }
        else {
            rotationAngle -= PI;
        }
    }

    glPushMatrix();

    glTranslatef(ballCenterPos.getX(), ballCenterPos.getY(), ballCenterPos.getZ());
    glRotatef(rotationAngle * 180.f / PI, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(length * 1.2f, 0.0f, 0.0f);
    for(int i = 0; i <= 100; i++) {
        GLfloat theta = 2.0f * PI * (GLfloat) i / 100.0f;
        glVertex3f(length, cos(theta) * radius2, sin(theta) * radius2);
    }
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.0f, 0.0f, 1.0f);
    for(int i = 0; i <= 100; i++) {
        GLfloat theta = 2.0f * PI * (GLfloat) i / 100.0f;
        glVertex3f(0.0f, cos(theta) * radius1, sin(theta) * radius1);
        glVertex3f(length, cos(theta) * radius1, sin(theta) * radius1);
    }
    glEnd();

    glPopMatrix();
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    updateCameraFSU();
    ballMoveDirection =  ballMoveDirection.normalize();
    makeInitialSpherePoints(ballRadius, stackCount, sectorCount);
}

void doBallAutoRotate(int value) {
    if(isAutoSimulation) {
        ballCenterPos.setX(ballCenterPos.getX() + ballMoveDirection.getX() * ballMovementSpeed);
        ballCenterPos.setY(ballCenterPos.getY() + ballMoveDirection.getY() * ballMovementSpeed);
        ballMoveAngle += 5.0f;
//        checkBallBoundaryCond();
        totalColsnCnt += checkBallBoundaryCond();
        rotateSpherePoints(ballMoveAngle, stackCount, sectorCount);
        glutPostRedisplay();
        glutTimerFunc(ballAutoRotateRefreshMills, doBallAutoRotate, 0);
    }
}

CollisionEvent gg(0, 0.0f);

void printCollision(int val) {
    if(!isAutoSimulation) {
        colsnEvents = priority_queue<CollisionEvent>();
        return;
    }
    if(val && checkBallBoundaryCond()) {
        totalColsnCnt++;
    }
    for(int i = 0; i < segmentCount; i++) {
        GLfloat theta = 2.0f * PI * (GLfloat)i / segmentCount;
        GLfloat a = cos(theta), b = sin(theta);
        GLfloat x2 = ballCenterPos.getX(), y2 = ballCenterPos.getY();
        GLfloat d = fabs(a*x2 + b*y2 - centerToBoundaryWallLength) - ballRadius;
        GLfloat velocityComponent = ballMoveDirection.getDotProduct(myVector(a, b, 0)) * ballMovementSpeed;
        GLfloat t = d / (velocityComponent / ballAutoRotateRefreshMills);
        if(t > 0.0f) {
            CollisionEvent temp(totalColsnCnt, t);
            colsnEvents.push(temp);
        }
    }

    while(!colsnEvents.empty()) {
        CollisionEvent temp = colsnEvents.top();
        colsnEvents.pop();
        if(temp.colsnCnt == totalColsnCnt) {
            glutTimerFunc(temp.colsnTime, printCollision, 1);
            break;
        }
    }
}

void displayHandler() {
    updateCameraFSU();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cameraEye.getX(), cameraEye.getY(), cameraEye.getZ(),
              cameraAt.getX(), cameraAt.getY(), cameraAt.getZ(),
              cameraUp.getX(), cameraUp.getY(), cameraUp.getZ());

    drawXY(4);
    drawBoundary(centerToBoundaryWallLength, segmentCount);
    drawDirectionVector(0.1f, 0.2f, 2.0f * ballRadius);

    glPushMatrix();
    glTranslatef(ballCenterPos.getX(), ballCenterPos.getY(), ballCenterPos.getZ());
    drawSphere(ballRadius, stackCount, sectorCount);
    glPopMatrix();

    glutSwapBuffers();

}

void reshapeHandler(int w, int h) {
    h = (h == 0) ? 1 : h;
    w = (w == 0) ? 1 : w;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat) w / (GLfloat) h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardHandler(unsigned char key, int x, int y) {
    GLfloat cameraRotationChangeAngle = 0.8f, cameraMovementSpeed = 0.4f;
    switch (key) {
        case '1':
            cameraAt = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, u).normalize();
            break;
        case '2':
            cameraAt = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, u).normalize();
            break;
        case '3':
            cameraAt = cameraEye + f.rotateAroundAxis(cameraRotationChangeAngle, s).normalize();
            break;
        case '4':
            cameraAt = cameraEye + f.rotateAroundAxis(-cameraRotationChangeAngle, s).normalize();
            break;
        case '5':
            cameraUp = cameraUp.rotateAroundAxis(cameraRotationChangeAngle, f).normalize();
            break;
        case '6':
            cameraUp = cameraUp.rotateAroundAxis(-cameraRotationChangeAngle, f).normalize();
            break;
        case 'w':
//            cameraEye.setZ(cameraEye.getZ() + cameraMovementSpeed);
            break;
        case 's':
//            cameraEye.setZ(cameraEye.getZ() - cameraMovementSpeed);
            break;
        case 'j':
            changeBallMoveDirectionXY(5.0f);
            if(isAutoSimulation) {
                printCollision(1);
            }
            break;
        case 'l':
            changeBallMoveDirectionXY(-5.0f);
            if(isAutoSimulation) {
                printCollision(1);
            }
            break;
        case 'i':
            ballCenterPos.setX(ballCenterPos.getX() + ballMoveDirection.getX() * ballMovementSpeed);
            ballCenterPos.setY(ballCenterPos.getY() + ballMoveDirection.getY() * ballMovementSpeed);
            ballMoveAngle += 5.0f;
            totalColsnCnt += checkBallBoundaryCond();
//            checkBallBoundaryCond();
            rotateSpherePoints(ballMoveAngle, stackCount, sectorCount);
            if(isAutoSimulation) {
                printCollision(1);
            }
            break;
        case 'k':
            ballCenterPos.setX(ballCenterPos.getX() - ballMoveDirection.getX() * ballMovementSpeed);
            ballCenterPos.setY(ballCenterPos.getY() - ballMoveDirection.getY() * ballMovementSpeed);
            ballMoveAngle -= 5.0f;
            totalColsnCnt += checkBallBoundaryCond();
//            checkBallBoundaryCond();
            rotateSpherePoints(ballMoveAngle, stackCount, sectorCount);
            if(isAutoSimulation) {
                printCollision(1);
            }
            break;
        case ' ':
            isAutoSimulation = 1 - isAutoSimulation;
            if(isAutoSimulation) {
                glutTimerFunc(0, doBallAutoRotate, 0);
                glutTimerFunc(0, printCollision, 0);
            }
            break;
        default:
            cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

void specialKeyboardHandler(int key, int x, int y) {
    GLfloat cameraMovementSpeed = 0.4f;
    switch (key) {
        case GLUT_KEY_UP:
            cameraEye += f * cameraMovementSpeed;
            cameraAt += f * cameraMovementSpeed;
            break;
        case GLUT_KEY_DOWN:
            cameraEye -= f * cameraMovementSpeed;
            cameraAt -= f * cameraMovementSpeed;
            break;
        case GLUT_KEY_RIGHT:
            cameraEye += s * cameraMovementSpeed;
            cameraAt += s * cameraMovementSpeed;
            break;
        case GLUT_KEY_LEFT:
            cameraEye -= s * cameraMovementSpeed;
            cameraAt -= s * cameraMovementSpeed;
            break;
        case GLUT_KEY_PAGE_UP:
            cameraEye += u * cameraMovementSpeed;
            cameraAt += u * cameraMovementSpeed;
            break;
        case GLUT_KEY_PAGE_DOWN:
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