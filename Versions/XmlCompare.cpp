#include "XmlCompare.h"
#include "XmlCompare_p.h"
#include <QDomDocument>
#include <QDomNode>
#include <QDebug>
#include <QFile>

XmlCompare::XmlCompare()
    : UpdateCompare(*new XmlComparePrivate)
{
}

XmlCompare::XmlCompare(XmlComparePrivate &data)
    : UpdateCompare(data)
{
}

XmlCompare::~XmlCompare()
{
}

bool XmlCompare::saveXml()
{
    DPTR_D(XmlCompare);
    if(NULL == d.pDocument_)
    {
        qDebug() << "d.pDocument_ is null";
        return false;
    }
    QFile file("result.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    d.pDocument_->save(out, 4);
    file.close();

    return true;
}

QString XmlCompare::getDifference(const QString &clientXml, const QString &serverXml)
{
//初始化数据
    DPTR_D(XmlCompare);
    d.clientVersion_ = clientXml;
    d.serverVersion_ = serverXml;

    if(!initClientVersion())
    {
        qDebug() << "initClientVersion error";
    }
    if(!initServerVersion())
    {
        qDebug() << "initServerVersion error";
    }

    initUpdateVersion();

    //开始比较
    compare();

    //保存结果
    QString result = d.pDocument_->toString();
    return result;
}

QList<UpdateFileInformation> XmlCompare::getUpdateFileList(const QString &clientXml, const QString &serverXml)
{
    QString domString = getDifference(clientXml, serverXml);

    //////////////////////////
    ///发布时删掉
    {
        QFile fileTmp("request.xml");
        if(!fileTmp.open(QFile::WriteOnly))
        {
            qDebug() << "request.xml can't open!";
        }
        fileTmp.write(domString.toUtf8());
    }
    //////////////////////////

    /////////////////////////////////////////////////////////////////
    //处理Xml中的有效数据
    QDomDocument *pDoc = new QDomDocument();
    QString errorStr;
    int errorLine;
    int errorCol;

    if(!pDoc->setContent(domString, true, &errorStr, &errorLine, &errorCol))
    {
        qDebug()<<"error! "<<errorStr<<" in line:"<<errorLine<<" "<<errorCol;
        return QList<UpdateFileInformation> ();
    }

    QDomNode *pUpdate = new QDomNode(pDoc->firstChildElement("update"));
    QDomNode *pFiles = new QDomNode(pUpdate->firstChildElement("files"));

    QDomNodeList nodeList = pFiles->childNodes();
    int sizeUpdateFiles = nodeList.size();
    qDebug() << "update files is : " << sizeUpdateFiles;
    if(0 == sizeUpdateFiles)
    {
        //emit signalCloseParent();
    }

    QList<UpdateFileInformation> listFileInfo;

    for(int i = 0; i != nodeList.size(); ++i)
    {
        QString name = nodeList.at(i).firstChildElement("name").firstChild().toText().nodeValue();
        QString path = nodeList.at(i).firstChildElement("path").firstChild().toText().nodeValue();
        QString md5 = nodeList.at(i).firstChildElement("md5").firstChild().toText().nodeValue();
//        qDebug() << "name: " << name;
//        qDebug() << "path : " << path;
//        qDebug() << "md5 : " << md5;

        UpdateFileInformation fileInfor;
        fileInfor.name = name;
        fileInfor.path = path;

        listFileInfo.append(fileInfor);
    }
    //////////////////////////////////////////////////////////////////////

    return listFileInfo;
}

void XmlCompare::compare()
{
    DPTR_D(XmlCompare);

    QDomNodeList serverNodes = d.pServerFilesNode_->childNodes();
    QDomNodeList clientNodes = d.pClientFilesNode_->childNodes();

    for(int i = 0; i != serverNodes.size(); ++i)
    {
        bool bInClient = false;
        QDomNode s = serverNodes.at(i);
        for(int j = 0; j != clientNodes.size(); ++j)
        {
            QDomNode c =  clientNodes.at(j);
            if(getNodeValue(s, tr("name")).nodeValue() == getNodeValue(c, tr("name")).nodeValue())
            {
                //比较MD5 和 lastModify
                if(getNodeValue(s, tr("md5")).nodeValue() == getNodeValue(c, tr("md5")).nodeValue())
                {
                    bInClient = true;
                }
            }
        }

        //这里是更新部分
        if(!bInClient)
        {
            QDomElement updateNode = this->createElement(d.pDocument_, s);
            d.pFilesNode_->appendChild(updateNode);
        }
    }
}

void XmlCompare::star(const QString &path1, const QString &path2)
{
    QFile file1(path1);
    if(!file1.open(QFile::ReadOnly))
    {
        qDebug() << "can't open " << path1;
        return;
    }

    QFile file2(path2);
    if(!file2.open(QFile::ReadOnly))
    {
        qDebug() << "can't open " << path2;
        return;
    }
//初始化数据
    DPTR_D(XmlCompare);
    d.clientVersion_ = QString(file1.readAll());
    d.serverVersion_ = QString(file2.readAll());

    file1.close();
    file2.close();

//    qDebug() << d.clientVersion_;
//    qDebug() << d.serverVersion_;

    if(!initClientVersion())
    {
        qDebug() << "initClientVersion error";
    }
    if(!initServerVersion())
    {
        qDebug() << "initServerVersion error";
    }

    initUpdateVersion();

//开始比较
    compare();

    //保存结果
    saveXml();
}

bool XmlCompare::initClientVersion()
{
    DPTR_D(XmlCompare);
    return initVersion(&d.pClientDocument_, &d.pClientUpdateNode_, &d.pClientFilesNode_, d.clientVersion_);
}

bool XmlCompare::initServerVersion()
{
    DPTR_D(XmlCompare);
    return initVersion(&d.pServerDocument_, &d.pServerUpdateNode_, &d.pServerFilesNode_, d.serverVersion_);
}

bool XmlCompare::initVersion(QDomDocument **pDoc, QDomNode **pUpdate, QDomNode **pFiles, const QString &domString)
{
    (*pDoc) = new QDomDocument();
    //加载Xml的string描述到Dom
    QString errorStr;
    int errorLine;
    int errorCol;

    if(!(*pDoc)->setContent(domString, true, &errorStr, &errorLine, &errorCol))
    {
        qDebug()<<"error! "<<errorStr<<" in line:"<<errorLine<<" "<<errorCol;
        return false;
    }

    *pUpdate = new QDomNode((*pDoc)->firstChildElement("update"));
    *pFiles = new QDomNode((*pUpdate)->firstChildElement("files"));

    return true;
}

void XmlCompare::initUpdateVersion()
{
    DPTR_D(XmlCompare);
    d.pDocument_ = new QDomDocument();
    QDomProcessingInstruction instruction = d.pDocument_->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    d.pDocument_->appendChild(instruction);
    QDomElement updateElement = d.pDocument_->createElement("update");
    d.pDocument_->appendChild(updateElement);

    QDomElement fileElement = d.pDocument_->createElement("files");
    QDomElement ipAddressElement = d.pDocument_->createElement("serverAddress");
    updateElement.appendChild(fileElement);
    updateElement.appendChild(ipAddressElement);

    d.pUpdateNode_ = new QDomNode(d.pDocument_->firstChildElement("update"));
    d.pFilesNode_ = new QDomNode(d.pUpdateNode_->firstChildElement("files"));
    d.pIpAddressNode_= new QDomNode(d.pUpdateNode_->firstChildElement("serverAddress"));
    QDomText ipText = d.pDocument_->createTextNode("");
    d.pIpAddressNode_->appendChild(ipText);

//    qDebug() << d.pUpdateNode_->nodeName();
//    qDebug() << d.pFilesNode_->nodeName();
//    qDebug() << d.pIpAddressNode_->nodeName();
}

QDomText XmlCompare::getNodeValue(QDomNode &node, const QString &name)
{
    return node.firstChildElement(name).firstChild().toText();
}

void XmlCompare::setNodeValue(QDomNode &node, const QString &name, const QString &value)
{
    node.firstChildElement(name).toText().setNodeValue(value);
}

QDomElement XmlCompare::createElement(QDomDocument *pDoc, const QDomNode &node)
{
    QDomElement element = pDoc->createElement(node.nodeName());
    QDomNodeList childList = node.childNodes();
    for(int i = 0; i != childList.size(); ++i)
    {
        QDomElement tmpNode = pDoc->createElement(childList.at(i).nodeName());
        QString value = childList.at(i).firstChild().toText().nodeValue();
        //qDebug() << value;
        QDomText textNode = pDoc->createTextNode(value);
        tmpNode.appendChild(textNode);
        element.appendChild(tmpNode);
    }

    return element;
}




