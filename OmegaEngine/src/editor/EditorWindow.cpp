#include "editor/EditorWindow.h"
#include "editor/Viewport.h"
#include "editor/SceneHierarchy.h"
#include "editor/PropertiesPanel.h"
#include "editor/ProjectBrowser.h"
#include "editor/ConsolePanel.h"
#include "editor/Toolbar.h"
#include "core/Log.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

namespace Omega {

EditorWindow::EditorWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Omega Engine Editor");
    resize(1600, 900);
}

EditorWindow::~EditorWindow() {
    shutdown();
}

void EditorWindow::initialize() {
    createMenuBar();
    createToolbars();
    createDockWidgets();
    setupCentralWidget();
    
    Log::info("Editor Window initialized");
}

void EditorWindow::shutdown() {
    if (m_viewport) m_viewport->shutdown();
    Log::info("Editor Window shutdown");
}

void EditorWindow::createMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    
    // File Menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New Project", this, &EditorWindow::newProject, QKeySequence::New);
    fileMenu->addAction("&Open Project", this, &EditorWindow::openProject, QKeySequence::Open);
    fileMenu->addAction("&Save Project", this, &EditorWindow::saveProject, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", qApp, &QApplication::quit, QKeySequence::Quit);
    
    // Edit Menu
    QMenu* editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Undo", QKeySequence::Undo);
    editMenu->addAction("&Redo", QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction("Cu&t", QKeySequence::Cut);
    editMenu->addAction("&Copy", QKeySequence::Copy);
    editMenu->addAction("&Paste", QKeySequence::Paste);
    
    // Game Menu
    QMenu* gameMenu = menuBar->addMenu("&Game");
    gameMenu->addAction("&Play", this, &EditorWindow::playGame, QKeySequence(Qt::CTRL | Qt::Key_P));
    gameMenu->addAction("P&ause", this, &EditorWindow::pauseGame, QKeySequence(Qt::Key_Pause));
    gameMenu->addAction("S&top", this, &EditorWindow::stopGame, QKeySequence(Qt::CTRL | Qt::Key_S));
    
    // View Menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("Toggle Fullscreen", []() {
        // Toggle fullscreen logic
    });
    
    // Help Menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&Documentation", []() {
        // Open documentation
    });
    helpMenu->addAction("&About Omega Engine", []() {
        QMessageBox::about(nullptr, "About Omega Engine",
            "Omega Engine v1.0\n"
            "A powerful 2D/3D game engine\n"
            "Built with C++ and Qt");
    });
}

void EditorWindow::createToolbars() {
    m_toolbar = new Toolbar(this);
    addToolBar(Qt::TopToolBarArea, m_toolbar);
}

void EditorWindow::createDockWidgets() {
    // Scene Hierarchy
    m_sceneHierarchy = new SceneHierarchy(this);
    QDockWidget* hierarchyDock = new QDockWidget("Scene Hierarchy", this);
    hierarchyDock->setWidget(m_sceneHierarchy);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);
    
    // Properties Panel
    m_propertiesPanel = new PropertiesPanel(this);
    QDockWidget* propertiesDock = new QDockWidget("Properties", this);
    propertiesDock->setWidget(m_propertiesPanel);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);
    
    // Project Browser
    m_projectBrowser = new ProjectBrowser(this);
    QDockWidget* projectDock = new QDockWidget("Project", this);
    projectDock->setWidget(m_projectBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, projectDock);
    
    // Console Panel
    m_consolePanel = new ConsolePanel(this);
    QDockWidget* consoleDock = new QDockWidget("Console", this);
    consoleDock->setWidget(m_consolePanel);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
}

void EditorWindow::setupCentralWidget() {
    m_viewport = new Viewport(this);
    setCentralWidget(m_viewport);
}

void EditorWindow::newProject() {
    // Create new project dialog
    Log::info("Creating new project");
}

void EditorWindow::openProject() {
    // Open project dialog
    Log::info("Opening project");
}

void EditorWindow::saveProject() {
    // Save project
    Log::info("Saving project");
}

void EditorWindow::playGame() {
    m_isPlaying = true;
    Log::info("Game started");
    emit playGame();
}

void EditorWindow::stopGame() {
    m_isPlaying = false;
    Log::info("Game stopped");
}

void EditorWindow::pauseGame() {
    m_isPlaying = !m_isPlaying;
    Log::info(m_isPlaying ? "Game paused" : "Game resumed");
}

} // namespace Omega
