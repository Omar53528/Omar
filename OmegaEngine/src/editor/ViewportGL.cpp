#include "editor/ViewportGL.h"
#include <FL/fl_draw.H>
#include "../core/Log.h"

namespace Omega { namespace Editor {

ViewportGL::ViewportGL(int x, int y, int w, int h) 
    : Fl_Gl_Window(x, y, w, h), m_is3D(true), m_mousePressed(false),
      m_lastMouseX(0), m_lastMouseY(0), m_zoomLevel(1.0f), 
      m_rotationX(0), m_rotationY(0) {
    
    this->mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
}

ViewportGL::~ViewportGL() {}

int ViewportGL::handle(int event) {
    switch (event) {
        case FL_PUSH:
            m_mousePressed = true;
            m_lastMouseX = Fl::event_x();
            m_lastMouseY = Fl::event_y();
            return 1;
            
        case FL_DRAG:
            if (m_mousePressed) {
                int dx = Fl::event_x() - m_lastMouseX;
                int dy = Fl::event_y() - m_lastMouseY();
                
                if (Fl::event_state(FL_BUTTON3)) {
                    // زر الماوس الأيمن: تدوير
                    rotate(dx * 0.5f, dy * 0.5f);
                } else if (Fl::event_state(FL_BUTTON2) || Fl::event_state(FL_SHIFT)) {
                    // زر الماوس الأوسط أو Shift: تحريك
                    pan(dx * 0.1f, dy * 0.1f);
                }
                
                m_lastMouseX = Fl::event_x();
                m_lastMouseY = Fl::event_y();
                redraw();
            }
            return 1;
            
        case FL_RELEASE:
            m_mousePressed = false;
            return 1;
            
        case FL_MOUSEWHEEL:
            zoom(Fl::event_dy() * 0.1f);
            redraw();
            return 1;
            
        default:
            return Fl_Gl_Window::handle(event);
    }
}

void ViewportGL::draw() {
    if (!valid()) {
        glViewport(0, 0, w(), h());
        
        // إعداد الكاميرا
        float aspect = (float)w() / (float)h();
        m_camera.setPerspective(45.0f, aspect, 0.1f, 1000.0f);
        m_camera.setPosition({0, 2, 5});
        m_camera.lookAt({0, 0, 0});
    }
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    
    // رسم الشبكة
    drawGrid();
    
    // رسم المحاور (Gizmos)
    drawGizmos();
}

void ViewportGL::drawGrid() {
    glBegin(GL_LINES);
    glColor3f(0.3f, 0.3f, 0.3f);
    
    // شبكة أرضية
    for (int i = -10; i <= 10; ++i) {
        glVertex3f(i, 0, -10);
        glVertex3f(i, 0, 10);
        glVertex3f(-10, 0, i);
        glVertex3f(10, 0, i);
    }
    
    glEnd();
}

void ViewportGL::drawGizmos() {
    // رسم المحاور XYZ
    glBegin(GL_LINES);
    
    // محور X (أحمر)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    
    // محور Y (أخضر)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    
    // محور Z (أزرق)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    
    glEnd();
}

void ViewportGL::zoom(float delta) {
    m_zoomLevel += delta;
    if (m_zoomLevel < 0.1f) m_zoomLevel = 0.1f;
    if (m_zoomLevel > 10.0f) m_zoomLevel = 10.0f;
    
    auto pos = m_camera.getPosition();
    float dist = length(pos);
    float newDist = dist * (1.0f - delta * 0.1f);
    
    if (newDist > 0.5f && newDist < 100.0f) {
        m_camera.setPosition(normalize(pos) * newDist);
    }
}

void ViewportGL::pan(float dx, float dy) {
    auto pos = m_camera.getPosition();
    auto right = m_camera.getRight();
    auto up = m_camera.getUp();
    
    glm::vec3 newPos = pos + right * dx - up * dy;
    m_camera.setPosition(newPos);
}

void ViewportGL::rotate(float dx, float dy) {
    m_rotationY += dx;
    m_rotationX += dy;
    
    // تقييد الدوران العمودي
    if (m_rotationX > 89.0f) m_rotationX = 89.0f;
    if (m_rotationX < -89.0f) m_rotationX = -89.0f;
    
    // تحديث موقع الكاميرا بناءً على الزوايا
    float radius = length(m_camera.getPosition());
    float yaw = glm::radians(m_rotationY);
    float pitch = glm::radians(m_rotationX);
    
    glm::vec3 newPos;
    newPos.x = radius * cos(pitch) * sin(yaw);
    newPos.y = radius * sin(pitch);
    newPos.z = radius * cos(pitch) * cos(yaw);
    
    m_camera.setPosition(newPos);
    m_camera.lookAt({0, 0, 0});
}

void ViewportGL::resetCamera() {
    m_rotationX = 0;
    m_rotationY = 0;
    m_zoomLevel = 1.0f;
    m_camera.setPosition({0, 2, 5});
    m_camera.lookAt({0, 0, 0});
    redraw();
}

}} // namespace Omega::Editor
