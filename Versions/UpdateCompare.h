#ifndef UPDATECOMPARE_H
#define UPDATECOMPARE_H

#include <QObject>
#include "dptr.h"

class UpdateComparePrivate;

class UpdateCompare : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(UpdateCompare);

public:
    virtual ~UpdateCompare();
    void addClientVersion(const QString &str);
    void addServerVersion(const QString &str);
    virtual void compare()  = 0;
    virtual void star(const QString &path1, const QString &path2) = 0;

protected:
    UpdateCompare(QObject *parent = 0);
    explicit UpdateCompare(UpdateComparePrivate &data);

    DPTR_DECLARE(UpdateCompare);
};

#endif // UPDATECOMPARE_H
