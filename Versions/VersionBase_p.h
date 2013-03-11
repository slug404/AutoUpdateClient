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
        : pUpdateNode_(NULL)
        , pDocument_(NULL)
        , pMap_nodeName_domNode_(NULL)
    {

    }
    QDomNode *pUpdateNode_;
    QDomDocument  *pDocument_;
    QMap<QString, QDomNode *> *pMap_nodeName_domNode_;
};

#endif // VERSIONBASE_P_H
