#ifndef DOWNLOADBASE_P_H
#define DOWNLOADBASE_P_H
#include "dptr.h"

class DownloadBase;
class QString;
class DownloadBasePrivate : public DPtrPrivate<DownloadBase>
{
    DPTR_DECLARE_PUBLIC(DownloadBase);
public:
    DownloadBasePrivate()
        : requestXml_("")
        , downloadPath_("")
        , progress_(0)
    {

    }

    QString requestXml_;
    QString downloadPath_;
    int progress_;
};

#endif // DOWNLOADBASE_P_H
