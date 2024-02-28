#include <iostream>
#include <GL/freeglut.h>
#include <optional>
#include <chrono>
#include <set>
#include <functional>
#include <thread>
#include <cmath>
#include "time.hpp"

class point
{
public:
    float x;
    float y;
};

class color
{
public:
    float r;
    float g;
    float b;
};

constexpr size_t circle_segments = 50;
constexpr color color0 = {0.0f, 0.0f, 1.0f};
constexpr color color1 = {1.0f, 0.0f, 0.0f};
constexpr color color2 = {1.0f, 1.0f, 0.0f};
constexpr point center0 = {0.0f, 0.0f};
constexpr float radius0 = 0.5f;
constexpr float radius1 = 0.25f;
constexpr float radius2 = 0.1f;
constexpr float frequency0 = 0.25f;
constexpr float frequency1 = 0.5f;
size_t frame_count = 0;

void draw_circle(const point &_center, const float &_radius, const color &_color)
{
    glBegin(GL_LINES);
    glColor3f(_color.r, _color.g, _color.b);

    for(size_t i = 0; i < circle_segments; ++i)
    {
        const size_t start_idx = i;
        const size_t end_idx = (i + 1) % circle_segments;
        const float line_start_x = std::cos(start_idx * 2.0f * M_PI / circle_segments) * _radius + _center.x;
        const float line_start_y = std::sin(start_idx * 2.0f * M_PI / circle_segments) * _radius + _center.y;
        const float line_end_x = std::cos(end_idx * 2.0f * M_PI / circle_segments) * _radius + _center.x;
        const float line_end_y = std::sin(end_idx * 2.0f * M_PI / circle_segments) * _radius + _center.y;
        glVertex2f(line_start_x, line_start_y);
        glVertex2f(line_end_x, line_end_y);
    }

    glEnd();
}

void draw_circle_line(const point &_start, const point &_end, const color &_color)
{
    glBegin(GL_LINES);
    glColor3f(_color.r, _color.g, _color.b);
    glVertex2f(_start.x, _start.y);
    glVertex2f(_end.x, _end.y);
    glEnd();
}

void display_callback()  // draw each frame
{
    time::start_frame();
    glClear(GL_COLOR_BUFFER_BIT);

    point center1;
    point center2;
    center1.x = center0.x + std::cos(frame_count * frequency0 * 2.0 * M_PI / time::FPS()) * radius0;
    center1.y = center0.y + std::sin(frame_count * frequency0 * 2.0 * M_PI / time::FPS()) * radius0;
    center2.x = center1.x + std::cos(frame_count * frequency1 * 2.0 * M_PI / time::FPS()) * radius1;
    center2.y = center1.y + std::sin(frame_count * frequency1 * 2.0 * M_PI / time::FPS()) * radius1;

    draw_circle(center0, radius0, color0);
    draw_circle_line(center0, center1, color0);
    draw_circle(center1, radius1, color1);
    draw_circle_line(center1, center2, color1);
    draw_circle(center2, radius2, color2);

    glutSwapBuffers();
    glutPostRedisplay();    // draw next frame
    time::end_frame();

    ++frame_count;
}

int main(int argc, char **argv)
{
    time::initialise();

    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutCreateWindow("online-1-b1");
    glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glutDisplayFunc(display_callback);
    glutMainLoop();

    return 0;
}