#include "WidgetMain.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QFileDialog>
#include <QFileInfo>
#include <QThread>
#include <QCloseEvent>
#include <QMenu>
#include <QTimer>
#include <QFileDialog>
#include <QPainter>
#include <QThread>
#include <QApplication>
#include <QSettings>

#include "Versions/UpdateCompare.h"
#include "Versions/XmlCompare.h"
#include "Versions/VersionCreater.h"
#include "Download/TcpDownload.h"
#include "CallMindPlus.h"

WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , strLocalVersionInfo_("")
    , strServerVersionInfo_("")
{
    setupUi(this);
    initSetting();
    initData();

    QTimer::singleShot(1, this, SLOT(slotHideWindows()));
}

void WidgetMain::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            retranslateUi(this);
            break;
        default:
            break;
    }
}

void WidgetMain::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void WidgetMain::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QWidget *pParent_ = static_cast<QWidget *>(this->parent());
        if(NULL == pParent_)
        {
            return;
        }
        offset_ = event->globalPos() - pParent_->frameGeometry().topLeft();
    }
}

void WidgetMain::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    {
        return;
    }
    if ((event->pos() - offset_).manhattanLength()
            < QApplication::startDragDistance())
    {
        return;
    }
}

void WidgetMain::slotDownloadFinish(const QString &name)
{
    //下载完了做后续处理
    map_name_path_ = pDownload_->getFileNameAndFilePath();
    {
        //保存文件路径信息, 下次更新的时候需要
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out << map_name_path_;
        QFile file("./DownloadTemp/filePath.dat");
        if(!file.open(QFile::WriteOnly))
        {
            qDebug() << "open file fail : " << "./DownloadTemp/filePath.dat";
        }
        file.write(bytes);
    }
    qDebug() << "update data download finish";

    pDownload_->close();

    QSettings setting("./resource/setting.ini", QSettings::IniFormat);
    setting.setValue("Normal/update", true);
    //qApp->exit(4);
    this->close();
}

void WidgetMain::slotServerInfoDone(const QString &str)
{
    strServerVersionInfo_ = str;
    qDebug() << "start scan local files";
    /////////////////////////////////////////////////
    /// \brief 发布时注释
//    ////
//    {
//        QFile file("server.xml");
//        if(!file.open(QFile::WriteOnly))
//        {
//            qDebug() << "server.xml can't open! ";
//        }
//        file.write(strServerVersionInfo_.toUtf8());
//        file.waitForBytesWritten(1000);
//        file.close();
//    }
    /////////////////////////////////////////////////////

    XmlCompare compare;
    QList<UpdateFileInformation> listFileInfo = compare.getUpdateFileList(strLocalVersionInfo_, strServerVersionInfo_);

    qDebug() << "listFileInfo size is ::::::::::::::::" << listFileInfo.size();
    if(listFileInfo.isEmpty())
    {
        qDebug() << "listFileInfo is empty process will be close";
        this->close();
    }
    else
    {
        pDownload_->setUpdateFileList(listFileInfo);
    }
}

void WidgetMain::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::MiddleClick:
            this->showNormal();
            break;
        default:
            ;
    }
}

void WidgetMain::slotHideWindows()
{
    hide();
}

void WidgetMain::slotTcpError(const QString &errorString)
{
    QMessageBox::warning(this, tr("warning"), errorString);
    this->close();
}

void WidgetMain::initSetting()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void WidgetMain::copyFile(const QString &name, const QString &path)
{
    qDebug() << "handle :" << tr("./DownloadTemp") + "/" + name;
    QFile file(tr("./DownloadTemp") + "/" + name);
    if(!file.open(QFile::ReadOnly))
    {
        qDebug() << "open src file fail";
        return;
    }

    QByteArray bytes =  file.readAll();
    file.close();

    if(QFile::exists(tr("./DownloadTemp") + "/" + name))
    {
        qDebug() << "if exite it will be delete";
        if(!QFile::remove(tr("./DownloadTemp") + "/" + name))
        {
            qDebug() << "delete fail: " << name;
        }
    }

    ////////////////////////////////
    //在这里要确保path可用
    {
        QDir pathTest("./");
        QString oldPath = pathTest.currentPath();

        pathTest.setPath(path);
        if(!pathTest.exists())
        {
            qDebug() << "not exists the path, and it will try to cerate it:" << path;
            if(!pathTest.mkpath(path))
            {
                qDebug() << "create path fail!!!!";
            }
        }
        pathTest.setPath(oldPath);
    }
    ////////////////////////////////

    QFile desFile(path + "/" + name);
    if(!desFile.open(QFile::WriteOnly))
    {
        qDebug() << "open des file fail";
        return;
    }

    desFile.write(bytes);
    desFile.waitForBytesWritten(10);
    desFile.close();
}

