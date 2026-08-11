#include "editor/SceneHierarchy.h"
#include <QtWidgets/QAction>

namespace Omega {

SceneHierarchy::SceneHierarchy(QWidget* parent) : QTreeWidget(parent) {
    setHeaderLabel("Scene");
    setContextMenuPolicy(Qt::CustomContextMenu);
    
    connect(this, &QTreeWidget::itemClicked, this, &SceneHierarchy::onItemClicked);
    connect(this, &QTreeWidget::itemDoubleClicked, this, &SceneHierarchy::onItemDoubleClicked);
    connect(this, &QTreeWidget::customContextMenuRequested, this, &SceneHierarchy::showContextMenu);
    
    // Setup context menu
    m_contextMenu = new QMenu(this);
    QAction* createEmpty = m_contextMenu->addAction("Create Empty");
    QAction* create3DObj = m_contextMenu->addAction("Create 3D Object");
    QAction* create2DSprite = m_contextMenu->addAction("Create 2D Sprite");
    QAction* createCamera = m_contextMenu->addAction("Create Camera");
    QAction* createLight = m_contextMenu->addAction("Create Light");
    
    connect(createEmpty, &QAction::triggered, []() { /* Create empty entity */ });
    connect(create3DObj, &QAction::triggered, []() { /* Create 3D object */ });
    connect(create2DSprite, &QAction::triggered, []() { /* Create 2D sprite */ });
    connect(createCamera, &QAction::triggered, []() { /* Create camera */ });
    connect(createLight, &QAction::triggered, []() { /* Create light */ });
}

void SceneHierarchy::refresh() {
    clear();
    // Will be populated from scene data
}

void SceneHierarchy::selectEntity(int entityId) {
    // Find and select entity by ID
}

void SceneHierarchy::onItemClicked(QTreeWidgetItem* item, int column) {
    int entityId = item->data(0, Qt::UserRole).toInt();
    emit entitySelected(entityId);
}

void SceneHierarchy::onItemDoubleClicked(QTreeWidgetItem* item, int column) {
    int entityId = item->data(0, Qt::UserRole).toInt();
    emit entityDoubleClicked(entityId);
}

void SceneHierarchy::showContextMenu(const QPoint& pos) {
    m_contextMenu->exec(mapToGlobal(pos));
}

} // namespace Omega
