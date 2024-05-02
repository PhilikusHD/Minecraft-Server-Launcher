#pragma once

#include <qwidget.h>
#include <qline.h>
#include <qlineedit.h>
#include <qpushbutton.h>

namespace MCSL 
{
    class ServerLauncher : public QWidget
    {
        Q_OBJECT

    public:
        ServerLauncher(QWidget *parent = nullptr);

    private slots:
        void LaunchServer();
    private:
        QLineEdit *m_VersionLineEdit;
        QLineEdit *m_NameLineEdit;
        QPushButton *m_LaunchButton;
    };

}
