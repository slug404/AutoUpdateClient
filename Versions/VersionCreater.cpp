#include "VersionCreater.h"
#include "VersionCreater_p.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QStringList>
#include <QDomProcessingInstruction>
#include <QCryptographicHash>
#include <QDateTime>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>

VersionCreater::VersionCreater()
    : VersionBase(*new VersionCreaterPrivate)
{
}

VersionCreater::~VersionCreater()
{
}

QString VersionCreater::getXml()
{
    DPTR_D(VersionCreater);
    QString str = d.pDocument->toString();
    return str;
}

VersionCreater::VersionCreater(VersionCreaterPrivate &data)
    : VersionBase(data)
{
}

void VersionCreater::initDomTree()
{
    DPTR_D(VersionCreater);
    d.pDocument = new QDomDocument();
    QDomProcessingInstruction instruction = d.pDocument->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    d.pDocument->appendChild(instruction);
    QDomElement updateElement = d.pDocument->createElement("update");
    d.pDocument->appendChild(updateElement);

    QDomElement fileElement = d.pDocument->createElement("files");
    QDomElement ipAddressElement = d.pDocument->createElement("serverAddress");
    updateElement.appendChild(fileElement);
    updateElement.appendChild(ipAddressElement);

    d.pUpdateNode = new QDomNode(d.pDocument->firstChildElement("update"));
    d.pFilesNode_ = new QDomNode(d.pUpdateNode->firstChildElement("files"));
    d.pIpAddressNode_= new QDomNode(d.pUpdateNode->firstChildElement("serverAddress"));
    QDomText ipText = d.pDocument->createTextNode(getHostIp());
    //QDomText ipText = d.pDocument->createTextNode(getLocalIpAddress());
    d.pIpAddressNode_->appendChild(ipText);
    //qDebug()<< "ip address: " <<getLocalIpAddress();

    //    qDebug() << d.pUpdateNode->nodeName();
    //    qDebug() << d.pFilesNode_->nodeName();
    //    qDebug() << d.pIpAddressNode_->nodeName();
}

void VersionCreater::traveDomTree(const QString &str)
{
    //好了明天的工作就是开始遍历数据, 然后生成xml
    //先处理files
    QDir dir(str);
    dir.setFilter(QDir::Files);
    if(!dir.entryList().isEmpty())
    {
        foreach (QString fileName, dir.entryList())
        {
            QString path = str + dir.separator() + fileName;
            QFile file(path);
            if(!file.open(QFile::ReadOnly))
            {
                qDebug() << "can't open file:" << path;
                continue;
            }
            DPTR_D(VersionCreater);

            if("MindUpgrader.exe" == fileName)
            {
                continue;
            }
            else if("Thumbs.db" == fileName)
            {
                continue;
            }

            QDomNode filesNode = d.pDocument->createElement("file");
            d.pFilesNode_->appendChild(filesNode);

            //<name></name>
            QDomNode fileNameNode = d.pDocument->createElement("name");
            QDomText fileNameText = d.pDocument->createTextNode(fileName);
            fileNameNode.appendChild(fileNameText);
            filesNode.appendChild(fileNameNode);

            //<path></path>
            QDomNode filePathNode = d.pDocument->createElement("path");
            QDomText filePathText = d.pDocument->createTextNode(QDir::toNativeSeparators(path));
            filePathNode.appendChild(filePathText);
            filesNode.appendChild(filePathNode);

            //<md5></md5>
            QDomNode fileMD5Node = d.pDocument->createElement("md5");
            QDomText fileMD5Text = d.pDocument->createTextNode(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex());
            fileMD5Node.appendChild(fileMD5Text);
            filesNode.appendChild(fileMD5Node);

            //<lastModify></lastModify>
            QDomNode fileLastModifyNode = d.pDocument->createElement("lastModify");
            QDomText fileLastModifyText = d.pDocument->createTextNode(QFileInfo(path).lastModified().toString("yyyy-MM-dd,hh:mm"));
            fileLastModifyNode.appendChild(fileLastModifyText);
            filesNode.appendChild(fileLastModifyNode);
        }
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if(dir.entryList().isEmpty())
    {
        return;
    }
    else
    {
        //然后继续遍历子目录
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (QString dirName, dir.entryList())
        {
            QString path = str + dir.separator() + dirName;
            traveDomTree(path);
        }
    }
}

QString VersionCreater::getLocalIpAddress() const
{
    QString ipAddress = "";

#ifdef _WIN32
    QHostInfo localHostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> ipAddressList = localHostInfo.addresses();
#else
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
#endif
    for(int i = 0; i < ipAddressList.size(); ++i)
    {
        if(!ipAddressList.at(i).isNull() &&
                ipAddressList.at(i) != QHostAddress::LocalHost &&
                ipAddressList.at(i).protocol() ==  QAbstractSocket::IPv4Protocol)
        {
            ipAddress = ipAddressList.at(i).toString();
            break;
        }
    }

    return ipAddress;
}

QString VersionCreater::getHostIp()
{
    qWarning()<<__FUNCTION__;
    QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> hostNameLookupAddressList = hostInfo.addresses();
    QList<QHostAddress> interfaceAddressList = QNetworkInterface::allAddresses();
    qDebug()<<__FUNCTION__<<"hostName lookup addresses:"<<hostNameLookupAddressList;
    qDebug()<<__FUNCTION__<<"interface addresses:"<<interfaceAddressList;

    QString hostIpStr;
    foreach(QHostAddress addr, hostNameLookupAddressList)
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol && interfaceAddressList.contains(addr))
        {
            if(isLocalIp(addr))
            {
                qDebug()<<__FUNCTION__<<addr<<" is local ip";
                hostIpStr = addr.toString();
                break;
            }
            else if(isLinkLocalAddress(addr))
            {
                qDebug()<<__FUNCTION__<<addr<<" is Link Local Address";
                hostIpStr = addr.toString();
            }
        }
    }

    return hostIpStr;
}

/*!
* \brief 判断是否是本地链接寻址(Autoip)
* \param addr ip地址
* \return bool
*/
bool VersionCreater::isLinkLocalAddress(QHostAddress addr)
{
    quint32 hostIpv4Addr = addr.toIPv4Address();
    quint32 rangeMin = QHostAddress("169.254.1.0").toIPv4Address();
    quint32 rangeMax = QHostAddress("169.254.254.255").toIPv4Address();
    if(hostIpv4Addr >= rangeMin && hostIpv4Addr <= rangeMax)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
* \brief 判断是否是私有地址
* \param addr 地址
* \return bool
*/
bool VersionCreater::isLocalIp(QHostAddress addr)
{
    quint32 hostIpv4Addr = addr.toIPv4Address();
    //A类地址私有地址段
    quint32 range1Min = QHostAddress("10.0.0.0").toIPv4Address();
    quint32 range1Max = QHostAddress("10.255.255.255").toIPv4Address();
    //B类地址私有地址段
    quint32 range3Min = QHostAddress("172.16.0.0").toIPv4Address();
    quint32 range3Max = QHostAddress("172.31.255.255").toIPv4Address();
    //C类地址私有地址段
    quint32 range2Min = QHostAddress("192.168.0.0").toIPv4Address();
    quint32 range2Max = QHostAddress("192.168.255.255").toIPv4Address();

    if((hostIpv4Addr >= range1Min && hostIpv4Addr <= range1Max)
            || (hostIpv4Addr >= range2Min && hostIpv4Addr <= range2Max)
            || (hostIpv4Addr >= range3Min && hostIpv4Addr <= range3Max))
    {
        return true;
    }
    else
    {
        return false;
    }
}
