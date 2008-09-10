
#include "calibrate.h"
#include "main.h"
#include "fontmanager.h"

#include <qpainter.h>
#include <qtimer.h>
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>
#endif
#include <qfile.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qpushbutton.h>

#define BUTTON_SEC_TIMEOUT 10



Calibrate::Calibrate(QWidget* parent, const char * name, WFlags wf, unsigned char m) :
	QWidget(parent, name, wf | WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop | WDestructiveClose), show_crosshair(true)
{
	const int offset = 30;
	QRect desk = qApp->desktop()->geometry();
	setGeometry(0, 0, desk.width(), desk.height());
	logo.load(IMG_PATH "my_home.png");

#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	if (!strcmp(getenv("QWS_DISPLAY"), "Transformed:Rot180"))
	{
		cd.screenPoints[QWSPointerCalibrationData::BottomRight] = QPoint(offset, offset);
		cd.screenPoints[QWSPointerCalibrationData::TopRight] = QPoint(offset, qt_screen->deviceHeight() - offset);
		cd.screenPoints[QWSPointerCalibrationData::TopLeft] = QPoint(qt_screen->deviceWidth() - offset, qt_screen->deviceHeight() - offset);
		cd.screenPoints[QWSPointerCalibrationData::BottomLeft] = QPoint(qt_screen->deviceWidth() - offset, offset);
		cd.screenPoints[QWSPointerCalibrationData::Center] = QPoint(qt_screen->deviceWidth()/2, qt_screen->deviceHeight()/2);
	}
	else
	{
		cd.screenPoints[QWSPointerCalibrationData::TopLeft] = QPoint(offset, offset);
		cd.screenPoints[QWSPointerCalibrationData::BottomLeft] = QPoint(offset, qt_screen->deviceHeight() - offset);
		cd.screenPoints[QWSPointerCalibrationData::BottomRight] = QPoint(qt_screen->deviceWidth() - offset, qt_screen->deviceHeight() - offset);
		cd.screenPoints[QWSPointerCalibrationData::TopRight] = QPoint(qt_screen->deviceWidth() - offset, offset);
		cd.screenPoints[QWSPointerCalibrationData::Center] = QPoint(qt_screen->deviceWidth()/2, qt_screen->deviceHeight()/2);
	}
	crossPos = fromDevice(cd.screenPoints[QWSPointerCalibrationData::TopLeft]);
	location = QWSPointerCalibrationData::TopLeft;
	setCursor(QCursor(blankCursor));
#endif
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	if (QFile::exists("/etc/pointercal"))
		system("cp /etc/pointercal /etc/pointercal.calibrated");

	QWSServer::mouseHandler()->clearCalibration();
#endif
	grabMouse();
	manut = m;
}

Calibrate::~Calibrate()
{
	releaseMouse();
}

QPoint Calibrate::fromDevice(const QPoint &p)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	qWarning("inside Calibrate::fromDevice");
	return qt_screen->mapFromDevice(p,QSize(qt_screen->deviceWidth(), qt_screen->deviceHeight()));
#endif
}

bool Calibrate::sanityCheck()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)

	QPoint tl = cd.devPoints[QWSPointerCalibrationData::TopLeft];
	QPoint tr = cd.devPoints[QWSPointerCalibrationData::TopRight];
	QPoint bl = cd.devPoints[QWSPointerCalibrationData::BottomLeft];
	QPoint br = cd.devPoints[QWSPointerCalibrationData::BottomRight];

	int vl = QABS(tl.x() - bl.x());
	int vr = QABS(tr.x() - br.x());

	int diff = QABS(vl - vr);

	int avg1 = (tl.x() + bl.x()) / 2;
	int avg2 = (tr.x() + br.x()) / 2;
	if (avg1 > avg2)
		avg2=avg1;

    if ((vl > avg2/10)||(vr > avg2/10))
	{
		qWarning("err>10 per cento     ->  left: %d right: %d\n",vl/avg2*100 ,vr/avg2*100);
		return FALSE;
	}
	int ht = QABS(tl.y() - tr.y());
	int hb = QABS(br.y() - bl.y());
	diff = QABS(ht - hb);
	avg1 = (tl.y()+ tr.y()) / 2;
	avg2 = (br.y()+ bl.y()) / 2;
	if (avg1 > avg2)
		avg2=avg1;
	if ((ht > avg2/10)||(hb > avg2/10))
	{
		qWarning("err>10 per cento top: %d bottom: %d\n",ht/avg2*100 ,hb/avg2*100);
		return FALSE;
	}

#endif	
	qWarning("return TRUE Calibrate::sanityCheck");
	return TRUE;
}

