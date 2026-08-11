#include "editor/Toolbar.h"
#include <QToolButton>
namespace Omega {
Toolbar::Toolbar(QWidget* parent) : QToolBar("Main Toolbar", parent) {
    setMovable(false);
    addWidget(new QLabel("Omega Engine"));
    addSeparator();
    QToolButton* playBtn = new QToolButton(); playBtn->setText("▶ Play");
    QToolButton* pauseBtn = new QToolButton(); pauseBtn->setText("⏸ Pause");
    QToolButton* stopBtn = new QToolButton(); stopBtn->setText("⏹ Stop");
    addWidget(playBtn); addWidget(pauseBtn); addWidget(stopBtn);
    connect(playBtn, &QToolButton::clicked, this, &Toolbar::playClicked);
    connect(pauseBtn, &QToolButton::clicked, this, &Toolbar::pauseClicked);
    connect(stopBtn, &QToolButton::clicked, this, &Toolbar::stopClicked);
}
}
