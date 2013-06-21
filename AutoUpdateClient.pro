#-------------------------------------------------
#
# Project created by QtCreator 2013-02-04T18:25:11
#
#-------------------------------------------------

QT       += core gui
QT += xml
QT += network

CONFIG(release, debug|release):DEFINES += USE_RELEASE
CONFIG(debug, debug|release):DEFINES += USE_DEBUG
CONFIG(debug, debug|release):QT += testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mind+
TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += QMAKE_CXXFLAGS += -std=c++0x
equals(QT_MAJOR_VERSION, 5): CONFIG += c++11

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

win32:RC_FILE = ico.rc
macx:ICON = v095_logo_macx.icns
unix:
