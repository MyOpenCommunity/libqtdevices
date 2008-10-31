#ifndef	CALIBRATE_H
#define	CALIBRATE_H

#include <QWSPointerCalibrationData>
#include <QWidget>
#include <QPixmap>


class BtButton;
class BtLabel;
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
	Calibrate(QWidget* parent=0, unsigned char manut=0);
	~Calibrate();

private:
	QPoint fromDevice(const QPoint &p);
	/// Check if the calibration is ok
	bool sanityCheck();
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
	// the new position of the crosshair
	QPoint newPos;
	QPoint penPos;
	QTimer *timer, *button_timer;
	BtLabel *box_text;
	BtButton *b1, *b2;
	unsigned char manut;
	int dx;
	int dy;
	bool button_test;
};


#endif //CALIBRATE_H