void Calibrate::moveCrosshair(QPoint pt)
{
	QPainter p(this);
	p.drawPixmap(crossPos.x()-8, crossPos.y()-8, saveUnder);
	saveUnder = QPixmap::grabWindow(winId(), pt.x()-8, pt.y()-8, 16, 16);
	if (show_crosshair)
	{
		p.drawRect(pt.x()-1, pt.y()-8, 2, 7);
		p.drawRect(pt.x()-1, pt.y()+1, 2, 7);
		p.drawRect(pt.x()-8, pt.y()-1, 7, 2);
		p.drawRect(pt.x()+1, pt.y()-1, 7, 2);
	}
	crossPos = pt;
}

void Calibrate::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	int y;

	if (!logo.isNull())
	{
		y = height() / 2 - logo.height() - 15;
		p.drawPixmap((width() - logo.width())/2, y, logo);
	}
	else
		qDebug("logo is Null");

	y = height() / 2 + 15;

	saveUnder = QPixmap::grabWindow(winId(), crossPos.x()-8, crossPos.y()-8, 16, 16);
	moveCrosshair(crossPos);
}

void Calibrate::mousePressEvent(QMouseEvent *e)
{
	// map to device coordinates
	qWarning("Calibrate::mousePressEvent");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QPoint devPos = qt_screen->mapToDevice(e->pos(),
	QSize(qt_screen->width(), qt_screen->height()));

	if (penPos.isNull())
		penPos = devPos;
	else
		penPos = QPoint((penPos.x() + devPos.x())/2, (penPos.y() + devPos.y())/2);
#endif
}

QPushButton *Calibrate::drawButton(int x, int y)
{
	QFont aFont;
	FontManager::instance()->getFont(font_homepage_bottoni_label, aFont);

	QPushButton *b = new QPushButton(this);
	b->setText(tr("Click here!"));
	b->setFont(aFont);
	b->setGeometry(x, y, 100, 50);
	b->show();
	return b;
}

void Calibrate::showButton1()
{
	qWarning("Calibrate::showButton1");
	timer->stop();
	timer->disconnect();
	timer->start(BUTTON_SEC_TIMEOUT * 1000, true);
	connect(timer, SIGNAL(timeout()), SLOT(rollbackCalibration()));

	QPushButton *b = drawButton(20, 20);
	connect(b, SIGNAL(clicked()), SLOT(showButton2()));
	connect(b, SIGNAL(clicked()), b, SLOT(hide()));
	show_crosshair = false;
	update();
}

void Calibrate::showButton2()
{
	qWarning("Calibrate::showButton2");

	QPushButton *b = drawButton(120, 250);
	connect(b, SIGNAL(clicked()), SLOT(endCalibration()));
}

void Calibrate::rollbackCalibration()
{
	qWarning("Calibrate::rollbackCalibration");
	if (QFile::exists("/etc/pointercal.calibrated"))
		system("cp /etc/pointercal.calibrated /etc/pointercal");
	else
		qWarning("Cannot found backup calibration!");

	endCalibration();
}

void Calibrate::endCalibration()
{
	qWarning("Calibrate::endCalibration");
	if (QFile::exists("/etc/pointercal.calibrated"))
		system("rm /etc/pointercal.calibrated");

	hide();
	close();

	emit (fineCalib());
	if (manut==1)
		delete(this);
}

void Calibrate::mouseReleaseEvent(QMouseEvent *)
{
	if (timer->isActive())
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
	//if (location < QWSPointerCalibrationData::LastLocation) {
	if (location < lastLoc)
	{
		qWarning("location < lastLoc    ovvero %d<%d",location,lastLoc);
		location = (QWSPointerCalibrationData::Location)((int)location + 1);
		qWarning("new location: %d", location);
	}
	else
	{
		qWarning("sto per fare Sanity Chack");
		if (sanityCheck())
		{
			releaseMouse();
			QWSServer::mouseHandler()->calibrate(&cd);
			doMove = FALSE;
			if (!manut)
				showButton1();
			else
				endCalibration();
		}
		else
			location = QWSPointerCalibrationData::TopLeft;
    }

	if (doMove)
	{
		QPoint target = fromDevice(cd.screenPoints[location]);
		dx = (target.x() - crossPos.x())/10;
		dy = (target.y() - crossPos.y())/10;
		timer->start(30);
	}
#endif	
}

void Calibrate::timeout()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QPoint target = fromDevice(cd.screenPoints[location]);

	bool doneX = FALSE;
	bool doneY = FALSE;
	QPoint newPos(crossPos.x() + dx, crossPos.y() + dy);

	if (QABS(crossPos.x() - target.x()) <= QABS(dx))
	{
		newPos.setX(target.x());
		doneX = TRUE;
	}

	if (QABS(crossPos.y() - target.y()) <= QABS(dy))
	{
		newPos.setY(target.y());
		doneY = TRUE;
	}

	if (doneX && doneY)
	{
		penPos = QPoint();
		timer->stop();
	}
	moveCrosshair(newPos);
	#endif
}

