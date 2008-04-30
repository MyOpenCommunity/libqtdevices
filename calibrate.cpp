//#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#include <qpainter.h>
#include <qtimer.h>
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>
#endif
//#include "../library/resource.h"
#include "calibrate.h"
#include <qfile.h>
#include <qapplication.h>
#include <qcursor.h>
#include "main.h"

Calibrate::Calibrate(QWidget* parent, const char * name, WFlags wf, unsigned char m) :
    QWidget( parent, name, wf | WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop | WDestructiveClose )
{
    const int offset = 30;
    QRect desk = qApp->desktop()->geometry();
    setGeometry( 0, 0, desk.width(), desk.height() );
    logo.load(IMG_PATH "my_home.png");
//    bersaglio.load(IMG_PATH "bersaglio.png");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
  if(!strcmp(getenv("QWS_DISPLAY"), "Transformed:Rot180"))
  {
    cd.screenPoints[QWSPointerCalibrationData::BottomRight] = QPoint( offset, offset );
    cd.screenPoints[QWSPointerCalibrationData::TopRight] = QPoint( offset, qt_screen->deviceHeight() - offset );
    cd.screenPoints[QWSPointerCalibrationData::TopLeft] = QPoint( qt_screen->deviceWidth() - offset, qt_screen->
deviceHeight() - offset );
    cd.screenPoints[QWSPointerCalibrationData::BottomLeft] = QPoint( qt_screen->deviceWidth() - offset, offset );
    cd.screenPoints[QWSPointerCalibrationData::Center] = QPoint( qt_screen->deviceWidth()/2, qt_screen->deviceHeight
()/2 );
  }
  else
  {
    cd.screenPoints[QWSPointerCalibrationData::TopLeft] = QPoint( offset, offset );
    cd.screenPoints[QWSPointerCalibrationData::BottomLeft] = QPoint( offset, qt_screen->deviceHeight() - offset );
    cd.screenPoints[QWSPointerCalibrationData::BottomRight] = QPoint( qt_screen->deviceWidth() - offset, qt_screen->deviceHeight() - offset );
    cd.screenPoints[QWSPointerCalibrationData::TopRight] = QPoint( qt_screen->deviceWidth() - offset, offset );
    cd.screenPoints[QWSPointerCalibrationData::Center] = QPoint( qt_screen->deviceWidth()/2, qt_screen->deviceHeight()/2 );
  }
    crossPos = fromDevice( cd.screenPoints[QWSPointerCalibrationData::TopLeft] );
    location = QWSPointerCalibrationData::TopLeft;
   setCursor (QCursor (blankCursor));
#endif
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timeout()) );
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    QWSServer::mouseHandler()->clearCalibration();
#endif  
    grabMouse();
    manut=m;
}

Calibrate::~Calibrate()
{
    releaseMouse();
}

QPoint Calibrate::fromDevice( const QPoint &p )
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	qWarning("inside Calibrate::fromDevice");
    return qt_screen->mapFromDevice( p,QSize(qt_screen->deviceWidth(), qt_screen->deviceHeight()) );
#endif
}

bool Calibrate::sanityCheck()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
   
    QPoint tl = cd.devPoints[QWSPointerCalibrationData::TopLeft];
    QPoint tr = cd.devPoints[QWSPointerCalibrationData::TopRight];
    QPoint bl = cd.devPoints[QWSPointerCalibrationData::BottomLeft];
    QPoint br = cd.devPoints[QWSPointerCalibrationData::BottomRight];

    int vl = QABS( tl.x() - bl.x() );
    int vr = QABS( tr.x() - br.x() );

    int diff = QABS( vl - vr );

    int avg1 = ( tl.x()+ bl.x()) / 2;
    int avg2 = ( tr.x()+ br.x()) / 2;
    if (avg1>avg2)
        avg2=avg1;

    if ((vl>avg2/10)||(vr>avg2/10))
{
        qWarning("err>10 per cento     ->  left: %d right: %d\n",vl/avg2*100 ,vr/avg2*100);
        return FALSE;
}
    int ht = QABS( tl.y() - tr.y() );
    int hb = QABS( br.y() - bl.y() );
    diff = QABS( ht - hb );
    avg1 = ( tl.y()+ tr.y()) / 2;
    avg2 = ( br.y()+ bl.y()) / 2;
    if (avg1>avg2)
        avg2=avg1;
    if ((ht>avg2/10)||(hb>avg2/10))
{
    qWarning("err>10 per cento top: %d bottom: %d\n",ht/avg2*100 ,hb/avg2*100);
	return FALSE;
}

