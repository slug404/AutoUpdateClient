#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include "ui_WidgetMain.h"
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
#include <QSystemTrayIcon>
#include <QLocalServer>
#include <QLocalSocket>

class TcpDownload;

class WidgetMain : public QWidget, private Ui::WidgetMain
{
    Q_OBJECT

public:
    explicit WidgetMain(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void slotDownloadFinish(const QString &name);
    void slotServerInfoDone(const QString &str);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void slotHideWindows();
    void slotTcpError(const QString &errorString);

    void on_pushButton_clicked();

private:
    void initSetting();
    void initData();
    void showMessage();
    void createActions();
    void createTrayIcon();

    void moveTempFileToWorkPath();
    void moveTempFileToWorkPath(QMap<QString, QString> &map_name_path);
	void startMind(const QString &path);
//真应该分成及派生类, Windows一个, Linux一个, Mac一个. 回学校之后再改吧

	void copyFile(const QString &name, const QString &path);

private:
    TcpDownload *pDownload_;
    QString strLocalVersionInfo_;
    QString strServerVersionInfo_;
	QMap<QString, QString> map_name_path_;
	QPoint offset_;
    /////////////////////////////////////////////
    //通知栏
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // WIDGETMAIN_H
