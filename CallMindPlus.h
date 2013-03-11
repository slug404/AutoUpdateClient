#ifndef CALLMINDPLUS_H
#define CALLMINDPLUS_H

#include <QObject>

class CallMindPlus : public QObject
{
    Q_OBJECT
public:
    explicit CallMindPlus(const QString &path, QObject *parent = 0);

signals:

public slots:
    void slotCallMindPlus();

private:
    QString path_;
};

#endif // CALLMINDPLUS_H
