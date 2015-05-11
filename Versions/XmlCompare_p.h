#ifndef XMLCOMPARE_P_H
#define XMLCOMPARE_P_H

#include "UpdateCompare_p.h"
#include <QMap>

class XmlCompare;
class QDomNode;
class QString;
class QDomDocument;

class XmlComparePrivate : public UpdateComparePrivate
{
    DPTR_DECLARE_PUBLIC(XmlCompare)

public:
    explicit XmlComparePrivate()
        : pClientUpdateNode_(NULL)
        , pClientFilesNode_(NULL)
        , pClientDocument_(NULL)
        , pClientMap_nodeName_domNode_(NULL)
        , pServerUpdateNode_(NULL)
        , pServerFilesNode_(NULL)
        , pServerDocument_(NULL)
        , pServerMap_nodeName_domNode_(NULL)
        , pUpdateNode_(NULL)
        , pFilesNode_(NULL)
        , pDocument_(NULL)
        , pIpAddressNode_(NULL)
        , pMap_nodeName_domNode_(NULL)
    {
    }

    QDomNode *pClientUpdateNode_;
    QDomNode *pClientFilesNode_;
    QDomDocument  *pClientDocument_;
    QMap<QString, QDomNode *> *pClientMap_nodeName_domNode_;

    QDomNode *pServerUpdateNode_;
    QDomNode *pServerFilesNode_;
    QDomDocument  *pServerDocument_;
    QMap<QString, QDomNode *> *pServerMap_nodeName_domNode_;

    QDomNode *pUpdateNode_;
    QDomNode *pFilesNode_;
    QDomDocument  *pDocument_;
    QDomNode *pIpAddressNode_;
    QMap<QString, QDomNode *> *pMap_nodeName_domNode_;
};

#endif // XMLCOMPARE_P_H
