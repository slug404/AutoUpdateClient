#ifndef DOWNLOADBASE_H
#define DOWNLOADBASE_H

#include <QObject>
#include "dptr.h"

class DownloadBasePrivate;

class DownloadBase : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(DownloadBase);
public:
    virtual ~DownloadBase();
    void start();

protected:
    DISABLE_COPY(DownloadBase)
    explicit DownloadBase(DownloadBasePrivate &data);
    virtual void prepare() = 0;
    virtual void request() = 0;
    virtual void handleData() = 0;
    virtual void clear() = 0;

protected:
    DPTR_DECLARE(DownloadBase)
};

#endif // DOWNLOADBASE_H
