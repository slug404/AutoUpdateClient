#ifndef VERSIONBASE_P_H
#define VERSIONBASE_P_H
#include "dptr.h"
#include <QMap>
#include <QPointer>
class VersionBase;
class QDomNode;
class QString;
class QDomDocument;

class VersionBasePrivate : public DPtrPrivate<VersionBase>
{
    DPTR_DECLARE_PUBLIC(VersionBase)
public:
    VersionBasePrivate()
        : pUpdateNode(NULL)
        , pDocument(NULL)
        , pMap_nodeName_domNode(NULL)
    {

    }
    QDomNode *pUpdateNode;
    QDomDocument  *pDocument;
    QMap<QString, QDomNode *> *pMap_nodeName_domNode;
};

#endif // VERSIONBASE_P_H
