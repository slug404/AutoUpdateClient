#include "TcpDownload.h"
#include "TcpDownload_p.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QTcpSocket>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>

TcpDownload::TcpDownload()
    : DownloadBase(*new TcpDownloadPrivate)
    , blockSize_(0)
{
    initData();
}

void TcpDownload::initData()
{
    DPTR_D(TcpDownload);
    d.pTcpSocket_ = new QTcpSocket(this);
    connect(d.pTcpSocket_, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(d.pTcpSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(d.pTcpSocket_, SIGNAL(readyRead()), this, SLOT(slotReadyRead()), Qt::QueuedConnection);
}

void TcpDownload::handleXml(QDataStream &in)
{
    QString buffer;
    in >> buffer;
    //qDebug() << (buffer);

//    QFile file("requestXml.xml");
//    if(!file.open(QFile::WriteOnly))
//    {
//        qDebug() << "requestXml.xml open fail!";
//        return;
//    }
//    file.write(buffer.toUtf8());
    emit signalServerInfoDone(buffer);
}

/*!
 * \brief 请求服务端版本信息
 */
void TcpDownload::getXml()
{
    //在这里发送响应
    DPTR_D(TcpDownload);
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    //为了防止不同平台数据大小不一致, 全部指定用qint32和qint64
    //报文大小->socket descriptor-> request type -> data
    out << (qint32)0;      //占位
    out << (qint32)RequestXml;

    out.device()->seek(0);
    out << (qint32)(bytes.size() - (qint32)sizeof(qint32));
    d.pTcpSocket_->write(bytes.data(), bytes.size());
}

/*!
 * \brief 请求更新数据
 */
void TcpDownload::getData()
{
    //在这里发送响应
    DPTR_D(TcpDownload);
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    //为了防止不同平台数据大小不一致, 全部指定用qint32和qint64
    //报文大小->socket descriptor-> request type -> data
    out << (qint32)0;      //占位
    out << (qint32)RequestData;
    if(d.requestXml_.isEmpty())
    {
        qDebug() << ("requestXml_ is empty");
    }

    out.device()->seek(0);
    out << (qint32)(bytes.size() - (qint32)sizeof(qint32));
    d.pTcpSocket_->write(bytes.data(), bytes.size());
}

/*!
 * \brief 向服务端发送请求需要更新的文件列表(为了代替之前的getData, 把计算量尽量做在客户端)
 * \param list 要更新的文件列表, 其中的每一个元素包含一个文件名还有对应的路径
 */
void TcpDownload::getUpdateFilesData(QList<UpdateFileInformation> list)
{
    qDebug() << "start to request update file list";
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    //为了防止不同平台数据大小不一致, 全部指定用qint32和qint64
    //报文大小->socket descriptor-> request type -> data
    out << (qint32)0;      //占位
    out << (qint32)RequestData;
    out << (qint32)list.size();      //通知服务端有多少个要更新的文件

    qDebug() << "update files number : " << list.size();

    for(int i = 0; i != list.size(); ++i)
    {
       UpdateFileInformation tmp =  list.at(i);
        out << tmp.name;
        qDebug() << tmp.name;
    }

    out.device()->seek(0);
    out << (qint32)(bytes.size() - (qint32)sizeof(qint32));

    qDebug() << "bytes size is" << bytes.size();
    DPTR_D(TcpDownload);
    d.pTcpSocket_->write(bytes.data(), bytes.size());
}

/*!
 * \brief 处理更新数据
 * \param in
 */
void TcpDownload::handleData(QDataStream &in)
{
    //这里做后续的处理
    int num;
    in >> num;
    QDir tmpDir("./");
    qDebug()<<"need to download file : " << num;
    if(!tmpDir.exists("DownloadTemp"))
    {
        tmpDir.mkdir("DownloadTemp");
    }
    while (num--)
    {
        QString name;
        QByteArray bytes;
        in >> name >> bytes;

        QString filePath = tr("./DownloadTemp/%1").arg(name);
        if(QFile::exists(filePath))
        {
            qDebug() << "delete alrealy exist file";
            //QFile::remove(filePath);
        }
        QFile file(filePath);
        if(!file.open(QFile::WriteOnly))
        {
            continue;
        }

        file.write(bytes);
        qDebug() << "download file:" << name;
    }

    QByteArray serialize;
    in >> serialize;
    QDataStream serializeIn(&serialize, QIODevice::ReadOnly);
    QMap<QString, QString> map_name_file;
    serializeIn >> map_name_file;
    map_name_file_ = map_name_file;
    //qDebug() << map_name_file_;

    DPTR_D(TcpDownload);
    d.pTcpSocket_->close();
    emit signalDownloadFinish("");
}

/*!
 * \brief 请求exe文件
 */
void TcpDownload::getExecutable()
{
    //在这里发送响应
    DPTR_D(TcpDownload);
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    out << (qint32)0;      //占位
    out << (qint32)RequestExecutable;

    out.device()->seek(0);
    out << (qint32)(bytes.size() - (qint32)sizeof(qint32));
    d.pTcpSocket_->write(bytes.data(), bytes.size());
}

void TcpDownload::handleExecutable(QDataStream &in)
{
    QString name;
    in >> name;
    qDebug() << (QString("name: %1").arg(name));
    QByteArray bytes;
    in >> bytes;
    qDebug() << (tr("size of bytes :") + QString::number(bytes.size()));
    QByteArray md5;
    in >> md5;
    qDebug() << (QString(md5));
    QByteArray tmp = QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    qDebug() << (QString(tmp));
    if(md5 != tmp)
    {
        qDebug() << ("data error!");
    }

    QDir tmpDir("./");
    if(!tmpDir.exists("DownloadTemp"))
    {
        tmpDir.mkdir("DownloadTemp");
    }
    QFile file(tr("./DownloadTemp/%1").arg(name));
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << (tr("can't open file ") + name);
    }
    file.write(bytes);
    file.close();

    emit signalDownloadFinish(name);
}

void TcpDownload::prepare()
{
    DPTR_D(TcpDownload);
    emit signalDownloadStart();
    qDebug() << "connect to server";

#ifdef Q_OS_WIN32
    d.pTcpSocket_->connectToHost("mindplus.cc", 8769);
#elif defined(Q_OS_LINUX)
    d.pTcpSocket_->connectToHost("mindplus.cc", 8770);
#elif defined(Q_OS_MAC)
    d.pTcpSocket_->connectToHost("mindplus.cc", 8771);
#endif
}

void TcpDownload::request()
{
    getXml();
}

void TcpDownload::handleReadyRead()
{
    qDebug() << "start handle tcp datagram";
    qDebug() << ("logger : tart handle tcp datagram");
    DPTR_D(TcpDownload);
    QDataStream in(d.pTcpSocket_);
    in.setVersion(QDataStream::Qt_4_8);

    if(0 == blockSize_)
    {
        if(d.pTcpSocket_->bytesAvailable() < (qint32)sizeof(qint32))
        {
            return;
        }
        in  >> blockSize_;
    }

    qint32 currentSize = d.pTcpSocket_->bytesAvailable();
    qDebug() << "blockSize_:"  << blockSize_ << "currentSize:" << currentSize;
    emit signalDownloadProgress((currentSize- (qint32)sizeof(qint32))*100 / blockSize_);

    if(currentSize < blockSize_)
    {
        return;
    }

    int type;
    in >> type;
    emit signalDownloadProgress(100);
    switch (type)
    {
        case RequestXml:
        {
            handleXml(in);
            break;
        }
        case RequestData:
        {
            handleData(in);
            break;
        }
        case RequestExecutable:
        {
            handleExecutable(in);
            break;
        }
        default:
        {
            qDebug() << "error data type";
            break;
        }
    }

    blockSize_ = 0;
}

void TcpDownload::clear()
{
    DPTR_D(TcpDownload);
    d.pTcpSocket_->close();
}
/*!
 * \brief 关闭Tcp连接
 */
void TcpDownload::close()
{
    DPTR_D(TcpDownload);
    if(d.pTcpSocket_)
    {
        d.pTcpSocket_->close();
    }
}

/*!
 * \brief 设置更新文件的请求列表, 然后从服务端请求文件
 * \param dom树的string描述
 */
void TcpDownload::setUpdateRequestXml(const QString &str)
{
    DPTR_D(TcpDownload);
    d.requestXml_ = str;

    //发送更新请求, 让服务端返回更新数据
    getData();
}

void TcpDownload::setUpdateFileList(QList<UpdateFileInformation> list)
{
    getUpdateFilesData(list);
}

void TcpDownload::slotConnected()
{
    qDebug() << "connected !!";
    request();
}

void TcpDownload::slotError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    qDebug() << "tcp socket error!!";
    DPTR_D(TcpDownload);
    qDebug() << d.pTcpSocket_->errorString();
    emit signalTcpError(d.pTcpSocket_->errorString());
}

void TcpDownload::slotReadyRead()
{
    handleReadyRead();
}

void TcpDownload::startTest()
{
    prepare();
}
