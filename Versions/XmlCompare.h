#ifndef XMLCOMPARE_H
#define XMLCOMPARE_H

#include "UpdateCompare.h"

class XmlComparePrivate;
class QDomDocument;
class QDomNode;
class QDomText;
class QDomElement;

struct UpdateFileInformation
{
    QString name;
    QString path;
};

class XmlCompare : public UpdateCompare
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(XmlCompare)
public:
    explicit XmlCompare();
    virtual ~XmlCompare();
    void star(const QString &path1, const QString &path2);
    bool saveXml();
    QString getDifference(const QString &clientXml, const QString &serverXml);

    QList<UpdateFileInformation> getUpdateFileList(const QString &clientXml, const QString &serverXml);

signals:
    void signalCloseParent();

protected:
    explicit XmlCompare(XmlComparePrivate &data);
    virtual void compare();

private:
    bool initVersion(QDomDocument **pDoc, QDomNode **pUpdate, QDomNode **pFiles, const QString &domString);

    bool initClientVersion();
    bool initServerVersion();
    void initUpdateVersion();
    QDomText getNodeValue(QDomNode &node, const QString &name);
    void setNodeValue(QDomNode &node, const QString &name, const QString &value);
    QDomElement createElement(QDomDocument *pDoc, const QDomNode &node);
};

#endif // XMLCOMPARE_H
