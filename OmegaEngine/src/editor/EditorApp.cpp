#include "editor/EditorApp.h"
#include "editor/ViewportGL.h"
#include "editor/SceneTreePanel.h"
#include "editor/PropertiesPanel.h"
#include "editor/ConsolePanel.h"
#include "editor/MainMenuBar.h"
#include "../core/Log.h"

namespace Omega { namespace Editor {

EditorApp::EditorApp(int w, int h, const char* title) 
    : Fl_Window(w, h, title), m_wantsToQuit(false),
      m_menuBar(nullptr), m_mainSplit(nullptr), m_bottomSplit(nullptr),
      m_viewport(nullptr), m_sceneTree(nullptr), m_properties(nullptr), m_console(nullptr) {
    
    this->callback(closeCallback, this);
    setupUI();
}

EditorApp::~EditorApp() {
    // تنظيف المكونات سيتم تلقائياً بواسطة FLTK
}

void EditorApp::setupUI() {
    this->begin();
    
    // شريط القوائم
    m_menuBar = new MainMenuBar(0, 0, w(), 25);
    m_menuBar->setCallback([this](const char* item) {
        OMEGA_LOG_INFO("Menu selected: {}", item);
        if (strcmp(item, "File/Exit") == 0) {
            m_wantsToQuit = true;
        }
    });
    
    // تقسيم رئيسي أفقي
    m_mainSplit = new Fl_Split(0, 25, w(), h() - 25);
    m_mainSplit->type(Fl_Split::HORIZONTAL);
    m_mainSplit->position(200);
    m_mainSplit->begin();
    
    // الجانب الأيسر: شجرة المشهد
    m_sceneTree = new SceneTreePanel(0, 25, 200, h() - 150);
    m_sceneTree->setSelectionCallback([](uint32_t entityId, void* userData) {
        auto* app = static_cast<EditorApp*>(userData);
        OMEGA_LOG_INFO("Entity selected: {}", entityId);
        // تحديث لوحة الخصائص
    }, this);
    
    // الجانب الأيمن: Viewport و Properties
    Fl_Group* rightPanel = new Fl_Group(200, 25, w() - 200, h() - 150);
    rightPanel->begin();
    
    // تقسيم عمودي للـ Viewport والـ Console
    m_bottomSplit = new Fl_Split(200, 25, w() - 400, h() - 175);
    m_bottomSplit->type(Fl_Split::VERTICAL);
    m_bottomSplit->position(h() - 200);
    m_bottomSplit->begin();
    
    // Viewport
    m_viewport = new ViewportGL(200, 25, w() - 400, h() - 200);
    
    // Console في الأسفل
    m_console = new ConsolePanel(200, h() - 200, w() - 400, 150);
    m_console->logInfo("Omega Engine Editor Started");
    
    m_bottomSplit->end();
    
    // Properties Panel على اليمين
    m_properties = new PropertiesPanel(w() - 200, 25, 200, h() - 150);
    
    rightPanel->end();
    rightPanel->resizable(m_bottomSplit);
    
    m_mainSplit->end();
    m_mainSplit->resizable(rightPanel);
    
    this->end();
    this->resizable(this);
}

void EditorApp::run() {
    this->show();
    while (!m_wantsToQuit) {
        Fl::wait(0.016); // ~60 FPS
        // تحديث المحرك هنا
    }
}

void EditorApp::closeCallback(Fl_Widget* w, void* data) {
    auto* app = static_cast<EditorApp*>(data);
    app->m_wantsToQuit = true;
}

}} // namespace Omega::Editor
