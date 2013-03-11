#ifndef UPDATECOMPARE_P_H
#define UPDATECOMPARE_P_H
//#include "UpdateCompare.h"
#include <QString>
#include "dptr.h"

class UpdateCompare;

class UpdateComparePrivate: public DPtrPrivate<UpdateCompare>
{
    DPTR_DECLARE_PUBLIC(UpdateCompare);

public:
    QString clientVersion_;
    QString serverVersion_;
};

#endif // UPDATECOMPARE_P_H
