#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include "ui_WidgetMain.h"
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#include <QSystemTrayIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include "Log4Qt/Logger"
class TcpDownload;

class WidgetMain : public QWidget, private Ui::WidgetMain
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit WidgetMain(QWidget *parent = 0);

signals:
    void signalCloseProcess();

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void slotDownloadFinish(const QString &name);
    void slotServerInfoDone(const QString &str);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void slotNewConnection();
    void slotReadyRead();
    void slotConnected();
    void slotHideWindows();
    void slotTcpError(const QString &errorString);

    void on_pushButton_clicked();

private:
    void initSetting();
    void initData();
    void showMessage();
    void createActions();
    void createTrayIcon();
    void initLocalNetwork();
    void moveTempFileToWorkPath();
    void startMind(const QString &path);
//真应该分成及派生类, Windows一个, Linux一个, Mac一个. 回学校之后再改吧

#ifdef Q_OS_WIN32
    void updateProgram_Windows(const QString &name);
    int killProcess_Windows(const char *pClassName, const char *pTitleName);
    void overlayFile(const QString &path);
    void copyFile(const QString &name, const QString &path, QTextStream &out);
    void copyFile(const QString &name, const QString &path);

#elif defined(Q_OS_LINUX)
    void updateProgram_Linux();
#elif defined(Q_OS_MAC)
    void updateProgram_Mac();
#endif

private:
    TcpDownload *pDownload_;
    QString strLocalVersionInfo_;
    QString strServerVersionInfo_;

    /////////////////////////////////////////////
    //通知栏
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    //用于和Mind+的交互
    QLocalServer *pLocalServer_;
    QLocalSocket *pLocalSocket_;
    QString cmd_;
    QMap<QString, QString> map_name_path_;

    int updateType_; //1 立即更新 2关闭窗口时更新 3 不更新
    QPoint offset_;
};

#endif // WIDGETMAIN_H
