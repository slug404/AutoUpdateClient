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
    QMap<QString, QString> getFilePath() { return map_name_path_; }

protected:
    VersionCreater(VersionCreaterPrivate &data);
    virtual void initDomTree();
    virtual void traveDomTree(const QString &str, const QStringList &filterFolderPaths);
    QString & getLocalIpAddress() const;
    QString getHostIp();
    bool isLinkLocalAddress(QHostAddress addr);
    bool isLocalIp(QHostAddress addr);

private:
    QMap<QString, QString> map_name_path_;
};

#endif // VERSIONCREATER_H
