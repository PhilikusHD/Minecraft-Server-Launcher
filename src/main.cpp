#include <QApplication>
#include "Server/Launcher/ServerLauncher.h"

using namespace MCSL;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ServerLauncher launcher;
    launcher.show();

    return app.exec();
}
