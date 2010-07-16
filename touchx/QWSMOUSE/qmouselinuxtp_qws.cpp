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

#define GET_SECOND_LAST
#undef NO_MOVE_VERIFY

/*
#if defined(QT_QWS_IPAQ)
#define QT_QWS_IPAQ_RAW
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;
#elif defined(QT_QWS_EBX)
#define QT_QWS_EBX_RAW
#ifndef QT_QWS_SHARP
typedef struct {
        unsigned short pressure;
        unsigned short x;
        unsigned short y;
        unsigned short pad;
} TS_EVENT;
#else
typedef struct {
       long y;
       long x;
       long pressure;
       long long millisecs;
} TS_EVENT;
#define QT_QWS_TP_SAMPLE_SIZE 10
#define QT_QWS_TP_MINIMUM_SAMPLES 2
#define QT_QWS_TP_PRESSURE_THRESHOLD 500
#define QT_QWS_TP_MOVE_LIMIT 50
#define QT_QWS_TP_JITTER_LIMIT 2
#endif
//BONF
#elif defined( QT_QWS_SHARP_LH7x)
 typedef struct {
         unsigned short pressure;
         unsigned short x;
         unsigned short y;
         unsigned short pad;
  } TS_EVENT;


#define QT_QWS_TP_PRESSURE_THRESHOLD 1

//ORIGINALE #define QT_QWS_TP_PRESSURE_THRESHOLD 500
//ORIGINALE #define QT_QWS_TP_MOVE_LIMIT 25
#define QT_QWS_TP_MOVE_LIMIT 500

#define QT_QWS_TP_SAMPLE_SIZE 5
//ENDBONF
#else
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;
#endif
*/



typedef struct
{
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} TS_EVENT;

#ifndef QT_QWS_TP_SAMPLE_SIZE
#define QT_QWS_TP_SAMPLE_SIZE 3
#endif

#ifndef QT_QWS_TP_MINIMUM_SAMPLES
#define QT_QWS_TP_MINIMUM_SAMPLES 3
#endif

#ifndef QT_QWS_TP_PRESSURE_THRESHOLD
#define QT_QWS_TP_PRESSURE_THRESHOLD 1
#endif

#ifndef QT_QWS_TP_MOVE_LIMIT
#define QT_QWS_TP_MOVE_LIMIT 100
#endif

#ifndef QT_QWS_TP_JITTER_LIMIT
#define QT_QWS_TP_JITTER_LIMIT 2
#endif



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
    : samples ( QT_QWS_TP_SAMPLE_SIZE ), currSample ( 0 ), lastSample ( 0 ),
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
        uchar *mb = mouseBuf + idx;
        data = (TS_EVENT *) mb;


        if (data->pressure >= QT_QWS_TP_PRESSURE_THRESHOLD)
        {
            samples[currSample] = QPoint (data->x, data->y);

            numSamples++;
            if (numSamples >= QT_QWS_TP_MINIMUM_SAMPLES)
            {
                int sampleCount = qMin ((int) numSamples + 1, samples.count ());
                // average the rest
                QPoint mousePos = QPoint (0, 0);
                QPoint totalMousePos = oldTotalMousePos;


                // avoid first sample
                if (currSample)
                    totalMousePos += samples[currSample];

                if ((int) numSamples >= samples.count ())
                    totalMousePos -= samples[lastSample];

#ifdef GET_SECOND_LAST
                if (currSample > 0)
                    mousePos = samples[currSample - 1];
                else
                    mousePos = samples[1];
#else
                mousePos = totalMousePos / (sampleCount - 1);
#endif

                // si trova in qmouse_qws.cpp
                mousePos = handler->transform (mousePos);

                if (!waspressed)
                    oldmouse = mousePos;
                QPoint dp = mousePos - oldmouse;
                int dxSqr = dp.x () * dp.x ();
                int dySqr = dp.y () * dp.y ();

#ifdef NO_MOVE_VERIFY
                if ( dxSqr + dySqr < ( QT_QWS_TP_MOVE_LIMIT * QT_QWS_TP_MOVE_LIMIT ) )
#endif
                {
                    if (waspressed)
                    {
#ifdef NO_MOVE_VERIFY
                        if ( ( dxSqr + dySqr > ( QT_QWS_TP_JITTER_LIMIT * QT_QWS_TP_JITTER_LIMIT ) ) || skipCount > 3 )
#endif
                        {
                            handler->mouseChanged (mousePos, Qt::LeftButton);
                            oldmouse = mousePos;
                            //qCritical("Sample at x:<%d> and y:<%d>", oldmouse.x(), oldmouse.y());
                            skipCount = 0;
                        }
#ifdef NO_MOVE_VERIFY
                        else
                        {
                            //qCritical ("Skip ");
                            skipCount++;
                        }
#endif
                    }
                    else
                    {
                        handler->mouseChanged (mousePos, Qt::LeftButton);
                        oldmouse = mousePos;
                        waspressed = true;
                        //qCritical("Pressed at x:<%d> and y:<%d>", oldmouse.x(), oldmouse.y());
                    }

                    // save recuring information
                    currSample++;
                    if ((int) numSamples >= samples.count ())
                        lastSample++;
                    oldTotalMousePos = totalMousePos;
                }
#ifdef NO_MOVE_VERIFY
                else
                {
                    numSamples--;	// don't use this sample, it was bad.
                }
#endif
            }
            else
            {
                // build up the average
                oldTotalMousePos += samples[currSample];
                currSample++;
            }
            if ( (int) currSample >= samples.count() ) {
                currSample = 0;
            }
            if ((int) lastSample >= samples.count ())
                lastSample = 0;
        }			// pressure
        else
        {
            currSample = 0;
            lastSample = 0;
            numSamples = 0;
            skipCount = 0;
            oldTotalMousePos = QPoint (0, 0);
            if (waspressed)
            {
                handler->mouseChanged (oldmouse, 0);
                //qCritical("Released at x:<%d> and y:<%d>", oldmouse.x(), oldmouse.y());
                oldmouse = QPoint (-100, -100);
                waspressed = false;
            }
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
