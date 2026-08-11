#include "editor/Viewport.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

namespace Omega {

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent) {
    setMinimumSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);
}

Viewport::~Viewport() {
    makeCurrent();
}

void Viewport::shutdown() {
    doneCurrent();
}

void Viewport::initializeGL() {
    initializeOpenGLFunctions();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    
    m_timer.start();
}

void Viewport::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    updateCamera();
    renderGrid();
    
    update();
}

void Viewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void Viewport::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_lastMousePos = event->pos();
    }
}

void Viewport::mouseMoveEvent(QMouseEvent* event) {
    if (m_mousePressed) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_rotationY += delta.x() * 0.5f;
        m_rotationX += delta.y() * 0.5f;
        m_lastMousePos = event->pos();
        update();
    }
}

void Viewport::wheelEvent(QWheelEvent* event) {
    m_zoom += event->angleDelta().y() * 0.001f;
    m_zoom = std::max(0.1f, std::min(10.0f, m_zoom));
    update();
}

void Viewport::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_W: /* Move forward */ break;
        case Qt::Key_S: /* Move backward */ break;
        case Qt::Key_A: /* Move left */ break;
        case Qt::Key_D: /* Move right */ break;
    }
    update();
}

void Viewport::renderGrid() {
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.5f);
    
    for (int i = -10; i <= 10; i++) {
        glVertex3f(i * 1.0f, 0, -10);
        glVertex3f(i * 1.0f, 0, 10);
        glVertex3f(-10, 0, i * 1.0f);
        glVertex3f(10, 0, i * 1.0f);
    }
    
    glEnd();
}

void Viewport::updateCamera() {
    // Camera logic will be implemented later
}

} // namespace Omega
