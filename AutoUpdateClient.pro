#-------------------------------------------------
#
# Project created by QtCreator 2013-02-04T18:25:11
#
#-------------------------------------------------

QT       += core gui
QT += xml
QT += network
QT += testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MindUpgrader
TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += QMAKE_CXXFLAGS += -std=c++0x
equals(QT_MAJOR_VERSION, 5): CONFIG += c++11

LIBS += -llibLog4Qt

SOURCES += main.cpp\
		WidgetMain.cpp\
	Versions/VersionBase.cpp \
	Versions/VersionCreater.cpp \
	Versions/XmlCompare.cpp \
	Versions/UpdateCompare.cpp \
	Download/DownloadBase.cpp \
	Download/TcpDownload.cpp \
	CallMindPlus.cpp

HEADERS  += WidgetMain.h \
	Versions/VersionCreater_p.h \
	Versions/VersionCreater.h \
	Versions/VersionBase_p.h \
	Versions/VersionBase.h \
	dptr.h \
	Versions/XmlCompare.h \
	Versions/XmlCompare_p.h \
	Versions/UpdateCompare.h \
	Versions/UpdateCompare_p.h \
	Download/DownloadBase.h \
	Download/DownloadBase_p.h \
	Download/TcpDownload_p.h \
	Download/TcpDownload.h \
	CallMindPlus.h

FORMS    += WidgetMain.ui

RESOURCES += \
	src.qrc

#RC_FILE = ico.rc
