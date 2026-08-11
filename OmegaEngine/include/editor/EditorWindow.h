#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTabWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

namespace Omega {

class Viewport;
class SceneHierarchy;
class PropertiesPanel;
class ProjectBrowser;
class ConsolePanel;
class Toolbar;

class EditorWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit EditorWindow(QWidget* parent = nullptr);
    ~EditorWindow();
    
    void initialize();
    void shutdown();
    
    Viewport* getViewport() { return m_viewport; }
    
private slots:
    void newProject();
    void openProject();
    void saveProject();
    void playGame();
    void stopGame();
    void pauseGame();
    
private:
    void createMenuBar();
    void createToolbars();
    void createDockWidgets();
    void setupCentralWidget();
    
    Viewport* m_viewport = nullptr;
    SceneHierarchy* m_sceneHierarchy = nullptr;
    PropertiesPanel* m_propertiesPanel = nullptr;
    ProjectBrowser* m_projectBrowser = nullptr;
    ConsolePanel* m_consolePanel = nullptr;
    Toolbar* m_toolbar = nullptr;
    
    bool m_isPlaying = false;
};

} // namespace Omega
