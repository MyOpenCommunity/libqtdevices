/****************************************************************************
** $Id: qmouselinuxtp_qws.cpp,v 1.6 2007/01/08 08:04:11 cvs Exp $
**
** Implementation of Qt/Embedded mouse drivers
**
** Created : 20020220
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#if defined (BTWEB) ||  defined (BT_EMBEDDED)


#include "qmouselinuxevent-2-6_qws.h"


#ifndef QT_NO_QWS_MOUSE_LINUXEV
#define QT_NO_QWS_MOUSE_LINUXEV

#include <qwindowsystem_qws.h>
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qscreen_qws.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <include/linux/input.h>

typedef struct {
    long sec_dummy;
    long usec_dummy;
    unsigned short type;
    unsigned short code;
    unsigned int value;
} TS_EVENT;

#define QT_QWS_EV_SAMPLE_SIZE 1
#define QT_QWS_TP_SAMPLE_SIZE 1
#define QT_QWS_EV_MINIMUM_SAMPLES 1
#define QT_QWS_EV_PRESSURE_THRESHOLD 1
#define QT_QWS_EV_MOVE_LIMIT 100
#define QT_QWS_EV_JITTER_LIMIT 2
#define QT_QWS_EV_DEVICE "/dev/ts"

class QWSLinuxTPMouseHandlerPrivate : public QObject
{
    Q_OBJECT
public:
    QWSLinuxTPMouseHandlerPrivate( QWSLinuxTPMouseHandler *h );
    ~QWSLinuxTPMouseHandlerPrivate();

private:
//    static const int mouseBufSize = 2048;
//    static const int mouseBufSize = 128;
    static const int mouseBufSize = 100 * sizeof( TS_EVENT );
    int mouseFD;
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
};

QWSLinuxTPMouseHandler::QWSLinuxTPMouseHandler( const QString &, const QString & )
{
    d = new QWSLinuxTPMouseHandlerPrivate( this );
}

QWSLinuxTPMouseHandler::~QWSLinuxTPMouseHandler()
{
    delete d;
}

QWSLinuxTPMouseHandlerPrivate::QWSLinuxTPMouseHandlerPrivate( QWSLinuxTPMouseHandler *h )
    : samples(QT_QWS_EV_SAMPLE_SIZE), currSample(0), lastSample(0),
    numSamples(0), skipCount(0), handler(h)
{
    if ((mouseFD = open( QT_QWS_EV_DEVICE, O_RDONLY | O_NDELAY)) < 0)
    {
        qDebug( "Cannot open %s (%s)", QT_QWS_EV_DEVICE, strerror(errno));
        return;
    }
    qDebug( "OK open %s",QT_QWS_EV_DEVICE);
    
    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));

    waspressed=FALSE;
    mouseIdx = 0;
}

QWSLinuxTPMouseHandlerPrivate::~QWSLinuxTPMouseHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}



void QWSLinuxTPMouseHandlerPrivate::readMouseData()
{
  int n;
  static unsigned int tmpx = 0,tmpy = 0;
  unsigned int press = 0;
  static QPoint oldpress = QPoint(-1,-1);


  //qDebug( "readMouseData ");

  if(!qt_screen)
    return;

  mouseIdx = 0; 

  //qDebug("mouseIdx=%d",mouseIdx);

  do
  {
    n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );

    if ( n > 0 )
      mouseIdx += n;
      
  } while ( n > 0 && mouseIdx < mouseBufSize );

  TS_EVENT *data;
  int idx = 0;
  //qDebug("1 mouseIdx=%d",mouseIdx);

  // perhaps we shouldn't be reading EVERY SAMPLE.
  while ( mouseIdx-idx >= (int)sizeof( TS_EVENT ) )
  {
    uchar *mb = mouseBuf+idx;
    data = (TS_EVENT *) mb;
    QPoint mousePos;
    
    //qDebug( "D:%d,%d,%d",data->type,data->code,data->value);

    switch (data->type) {
    case EV_SYN:
      //qDebug( "EV_SYN"); 
      if (tmpx && tmpy) {
      //qDebug( "V:%d,%d",tmpx,tmpy);
      QPoint point = QPoint( tmpx, tmpy );
      mousePos = handler->transform( point );
      //qDebug( "S:%d,%d",mousePos.x(),mousePos.y());
      oldpress = mousePos;
      handler->mouseChanged(mousePos,Qt::LeftButton);
    }
    break;
    case EV_KEY:
      //qDebug( "EV_KEY"); 
      if ((data->code == BTN_TOUCH) && (data->value == 0x1)) {
        //qDebug( "BTN_TOUCH DOWN"); 
        tmpx=0; tmpy=0;
        press=1;
      } else if ((data->code == BTN_TOUCH) && (data->value == 0x0)) {
	//qDebug( "BTN_TOUCH UP"); 
        press=0;
        tmpx=0; tmpy=0;
        //qDebug( "R:%d,%d",oldpress.x(),oldpress.y()); 
        handler->mouseChanged(oldpress,0);
        oldpress=QPoint(-1,-1);
      }
    break;
    case EV_ABS:
      //qDebug( "EV_ABS"); 
      /* the TS */
      if (press==1) {
        if (data->code == ABS_Y) tmpx = data->value;
        if (data->code == ABS_X) tmpy = data->value;
      } 
      //qDebug( "A:%d,%d",tmpx,tmpy);
    }

#if 0
    if ((press==1) && tmpx && tmpy) {
      qDebug( "V:%d,%d",tmpx,tmpy);
      press = 0;
      QPoint point = QPoint( tmpx, tmpy );
      mousePos = handler->transform( point );
      qDebug( "M:%d,%d",mousePos.x(),mousePos.y());
      oldpress = mousePos;
      handler->mouseChanged(mousePos,Qt::LeftButton);
    }
#endif
    //qDebug( "-------------- idx=%d",idx);

    idx += sizeof( TS_EVENT );

  }

}

#endif //QT_NO_QWS_MOUSE_LINUXEV

#endif
