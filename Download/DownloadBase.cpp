#include "DownloadBase.h"
#include "DownloadBase_p.h"

DownloadBase::~DownloadBase()
{
}

void DownloadBase::start()
{
    prepare();
    request();
}

DownloadBase::DownloadBase(DownloadBasePrivate &data)
    : DPTR_INIT(&data)
{
}
