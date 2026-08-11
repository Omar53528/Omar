#pragma once
#include <QToolBar>
namespace Omega {
class Toolbar : public QToolBar {
    Q_OBJECT
public:
    explicit Toolbar(QWidget* parent = nullptr);
signals:
    void playClicked();
    void pauseClicked();
    void stopClicked();
};
}
