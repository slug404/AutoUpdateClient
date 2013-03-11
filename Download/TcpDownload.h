#ifndef TCPDOWNLOAD_H
#define TCPDOWNLOAD_H

#include "DownloadBase.h"
#include <QTcpSocket>
#include "dptr.h"
#include "Log4Qt/Logger"
#include "Versions/XmlCompare.h"

class TcpDownloadPrivate;

class TcpDownload : public DownloadBase
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum RequestType
    {
        RequestXml,
        RequestData,
        RequestExecutable
    };

    TcpDownload();
    virtual void prepare();
    virtual void request();
    virtual void handleData();
    virtual void clear();
    void close();

    void setUpdateRequestXml(const QString &str);
    void setUpdateFileList(QList<UpdateFileInformation> list);
    QMap<QString, QString> getFileNameAndFilePath() { return map_name_file_; }

signals:
    void signalBytesAvailable();
    void signalServerInfoDone(const QString &str);
    void signalDownloadProgress(int progress);
    void signalDownloadStart();
    void signalDownloadFinish(const QString &name);
    void signalTcpError(const QString &errorString);

public slots:
    void startTest();

public slots:
    void slotConnected();
    void slotError(QAbstractSocket::SocketError error);
    void slotReadyRead();

private:
    void initData();
    void handleXml(QDataStream &in);
    void handleData(QDataStream &in);
    void handleExecutable(QDataStream &in);
    void getXml();
    void getData();
    void getUpdateFilesData(QList<UpdateFileInformation> list);
    void getExecutable();

private:
    DPTR_DECLARE_PRIVATE(TcpDownload);
    qint32 blockSize_;
    QMap<QString, QString> map_name_file_;
};

#endif // TCPDOWNLOAD_H
