#ifndef VERSIONCREATER_P_H
#define VERSIONCREATER_P_H
#include "VersionBase_p.h"
class VersionCreater;
class VersionCreaterPrivate: public VersionBasePrivate
{
    DPTR_DECLARE_PUBLIC(VersionCreater);
    QDomNode *pFilesNode_;
    QDomNode *pIpAddressNode_;
};

#endif // VERSIONCREATER_P_H
