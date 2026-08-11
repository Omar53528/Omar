#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QMenu>

namespace Omega {

class SceneHierarchy : public QTreeWidget {
    Q_OBJECT
    
public:
    explicit SceneHierarchy(QWidget* parent = nullptr);
    
    void refresh();
    void selectEntity(int entityId);
    
signals:
    void entitySelected(int entityId);
    void entityDoubleClicked(int entityId);
    
private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void showContextMenu(const QPoint& pos);
    
private:
    QMenu* m_contextMenu;
};

} // namespace Omega