#endif	
qWarning("return TRUE Calibrate::sanityCheck");
    return TRUE;
}

void Calibrate::moveCrosshair( QPoint pt )
{
    QPainter p( this );
    p.drawPixmap( crossPos.x()-8, crossPos.y()-8, saveUnder );
   saveUnder = QPixmap::grabWindow( winId(), pt.x()-8, pt.y()-8, 16, 16 );
    p.drawRect( pt.x()-1, pt.y()-8, 2, 7 );
    p.drawRect( pt.x()-1, pt.y()+1, 2, 7 );
    p.drawRect( pt.x()-8, pt.y()-1, 7, 2 );
    p.drawRect( pt.x()+1, pt.y()-1, 7, 2 );
/*   p.drawPixmap(pt.x()-40,pt.y()-40,bersaglio);*/
    crossPos = pt;
}

void Calibrate::paintEvent( QPaintEvent * )
{
    QPainter p( this );

    int y;

   if ( !logo.isNull() ) {
	y = height() / 2 - logo.height() - 15;
	p.drawPixmap( (width() - logo.width())/2, y, logo );
    }
   else
       qDebug("logo is Null");

    y = height() / 2 + 15;

    saveUnder = QPixmap::grabWindow( winId(), crossPos.x()-8, crossPos.y()-8, 16, 16 );
    moveCrosshair( crossPos );
}

void Calibrate::mousePressEvent( QMouseEvent *e )
{
    // map to device coordinates
	qWarning("Calibrate::mousePressEvent");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    QPoint devPos = qt_screen->mapToDevice( e->pos(),
                                            QSize(qt_screen->width(), qt_screen->height()) );

    if ( penPos.isNull() )
        penPos = devPos;
    else
        penPos = QPoint( (penPos.x() + devPos.x())/2,
                         (penPos.y() + devPos.y())/2 );
#endif    
}

void Calibrate::mouseReleaseEvent( QMouseEvent * )
{
    if ( timer->isActive() )
	return;

    bool doMove = TRUE;

#if defined (BTWEB) ||  defined (BT_EMBEDDED)

    cd.devPoints[location] = penPos;
    
    unsigned char lastLoc;
    if (manut==0)
	lastLoc=QWSPointerCalibrationData::LastLocation;
    else
	lastLoc=QWSPointerCalibrationData::LastLocation-1;
    qWarning("il mio stato di manutenzione è: %d - e lastLoc= %d", manut, lastLoc);
    //if ( location < QWSPointerCalibrationData::LastLocation ) {
    if ( location < lastLoc) {
	qWarning("location < lastLoc    ovvero %d<%d",location,lastLoc);
	location = (QWSPointerCalibrationData::Location)((int)location + 1);
	qWarning("new location: %d", location);
    } else {
	qWarning("sto per fare Sanity Chack");
	if ( sanityCheck() ) {
	    QWSServer::mouseHandler()->calibrate( &cd );
	    releaseMouse();
	    hide();
	    close();
	    doMove = FALSE;
	    emit (fineCalib());
	    if (manut==1)
		delete(this);
	} else {
	    location = QWSPointerCalibrationData::TopLeft;
	}
    }
    
    if ( doMove ) {
	QPoint target = fromDevice( cd.screenPoints[location] );
	dx = (target.x() - crossPos.x())/10;
	dy = (target.y() - crossPos.y())/10;
	timer->start( 30 );
    }
#endif	
}

void Calibrate::timeout()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    QPoint target = fromDevice( cd.screenPoints[location] );

    bool doneX = FALSE;
    bool doneY = FALSE;
    QPoint newPos( crossPos.x() + dx, crossPos.y() + dy );

    if ( QABS(crossPos.x() - target.x()) <= QABS(dx) ) {
	newPos.setX( target.x() );
	doneX = TRUE;
    }

    if ( QABS(crossPos.y() - target.y()) <= QABS(dy) ) {
	newPos.setY(target.y());
	doneY = TRUE;
    }

    if ( doneX && doneY ) {
	penPos = QPoint();
	timer->stop();
    }
    moveCrosshair( newPos );
    #endif
}
//#endif