void WidgetMain::initData()
{
    QSettings setting("./resource/setting.ini", QSettings::IniFormat);
    bool bUpdate = setting.value("Normal/update").toBool();
    if(bUpdate)
    {
        //更新文件
        qDebug() << "start to copy update file";
        //在这之前需要文件放在哪的信息
        QMap<QString, QString> map_file_path;
        {
            QFile file("./DownloadTemp/filePath.dat");
            if(!file.open(QFile::ReadOnly))
            {
                qDebug() << "open filePath.dat file";
                qDebug() << file.errorString();
            }
            QByteArray bytes = file.readAll();
            QDataStream in(&bytes, QIODevice::ReadOnly);

            in >> map_file_path;
            if(map_file_path.isEmpty())
            {
                qDebug() << "load filePath.dat error";
            }

        }
        moveTempFileToWorkPath(map_file_path);
        setting.setValue("Normal/update", false);
    }
    else
    {
        qDebug() << "don't need to update file, and start Mind+";
    }
#ifdef Q_OS_WIN32
	startMind("./mp.exe");
#elif defined(Q_OS_LINUX)
	startMind("./mp");
#elif defined(Q_OS_MAC)
	startMind("./mp.app");
#endif
    //检查本地版本以及服务器版本.

    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(0);

    ////////////////////////////////////////////////////////////////////////////
    //清理TempDownLoad中的垃圾文件
    QDir dirDelete("./DownloadTemp");
    dirDelete.setFilter(QDir::Files);
    foreach (const QString str, dirDelete.entryList())
    {
        if(QFile::remove(tr("./DownloadTemp/") + str))
        {
            qDebug() << "./DownloadTemp/" << str << "       fail!";
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    //本地文件的计算
    QDir dirtmp("./");
    qDebug() << "get local files version information";
    VersionCreater versionCreater;
    versionCreater.start(dirtmp.currentPath());
    strLocalVersionInfo_ = versionCreater.getXml();
    //qDebug() << strLocalVersionInfo_;

//    {
//        QFile fileTemp("./client.xml");
//        if(!fileTemp.open(QFile::WriteOnly))
//        {
//            //qdebug() << "open fail 1234";
//        }

//        fileTemp.write(strLocalVersionInfo_.toAscii());
//        fileTemp.close();
//    }
    //qDebug() << "strLocalVersionInfo_ done";
    //map_name_path_ = versionCreater.getFilePath();
    /////////////////////////////////////////////////////////////////////////////

    pDownload_ = new TcpDownload();
    connect(pDownload_, SIGNAL(signalDownloadProgress(int)), progressBar, SLOT(setValue(int)));
    connect(pDownload_, SIGNAL(signalDownloadFinish(QString)), this, SLOT(slotDownloadFinish(QString)));
    connect(pDownload_, SIGNAL(signalServerInfoDone(QString)), this, SLOT(slotServerInfoDone(QString)));
    connect(pDownload_, SIGNAL(signalTcpError(QString)), this, SLOT(slotTcpError(QString)));
    pDownload_->prepare();

    //通知栏
    createActions();
    createTrayIcon();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),     this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
}

void WidgetMain::showMessage()
{
    trayIcon->showMessage(tr("title"), tr("show the message"));
}

void WidgetMain::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void WidgetMain::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    //trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("auto update program"));
    trayIcon->setIcon(QIcon(QPixmap("./resource/images/AutoUpdate/ico.png").scaled(QSize(16, 16))));
}

void WidgetMain::moveTempFileToWorkPath()
{
    qDebug() << "start copy files";
    QDir dir("./DownloadTemp");
    dir.setFilter(QDir::Files);
    QStringList list = dir.entryList();

    for(int i = 0; i != list.size(); ++i)
    {
        QString name = list.at(i);
        if(map_name_path_.contains(name))
        {
            QString path = map_name_path_.value(name);
            qDebug() << name << path;
            copyFile(name, path);
        }
        else
        {
            qDebug() << "map_name_path_ not contains " << name;
        }
    }
}

void WidgetMain::moveTempFileToWorkPath(QMap<QString, QString> &map_name_path)
{
    qDebug() << "start copy files";
    QDir dir("./DownloadTemp");
    dir.setFilter(QDir::Files);
    QStringList list = dir.entryList();

    for(int i = 0; i != list.size(); ++i)
    {
        QString name = list.at(i);
        if(map_name_path.contains(name))
        {
            QString path = map_name_path.value(name);
            qDebug() << name << path;
            copyFile(name, path);
        }
        else
        {
            qDebug() << "map_name_path not contains " << name;
        }
    }
}

void WidgetMain::startMind(const QString &path)
{
#ifdef Q_OS_MAC
    if(!QFile::exists(path))
#else
    if(!QFile::exists(path))
#endif
    {
		int result = QMessageBox::warning(this, tr("warnning"), tr("Failed to find mp in current directory. Could you manually select a directory?"), QMessageBox::Yes, QMessageBox::No);
        if(QMessageBox::Yes == result)
        {
            QString pathNow = QFileDialog::getOpenFileName(this, tr(""), tr("."));
            startMind(pathNow);
        }
        else
        {
            return;
        }
    }
    //另外一个线程中去阻塞调用
    QThread *pThread = new QThread(this);
    CallMindPlus *pCallMindPlus = new CallMindPlus(path);
    connect(pThread, SIGNAL(started()), pCallMindPlus, SLOT(slotCallMindPlus()));
    pCallMindPlus->moveToThread(pThread);

    pThread->start();
}

void WidgetMain::on_pushButton_clicked()
{
    pDownload_->close();
    this->close();
}
