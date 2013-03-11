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

#include "Versions/UpdateCompare.h"
#include "Versions/XmlCompare.h"
#include "Versions/VersionCreater.h"
#include "Download/TcpDownload.h"
#include "CallMindPlus.h"

WidgetMain::WidgetMain(QWidget *parent)
    : QWidget(parent)
    , strLocalVersionInfo_("")
    , strServerVersionInfo_("")
    , pLocalServer_(NULL)
    , pLocalSocket_(NULL)
    , cmd_("")
    , updateType_(-1)
{
    setupUi(this);
    initSetting();
    initData();
    initLocalNetwork();
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
    //在这里华丽的进行覆盖的操作
    //比较方便的方式就根据不同的操作系统写不同的脚本, 也可以写递归遍历操作, 麻烦
    //updateProgram_Windows(name);
    //this->lower();
    map_name_path_ = pDownload_->getFileNameAndFilePath();
    qDebug() << "update data download finish";

    pDownload_->close();
    qDebug() << "connect to Mind+";
    pLocalSocket_->connectToServer("Mind+");
    if(!pLocalSocket_->waitForConnected())
    {
        qDebug() << "Mind+ not respond";
        moveTempFileToWorkPath();
    }
}

void WidgetMain::slotServerInfoDone(const QString &str)
{
    strServerVersionInfo_ = str;
    //UpdateCompare
    qDebug() << strServerVersionInfo_;
    XmlCompare compare;
    //connect(&compare, SIGNAL(signalCloseParent()), this, SLOT(close()));
    QList<UpdateFileInformation> listFileInfo = compare.getUpdateFileList(strLocalVersionInfo_, strServerVersionInfo_);

    qDebug() << "listFileInfo size is ::::::::::::::::" << listFileInfo.size();
    if(listFileInfo.isEmpty())
    {
        qDebug() << "listFileInfo is empty process will be close";
        qApp->exit(4);
    }

    int result = QMessageBox::information(this, tr("update informaiton"), tr("Mind+ have new version. \nDo you wanna Upgrade now? ?"), QMessageBox::Yes, QMessageBox::No);

    if(QMessageBox::Yes == result)
    {
        this->show();
        pDownload_->setUpdateFileList(listFileInfo);
    }
    else if(QMessageBox::No == result)
    {
        pDownload_->close();
        progressBar->setValue(0);
        this->close();
    }
    else
    {
        qDebug() << "error!!!";
    }
}

void WidgetMain::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            break;
        case QSystemTrayIcon::MiddleClick:
            showMessage();
            break;
        default:
            ;
    }
}

void WidgetMain::slotNewConnection()
{
    qDebug() << "have new connection";
    QLocalSocket *pLocalSocket = pLocalServer_->nextPendingConnection();
    connect(pLocalSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
}

/*!
 * \brief 根据Mind+那端发来的数据进行判断, 到底要不要更新
 */
void WidgetMain::slotReadyRead()
{
    qDebug() << "have local socket data";
    QLocalSocket *pLocalSocket = qobject_cast<QLocalSocket *>(sender());
    QByteArray tmp =  pLocalSocket->readAll();
    //Mind+那边可以选择的操作有立即更新, 下次更新(关闭时更新似乎更好)
    qDebug() << tmp;
    if(QString("Mind+ close") == QString(tmp))
    {
        //this->move(-9999,  -9999);
        this->hide();

        moveTempFileToWorkPath();
    }
}

void WidgetMain::slotConnected()
{
    qDebug() << "send update message to Mind+ (update program now)";
    QByteArray bytes;
    bytes.append("update program now");
    pLocalSocket_->write(bytes);
}

void WidgetMain::slotHideWindows()
{
    hide();
}

void WidgetMain::slotTcpError(const QString &errorString)
{
    QMessageBox::warning(this, tr("warning"), errorString);
    //this->close();
}

void WidgetMain::initSetting()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    //setWindowFlags(Qt::SubWindow);
}

#ifdef Q_WS_WIN32
void WidgetMain::updateProgram_Windows(const QString &name)
{
    //先杀死当前进程
    int result = QMessageBox::warning(this, tr("warning"), tr("Are you sure to update the program?"), QMessageBox::Ok, QMessageBox::Cancel);
    if(QMessageBox::Ok == result)
    {
        QString name = "Mind+";
        killProcess_Windows(NULL, name.toLatin1().constData());
    }
    else if(QMessageBox::Cancel == result)
    {
        logger()->debug("chose cancel!!");
        return;
    }
    else
    {
        logger()->debug("chose error!!");
        return;
    }
}

/*!
 * \brief 关闭某个程序
 * \param pClassName 要关闭的程序的类名
 * \param pTitleName 要关闭的程序的标题名 (前提是要有窗口和标题)
 * \return
 */
