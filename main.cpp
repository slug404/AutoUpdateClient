#include "WidgetMain.h"
#include <QApplication>
#include <QFileInfo>
//#include "Log4Qt/logger.h"
//#include "Log4Qt/BasicConfigurator"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WidgetMain w;
    //Log4Qt::BasicConfigurator::configure();
    //Log4Qt::Logger *pLog = Log4Qt::Logger::rootLogger();
//    pLog->removeAllAppenders();

//    Log4Qt::FileAppender *pFileAppender = new Log4Qt::FileAppender();
//    pFileAppender->setName("FileAppender");
//    pFileAppender->setFile("log.txt");
//    pFileAppender->activateOptions();

//    pLog->addAppender(pFileAppender);
////    Log4Qt::TTCCLayout *pLayout = new Log4Qt::TTCCLayout(Log4Qt::TTCCLayout::DateFormat);
////    pFileAppender->setLayout(pLayout);

//    pLog->debug("hello slug!");
//    pLog->info("infor slug!");
//    pLog->warn("nimei");

    w.setWindowTitle("Mind+ Upgrading");
    w.show();

    return a.exec();
}
