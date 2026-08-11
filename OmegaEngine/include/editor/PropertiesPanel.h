#ifndef PROPERTIES_PANEL_H
#define PROPERTIES_PANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Check_Button.H>
#include "../scene/Entity.h"
#include "../core/Types.h"

namespace Omega { namespace Editor {

/**
 * @brief لوحة خصائص الكيان المحدد باستخدام FLTK
 */
class PropertiesPanel : public Fl_Group {
public:
    PropertiesPanel(int x, int y, int w, int h);
    ~PropertiesPanel();

    // عرض خصائص كيان
    void displayEntity(Scene::Entity* entity);
    void clear();

    // حفظ التغييرات
    void applyChanges();

private:
    void createTransformSection();
    void createComponentSection(const std::string& name);
    
    static void valueChangedCallback(Fl_Widget* w, void* data);
    void onValueChanged();

    Scene::Entity* m_currentEntity;
    
    // عناصر التحكم بـ Transform
    Fl_Input* m_posX, *m_posY, *m_posZ;
    Fl_Input* m_rotX, *m_rotY, *m_rotZ;
    Fl_Input* m_scaleX, *m_scaleY, *m_scaleZ;
};

}} // namespace Omega::Editor

#endif // PROPERTIES_PANEL_H
