#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QElapsedTimer>

namespace Omega {

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
    
public:
    explicit Viewport(QWidget* parent = nullptr);
    ~Viewport();
    
    void shutdown();
    
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    
private:
    void renderGrid();
    void updateCamera();
    
    float m_zoom = 1.0f;
    float m_rotationX = 0.0f;
    float m_rotationY = 0.0f;
    QPoint m_lastMousePos;
    bool m_mousePressed = false;
    
    QElapsedTimer m_timer;
};

} // namespace Omega
