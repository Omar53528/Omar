#pragma once
#include <QtWidgets/QTreeView>
namespace Omega {
class ProjectBrowser : public QTreeView {
    Q_OBJECT
public:
    explicit ProjectBrowser(QWidget* parent = nullptr);
    void loadProject(const std::string& path);
private:
    std::string m_projectPath;
};
}
