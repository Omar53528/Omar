#include "editor/ProjectBrowser.h"
#include <QFileSystemModel>
namespace Omega {
ProjectBrowser::ProjectBrowser(QWidget* parent) : QTreeView(parent) {
    auto* model = new QFileSystemModel(this);
    model->setRootPath("");
    setModel(model);
    hideColumn(1); hideColumn(2); hideColumn(3);
}
void ProjectBrowser::loadProject(const std::string& path) {
    m_projectPath = path;
    setRootIndex(model()->index(QString::fromStdString(path)));
}
}
