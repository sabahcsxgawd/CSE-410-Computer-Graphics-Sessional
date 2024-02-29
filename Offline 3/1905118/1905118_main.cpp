#include "1905118_classes.h"

using namespace std;

double fovy = 80.0f;
double aspect = 1.0f;
double zNear = 0.01f;
double zFar = 1000.0f;

Vector3D cameraEye(100.0f, -100.0f, 100.0f);
Vector3D cameraCenter(0.0f, 0.0f, 0.0f);
Vector3D cameraUp(0.0f, 0.0f, 1.0f);

Vector3D f(0.0f, 0.0f, 0.0f);
Vector3D s(0.0f, 0.0f, 0.0f);
Vector3D u(0.0f, 0.0f, 0.0f);

int recursionLevel, imageCount;
int screenWidth, screenHeight, imageWidth, imageHeight;

vector<Object *> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;

bitmap_image image;

void loadData()
{
    ifstream fin("scene_test.txt");
    if (!fin)
    {
        cout << "Error opening file\n";
        return;
    }

    fin >> recursionLevel;
    fin >> screenWidth;
    screenHeight = screenWidth;
    imageWidth = screenWidth;
    imageHeight = screenHeight;

    Object *temp;
    int numObjects;
    fin >> numObjects;
    for (int i = 0; i < numObjects; i++)
    {
        string type;
        fin >> type;
        if (type == "sphere")
        {
            double x, y, z, radius;
            double color[3], coefficients[4], shine;
            fin >> x >> y >> z >> radius;
            fin >> color[0] >> color[1] >> color[2];
            fin >> coefficients[0] >> coefficients[1] >> coefficients[2] >> coefficients[3];
            fin >> shine;
            temp = new Sphere(Vector3D(x, y, z), radius);
            temp->setColor(color);
            temp->setCoEfficients(coefficients);
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else if (type == "triangle")
        {
            double x1, y1, z1, x2, y2, z2, x3, y3, z3;
            double color[3], coefficients[4], shine;
            fin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
            fin >> color[0] >> color[1] >> color[2];
            fin >> coefficients[0] >> coefficients[1] >> coefficients[2] >> coefficients[3];
            fin >> shine;
            temp = new Triangle(Vector3D(x1, y1, z1), Vector3D(x2, y2, z2), Vector3D(x3, y3, z3));
            temp->setColor(color);
            temp->setCoEfficients(coefficients);
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else if (type == "general")
        {
            double A, B, C, D, E, F, G, H, I, J;
            double referencePointX, referencePointY, referencePointZ;
            double length, width, height;
            double color[3], coefficients[4];
            int shine;
            fin >> A >> B >> C >> D >> E >> F >> G >> H >> I >> J;
            fin >> referencePointX >> referencePointY >> referencePointZ;
            fin >> length >> width >> height;
            fin >> color[0] >> color[1] >> color[2];
            fin >> coefficients[0] >> coefficients[1] >> coefficients[2] >> coefficients[3];
            fin >> shine;
            temp = new General(A, B, C, D, E, F, G, H, I, J, Vector3D(referencePointX, referencePointY, referencePointZ), length, width, height);
            temp->setColor(color);
            temp->setCoEfficients(coefficients);
            temp->setShine(shine);
            objects.push_back(temp);
        }
        else
        {
            cout << "Unknown object type\n";
        }
    }

    temp = new Floor(1000, 20);
    double floorCoEfficients[] = {0.4, 0.4, 0.3, 0.2};
    double floorShine = 5;
    temp->setShine(floorShine);
    temp->setCoEfficients(floorCoEfficients);
    objects.push_back(temp);

    int numPointLights;
    fin >> numPointLights;
    for (int i = 0; i < numPointLights; i++)
    {
        double x, y, z;
        double color[3];
        fin >> x >> y >> z;
        fin >> color[0] >> color[1] >> color[2];
        PointLight pointLight(Vector3D(x, y, z), color);
        pointLights.push_back(pointLight);
    }

    int numSpotLights;
    fin >> numSpotLights;
    for (int i = 0; i < numSpotLights; i++)
    {
        double x, y, z, dx, dy, dz, angle;
        double color[3];
        fin >> x >> y >> z >> dx >> dy >> dz >> angle;
        fin >> color[0] >> color[1] >> color[2];
        SpotLight spotLight(Vector3D(x, y, z), Vector3D(dx, dy, dz), angle, color);
        spotLights.push_back(spotLight);
    }

    fin.close();
}

void capture()
{
    imageCount++;
    image.setwidth_height(imageWidth, imageHeight, true);

    double planeDistance = (screenWidth / 2.0) / tan((fovy / 2.0) * M_PI / 180.0);
    Vector3D windowTopLeft = cameraEye + f * planeDistance - s * (screenWidth / 2.0) + u * (screenHeight / 2.0);

    double du = screenWidth / imageWidth;
    double dv = screenHeight / imageHeight;
    double dummyColor[3] = {0.0, 0.0, 0.0};

    Vector3D topLeft = windowTopLeft + s * (du / 2.0) - u * (dv / 2.0);

    for (int i = 0; i < imageWidth; i++)
    {
        for (int j = 0; j < imageHeight; j++)
        {
            Vector3D currPixel = topLeft + s * i * du - u * j * dv;
            Ray ray(cameraEye, (currPixel - cameraEye).normalize());

            double tMin = DBL_MAX;
            Object *nearestObject = nullptr;

            for (Object *object : objects)
            {
                double t = object->intersect(ray, dummyColor, 0);
                if (t > 0 && t < tMin)
                {
                    tMin = t;
                    nearestObject = object;
                }
            }

            if (nearestObject != nullptr)
            {
                double *color = new double[3];
                tMin = nearestObject->intersect(ray, color, 1);

                color[0] = max(0.0, min(1.0, color[0]));
                color[1] = max(0.0, min(1.0, color[1]));
                color[2] = max(0.0, min(1.0, color[2]));

                image.set_pixel(i, j, color[0] * 255, color[1] * 255, color[2] * 255);

                delete[] color;
            }
        }
    }

    string imageName = "Output1" + to_string(imageCount) + ".bmp";
    image.save_image(imageName);
}

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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, aspect, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

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
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
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
    gluPerspective(80.0f, (GLfloat)w / (GLfloat)h, 0.01f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardHandler(unsigned char key, int x, int y)
{
    GLfloat cameraRotationChangeAngle = 0.8f * M_PI / 180.0f;
    switch (key)
    {
    case '0':
        capture();
        break;
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
    GLfloat cameraMovementSpeed = 0.8f;
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
    case GLUT_KEY_END:
        objects.clear();
        objects.shrink_to_fit();

        pointLights.clear();
        pointLights.shrink_to_fit();

        spotLights.clear();
        spotLights.shrink_to_fit();

        exit(0);
        break;
    default:
        cout << "UNKNOWN\n";
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    loadData();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("1905118 Ray Tracing");
    glutDisplayFunc(displayHandler);
    glutReshapeFunc(reshapeHandler);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyboardHandler);
    initGL();
    glutMainLoop();

    return 0;
}