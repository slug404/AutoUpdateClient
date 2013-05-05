#ifndef VERSIONBASE_H
#define VERSIONBASE_H

#include <QObject>
#include "../dptr.h"

class VersionBasePrivate;
class QDomNode;

class VersionBase : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(VersionBase)
public:
    virtual ~VersionBase();
	void start(const QString &str);
    bool saveXml(const QString &filePath);

protected:
    explicit VersionBase(QObject *parent = 0);
    VersionBase(VersionBasePrivate &data);
    virtual void initDomTree() = 0;
    virtual void traveDomTree(const QString &str) = 0;

    DPTR_DECLARE(VersionBase)
};

#endif // VERSIONBASE_H