int WidgetMain::killProcess_Windows(const char* pClassName, const char* pTitleName)
{
    HANDLE hProcessHandle;
    ULONG nProcessID;
    HWND TheWindow;

    TheWindow = ::FindWindowA( pClassName, pTitleName );

    ::GetWindowThreadProcessId( TheWindow, &nProcessID );

    hProcessHandle = ::OpenProcess( PROCESS_TERMINATE, FALSE, nProcessID );

    ::TerminateProcess( hProcessHandle, 4 );
    //::WaitForSingleObject(hProcessHandle, 1000);    //强制等待
    ::Sleep(200);

    qDebug() << "Mind+ already close";
}

void WidgetMain::overlayFile(const QString &path)
{
    //合法性检查
    logger()->debug("start overlay file...");
    QDir dir("./");
    if(!dir.exists("DownloadTemp"))
    {
        logger()->debug("Can't find DownloadTemp directory");
        return;
    }
    dir.cd("DownloadTemp");
    if(!QFile::exists("./DownloadTemp/Mind+.exe"))
    {
        logger()->debug("can't find Mind+.exe");
        return;
    }

    //生成脚本命令
    QFile file("./Copy.bat");
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        logger()->debug("can't open file Copy.bat");
        file.close();
        return ;
    }

    QTextStream out(&file);
    out << tr("cd DownloadTemp").arg(path) << endl;
    out << tr("copy /y \"Mind+.exe\" \"%1\"").arg(path) << endl;
    file.close();

    //在这里调用命令
    QProcess::execute("./Copy.bat");    //执行成功

    //后续清理
    if(QFile::exists("Copy.bat"))
    {
        if(!QFile::remove("Copy.bat"))
        {
            logger()->debug("remove copy.bat fail");
        }
    }
}

/*!
 * \brief 把名为name的文件copy到path路径下
 * \param name 文件名
 * \param path 路径
 */
void WidgetMain::copyFile(const QString &name, const QString &path, QTextStream &out)
{
    //生成脚本命令
    QString pathWindows = QDir::toNativeSeparators(path);
    qDebug() << pathWindows;
    out << tr("copy /y \"%1\" \"%2\"").arg(name).arg(pathWindows) << endl;

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

    QFile desFile(path);
    if(!desFile.open(QFile::WriteOnly))
    {
        qDebug() << "open des file fail";
        return;
    }

    desFile.write(bytes);
    desFile.waitForBytesWritten(10);
    desFile.close();
}

#elif defined(Q_OS_LINUX)
void WidgetMain::updateProgram_Linux()
{
}
int WidgetMain::killProcess_Windows()
{
}
#elif defined(Q_OS_MAC)
void WidgetMain::updateProgram_Mac()
{
}
#endif

void WidgetMain::initData()
{
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

    VersionCreater versionCreater;
    QStringList list;
    list << +"/resource/tools";
    qDebug() << list;

    versionCreater.start(dirtmp.currentPath(), list);
    strLocalVersionInfo_ = versionCreater.getXml();
    //qDebug() << strLocalVersionInfo_;

//    QFile fileTemp("./hundan.xml");
//    if(!fileTemp.open(QFile::WriteOnly))
//    {
//        //qdebug() << "open fail 1234";
//    }

//    fileTemp.write(strLocalVersionInfo_.toAscii());

//    qDebug() << "strLocalVersionInfo_ done";
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
    //connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),     this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
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
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    //trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("auto update program"));
    trayIcon->setIcon(QIcon(QPixmap("./resource/images/AutoUpdate/ico.png").scaled(QSize(16, 16))));
}

void WidgetMain::initLocalNetwork()
{
    pLocalSocket_ = new QLocalSocket(this);
    connect(pLocalSocket_, SIGNAL(connected()), this, SLOT(slotConnected()));

    pLocalServer_ = new QLocalServer(this);
    if(!pLocalServer_->listen("AutoUpdate"))
    {
        qDebug() << pLocalServer_->errorString();
        return;
    }
    connect(pLocalServer_, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void WidgetMain::moveTempFileToWorkPath()
{
    //map_name_path_ = pDownload_->getFileNameAndFilePath();
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

    int result = QMessageBox::information(this, tr("information"), tr("The upgrade is complete. \nLaunch Mind+ now?"), QMessageBox::Yes, QMessageBox::No);
    if(QMessageBox::Yes == result)
    {
        startMind("./Mind+.exe");

        this->close();
    }
    else if(QMessageBox::No == result)
    {
        this->close();
    }
}

void WidgetMain::startMind(const QString &path)
{
    if(!QFile::exists(path))
    {
        int result = QMessageBox::warning(this, tr("warnning"), tr("Failed to find Mind+.exe in current directory. Could you manually select a directory?"), QMessageBox::Yes, QMessageBox::No);
        if(QMessageBox::Yes == result)
        {
            QString pathNow = QFileDialog::getExistingDirectory(this, tr(""), tr("."));
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
    ::Sleep(500);
}

void WidgetMain::on_pushButton_clicked()
{
    pDownload_->close();
    this->close();
}
