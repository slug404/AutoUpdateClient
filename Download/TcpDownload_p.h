#ifndef TCPDOWNLOAD_P_H
#define TCPDOWNLOAD_P_H

#include "DownloadBase_p.h"

class TcpDownload;
class QTcpSocket;

class TcpDownloadPrivate : public DownloadBasePrivate
{
public:
    TcpDownloadPrivate()
        : pTcpSocket(NULL)
    {
    }

    //public data here
    QTcpSocket *pTcpSocket;
    //其他的到时候再加

private:
    //q-ptr
    DPTR_DECLARE_PUBLIC(TcpDownload)
};

#endif // TCPDOWNLOAD_P_H
