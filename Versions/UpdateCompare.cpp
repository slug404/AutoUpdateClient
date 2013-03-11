#include "UpdateCompare.h"
#include "UpdateCompare_p.h"

UpdateCompare::UpdateCompare(UpdateComparePrivate &data)
    : DPTR_INIT(&data)
{
}

UpdateCompare::~UpdateCompare()
{
}

void UpdateCompare::addClientVersion(const QString &str)
{
    DPTR_D(UpdateCompare);
    d.clientVersion_ = str;
}

void UpdateCompare::addServerVersion(const QString &str)
{
    DPTR_D(UpdateCompare);
    d.serverVersion_ = str;
}
