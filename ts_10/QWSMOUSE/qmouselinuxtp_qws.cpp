/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#if defined (BTWEB) ||  defined (BT_EMBEDDED)

#include "qmouselinuxtp_qws.h"


#ifndef QT_NO_QWS_MOUSE_LINUXTP
#include "qwindowsystem_qws.h"
#include "qsocketnotifier.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qscreen_qws.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>



typedef struct
{
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;




class QWSLinuxTPMouseHandlerPrivate:public QObject
{
    Q_OBJECT
public:
    QWSLinuxTPMouseHandlerPrivate (QWSLinuxTPMouseHandler * h);
    virtual ~ QWSLinuxTPMouseHandlerPrivate ();

private:
    static const int mouseBufSize = 2048;
    static int mouseFD;
    QPoint oldmouse;
    QPoint oldTotalMousePos;
    bool waspressed;
    QPolygon samples;
    unsigned int currSample;
    unsigned int lastSample;
    unsigned int numSamples;
    int skipCount;
    int mouseIdx;
    uchar mouseBuf[mouseBufSize];
    QWSLinuxTPMouseHandler *handler;

private slots:
    void readMouseData();

protected :
        static QSocketNotifier *mouseNotifier;
};

int cc = 0;

int QWSLinuxTPMouseHandlerPrivate::mouseFD = -1;
QSocketNotifier * QWSLinuxTPMouseHandlerPrivate::mouseNotifier = 0;

QWSLinuxTPMouseHandler::QWSLinuxTPMouseHandler ( const QString &, const QString & )
{
    d = new QWSLinuxTPMouseHandlerPrivate (this);
}

QWSLinuxTPMouseHandler::~QWSLinuxTPMouseHandler ()
{
    delete d;
}

QWSLinuxTPMouseHandlerPrivate::QWSLinuxTPMouseHandlerPrivate ( QWSLinuxTPMouseHandler *h )
    : samples ( 30 ), currSample ( 0 ), lastSample ( 0 ),
    numSamples ( 0 ), skipCount ( 0 ), handler ( h )
{
    if (mouseFD < 0)
    {
        if ((mouseFD =::open ("/dev/ts", O_RDONLY | O_NDELAY)) <= 0)
        {
            printf ("\nCANNOT OPEN /dev/ts : %s\n", strerror (errno));
#ifndef DEBUG
            system ("/sbin/reboot");
#endif
            return;
        }
    }
    if (mouseNotifier != 0)
    {
        printf ("\n destroy  mouseNotifier ");
        disconnect ( mouseNotifier, SIGNAL ( activated ( int ) ),this, SLOT ( readMouseData() ) );
        delete mouseNotifier;
        mouseNotifier = 0;
    }
    mouseNotifier = new QSocketNotifier ( mouseFD, QSocketNotifier::Read,
                                          this );
    connect ( mouseNotifier, SIGNAL ( activated ( int ) ),this, SLOT ( readMouseData() ) );


    // BOH: rilascio manuale
    //QTimer * tick = new QTimer(this,"tick");
    //tick->start(400);
    //QObject::connect(tick,SIGNAL(timeout()), this, SLOT(readMouseData()));


    waspressed = FALSE;
    mouseIdx = 0;
}

QWSLinuxTPMouseHandlerPrivate::~QWSLinuxTPMouseHandlerPrivate ()
{
    printf ("\n Sto per Chiudere /dev/ts  FD = %d\n", mouseFD);
    if (mouseFD > 0)
    {
        if (mouseNotifier)
        {
            //mouseNotifier->setEnabled ( false );
            //disconnect ( mouseNotifier, SIGNAL ( activated ( int ) ),this, SLOT ( readMouseData() ) );
            //printf ( "\n delete mouseNotifier  FD = %d\n", mouseFD );
            //delete mouseNotifier;
            //mouseNotifier = 0;
        }

        int ret = -1;
        ret = close (mouseFD);
        mouseFD = -1;
        printf ("\n Chiudo /dev/ts retVal = %d : %s \n", ret, strerror (errno));
    }
}



void QWSLinuxTPMouseHandlerPrivate::readMouseData()
{
	QPoint mousePos;
    if (!qt_screen)
        return;

    int n;

    do
    {
        n = read (mouseFD, mouseBuf + mouseIdx, mouseBufSize - mouseIdx);

        if (n > 0)
            mouseIdx += n;

    }
    while (n > 0 && mouseIdx < mouseBufSize);

    TS_EVENT *data;
    int idx = 0;

    // perhaps we shouldn't be reading EVERY SAMPLE.
    while (mouseIdx - idx >= (int) sizeof (TS_EVENT))
    {
       // qCritical("MouseReadData");
        uchar *mb = mouseBuf + idx;
        data = (TS_EVENT *) mb;

       // qCritical("pressure : %d", data->pressure);
       // qCritical("x : %d", data->x);
       // qCritical("y : %d", data->y);
        if (data->pressure)
        {
		mousePos = handler->transform (QPoint (data->x, data->y));
		oldmouse = mousePos;
                if(!waspressed)
                {
		handler->mouseChanged (mousePos, Qt::LeftButton);
                    // si trova in qmouse_qws.cpp
                   // qCritical("Pressed at x:<%d> and y:<%d>", oldmouse.x(), oldmouse.y());
                    waspressed = true;
                }
                else
		{
		handler->mouseChanged (mousePos, Qt::LeftButton || Qt::NoButton);
                   // qCritical("Sample at x:<%d> and y:<%d>", mousePos.x(), mousePos.y());
		}
        }
        else
        {
                handler->mouseChanged (oldmouse, Qt::NoButton);
               // qCritical("Released at x:<%d> and y:<%d>", oldmouse.x(), oldmouse.y());
                oldmouse = QPoint (-100, -100);
                waspressed = false;
        }
        idx += sizeof (TS_EVENT);
    }

    int surplus = mouseIdx - idx;
    for (int i = 0; i < surplus; i++)
        mouseBuf[i] = mouseBuf[idx + i];
    mouseIdx = surplus;
}

// #include "qmouselinuxtp_qws.moc"
#endif //QT_NO_QWS_MOUSE_LINUXTP

#endif
