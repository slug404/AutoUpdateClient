#include "WidgetMain.h"
#include <QApplication>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WidgetMain w;
#ifdef Q_OS_MAC
    w.setWindowIcon(QIcon("v095_logo_macx.icns"));
#else
    w.setWindowIcon(QIcon("./resource/images/icon/v095_logop_4_48x48x8.png"));
    QApplication::addLibraryPath("./resource/plugins");
    QApplication::addLibraryPath("./resource/plugins/codecs/");
    QApplication::addLibraryPath("./resource/plugins/imageformats/");
    QApplication::addLibraryPath("./resource/plugins/sqldrivers/");
    QApplication::addLibraryPath("./resource/plugins/platforms/");
#endif

    w.setWindowTitle("Mind+ Upgrading");
    w.show();

    return a.exec();
}
