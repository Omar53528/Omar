#include "editor/MainMenuBar.h"

namespace Omega { namespace Editor {

static Fl_Menu_Item menu_items[] = {
    {"&File", 0, 0, 0, FL_SUBMENU},
        {"&New Scene", FL_CTRL + 'n', 0, 0},
        {"&Open Scene", FL_CTRL + 'o', 0, 0},
        {"&Save Scene", FL_CTRL + 's', 0, 0},
        {"Save &As", FL_CTRL + FL_SHIFT + 's', 0, 0},
        {"E&xit", FL_ALT + FL_F4, 0, 0},
    {0},
    {"&Edit", 0, 0, 0, FL_SUBMENU},
        {"&Undo", FL_CTRL + 'z', 0, 0},
        {"&Redo", FL_CTRL + 'y', 0, 0},
        {"Cu&t", FL_CTRL + 'x', 0, 0},
        {"&Copy", FL_CTRL + 'c', 0, 0},
        {"&Paste", FL_CTRL + 'v', 0, 0},
        {"&Delete", FL_DELETE, 0, 0},
    {0},
    {"&View", 0, 0, 0, FL_SUBMENU},
        {"&2D Mode", 0, 0, 0},
        {"&3D Mode", 0, 0, 0},
        {"&Fullscreen", FL_F11, 0, 0},
    {0},
    {"&Help", 0, 0, 0, FL_SUBMENU},
        {"&Documentation", 0, 0, 0},
        {"&About Omega Engine", 0, 0, 0},
    {0},
    {0}
};

MainMenuBar::MainMenuBar(int x, int y, int w, int h) 
    : Fl_Menu_Bar(x, y, w, h), m_callback(nullptr) {
    
    this->menu(menu_items);
    this->callback(menuCallback, this);
}

MainMenuBar::~MainMenuBar() {}

void MainMenuBar::menuCallback(Fl_Widget* w, void* data) {
    auto* menu = static_cast<MainMenuBar*>(w);
    if (menu->mvalue()) {
        menu->onMenuSelected(menu->mvalue()->label());
    }
}

void MainMenuBar::onMenuSelected(const char* item) {
    if (m_callback) {
        m_callback(item);
    }
}

}} // namespace Omega::Editor
