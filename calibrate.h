//#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#ifndef	CALIBRATE_H
#define	CALIBRATE_H

//#include <qwsmouse_qws.h>
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
//#include "QWSMOUSE/qmouselinuxtp_qws.h"
#include "QWSMOUSE/qmouse_qws.h"
#endif
#include <qwidget.h>
#include <qpixmap.h>

//class QTimer;
#include <qtimer.h>
/*!
  \class calibrate
  \brief This is a class that does the calibration of the device.
  
  According to the forth argument it is possible to choose if the calibration process must have for or five pressions.  
  \author Davide
  \date lug 2005
*/  

class Calibrate : public QWidget
{
    Q_OBJECT
public:
    Calibrate(QWidget* parent=0, const char * name=0, WFlags=0, unsigned char manut=0);
    ~Calibrate();

private:
    QPoint fromDevice( const QPoint &p );
    bool sanityCheck();
    void moveCrosshair( QPoint pt );
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );

private slots:
    void timeout();
signals:
 void inizioCalib();
void fineCalib();

private:
    QPixmap logo;
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    QWSPointerCalibrationData cd;
    QWSPointerCalibrationData::Location location;
#endif    
    QPoint crossPos;
    QPoint penPos;
    QPixmap saveUnder;
    //QPixmap bersaglio;    
    QTimer *timer;
    unsigned char manut;
    int dx;
    int dy;
};


#endif //CALIBRATE_H
//#endif // _WS_QWS_

