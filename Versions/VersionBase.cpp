#include "VersionBase.h"
#include "VersionBase_p.h"
#include <QFile>
#include <QTextStream>
#include <QDomNode>
#include <QDomDocument>
#include <QDebug>
#include <QPointer>

VersionBase::VersionBase(VersionBasePrivate &data)
    : DPTR_INIT(&data)
{
    DPTR_D(VersionBase);
    d.pMap_nodeName_domNode_ = new QMap<QString, QDomNode *>();
}

VersionBase::~VersionBase()
{
}

void VersionBase::start(const QString &str)
{
    initDomTree();
    traveDomTree(str);
}

bool VersionBase::saveXml(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    DPTR_D(VersionBase);
    if(NULL == d.pDocument_)
    {
        qDebug() << "pDocument_ is null!";
        return false;
    }
    d.pDocument_->save(out, 4);

    file.close();
    return true;
}
