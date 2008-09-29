#ifndef	CALIBRATE_H
#define	CALIBRATE_H

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
#include "QWSMOUSE/qmouse_qws.h"
#endif
#include <qwidget.h>
#include <qpixmap.h>


class BtButton;
class QLabel;
class QTimer;

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
	QPoint fromDevice(const QPoint &p);
	/// Check if the calibration is ok
	bool sanityCheck();
	void moveCrosshair(QPoint pt);
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	/// Start the buttons test
	void buttonsTest();
	void trackCrosshair();
	void startCalibration();
	BtButton *createButton(const char* icon_name, int x, int y);

private slots:
	void timeout();
	void endCalibration();
	void rollbackCalibration();

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
	QTimer *timer, *button_timer;
	QLabel *box_text;
	BtButton *b1, *b2;
	unsigned char manut;
	int dx;
	int dy;
	bool button_test;
};


#endif //CALIBRATE_H


