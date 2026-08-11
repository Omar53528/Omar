#ifndef VIEWPORT_GL_H
#define VIEWPORT_GL_H

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include "../core/Types.h"
#include "../graphics/Camera3D.h"

namespace Omega { namespace Editor {

/**
 * @brief نافذة العرض ثلاثية الأبعاد باستخدام OpenGL و FLTK
 */
class ViewportGL : public Fl_Gl_Window {
public:
    ViewportGL(int x, int y, int w, int h);
    ~ViewportGL();

    int handle(int event) override;
    void draw() override;

    // إعدادات الكاميرا
    void setCameraMode(bool is3D) { m_is3D = is3D; }
    
    // التحكم بالكاميرا
    void resetCamera();
    void zoom(float delta);
    void pan(float dx, float dy);
    void rotate(float dx, float dy);

    // حالة الماوس
    bool isMouseOver() const { return Fl::event_inside(this); }

private:
    void drawGrid();
    void drawGizmos();
    void updateCameraFromInput();

    bool m_is3D;
    Graphics::Camera3D m_camera;
    
    // حالة الماوس للتفاعل
    bool m_mousePressed;
    int m_lastMouseX, m_lastMouseY;
    float m_zoomLevel;
    float m_rotationX, m_rotationY;
};

}} // namespace Omega::Editor

#endif // VIEWPORT_GL_H
