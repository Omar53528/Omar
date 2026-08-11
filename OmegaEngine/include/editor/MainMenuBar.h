#ifndef MAIN_MENU_BAR_H
#define MAIN_MENU_BAR_H

#include <FL/Fl_Menu_Bar.H>
#include <functional>

namespace Omega { namespace Editor {

/**
 * @brief شريط القوائم الرئيسي للمحرر
 */
class MainMenuBar : public Fl_Menu_Bar {
public:
    MainMenuBar(int x, int y, int w, int h);
    ~MainMenuBar();

    // تعيين استدعاءات القوائم
    using MenuCallback = std::function<void(const char* item)>;
    void setCallback(MenuCallback cb) { m_callback = cb; }

private:
    static void menuCallback(Fl_Widget* w, void* data);
    void onMenuSelected(const char* item);

    MenuCallback m_callback;
};

}} // namespace Omega::Editor

#endif // MAIN_MENU_BAR_H
