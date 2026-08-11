#ifndef EDITOR_APP_H
#define EDITOR_APP_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Split.H>
#include <string>

namespace Omega { namespace Editor {

class ViewportGL;
class SceneTreePanel;
class PropertiesPanel;
class ConsolePanel;
class MainMenuBar;

/**
 * @brief النافذة الرئيسية لمحرر Omega Engine باستخدام FLTK
 */
class EditorApp : public Fl_Window {
public:
    EditorApp(int w, int h, const char* title = "Omega Engine Editor");
    ~EditorApp();

    void run();
    bool wantsToQuit() const { return m_wantsToQuit; }

    // الوصول للمكونات
    ViewportGL* getViewport() { return m_viewport; }
    SceneTreePanel* getSceneTree() { return m_sceneTree; }
    PropertiesPanel* getProperties() { return m_properties; }
    ConsolePanel* getConsole() { return m_console; }

private:
    void setupUI();
    static void closeCallback(Fl_Widget* w, void* data);

    bool m_wantsToQuit;
    
    // مكونات الواجهة
    Fl_Menu_Bar* m_menuBar;
    Fl_Split* m_mainSplit;
    Fl_Split* m_bottomSplit;
    
    ViewportGL* m_viewport;
    SceneTreePanel* m_sceneTree;
    PropertiesPanel* m_properties;
    ConsolePanel* m_console;
};

}} // namespace Omega::Editor

#endif // EDITOR_APP_H
