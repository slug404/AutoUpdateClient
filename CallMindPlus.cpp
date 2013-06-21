#include "CallMindPlus.h"
#include <QProcess>

CallMindPlus::CallMindPlus(const QString &path, QObject *parent)
	: QObject(parent)
	, path_(path)
{
}

void CallMindPlus::slotCallMindPlus()
{
	QProcess::execute(path_);
}
