#ifndef VERSIONCREATER_H
#define VERSIONCREATER_H

#include "VersionBase.h"
#include <QMap>
#include "dptr.h"

class VersionCreaterPrivate;
class QHostAddress;

class VersionCreater : public VersionBase
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VersionCreater);
public:
    VersionCreater();
    virtual ~VersionCreater();
    QString getXml();

protected:
    VersionCreater(VersionCreaterPrivate &data);
    virtual void initDomTree();
    virtual void traveDomTree(const QString &str);
	QString getLocalIpAddress() const;
    QString getHostIp();
    bool isLinkLocalAddress(QHostAddress addr);
    bool isLocalIp(QHostAddress addr);

private:
};

#endif // VERSIONCREATER_H
