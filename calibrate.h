//#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#ifndef	CALIBRATE
#define	CALIBRATE

//#include <qwsmouse_qws.h>
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
#include "qmouselinuxtp_qws.h" 
#endif
#include <qwidget.h>
#include <qpixmap.h>

class QTimer;

class Calibrate : public QWidget
{
    Q_OBJECT
public:
    Calibrate(QWidget* parent=0, const char * name=0, WFlags=0);
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

private:
//    QPixmap logo;
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    QWSPointerCalibrationData cd;
    QWSPointerCalibrationData::Location location;
#endif    
    QPoint crossPos;
    QPoint penPos;
    QPixmap saveUnder;
    QTimer *timer;
    int dx;
    int dy;
};


#endif
//#endif // _WS_QWS_

