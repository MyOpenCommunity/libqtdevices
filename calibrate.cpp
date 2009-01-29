#include "calibrate.h"
#include "main.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"

#include <QDesktopWidget>
#include <QGlobalStatic> // qAbs
#include <QApplication>
#include <QMouseEvent>
#include <QWSServer>
#include <QVariant> // setProperty
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <QLabel>
#include <QFile>
#include <QDebug>

#define BUTTON_SEC_TIMEOUT 10

#define IMG_OK IMG_PATH "btnok.png"


Calibrate::Calibrate(QWidget* parent, unsigned char m) :
	QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint), button_test(false)
{
	setProperty("noStyle", true);
	setAttribute(Qt::WA_DeleteOnClose);
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
	newPos = crossPos;
#endif
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	button_timer = new QTimer(this);
	b1 = createButton(IMG_OK, 5, 5);
	b2 = createButton(IMG_OK, 175, 255);
	box_text = new QLabel(this);
	box_text->setFont(bt_global::font.get(FontManager::TEXT));
	box_text->setAlignment(Qt::AlignHCenter);
	box_text->setGeometry(0, 205, desk.width(), 50);
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	if (QFile::exists("/etc/pointercal"))
		system("cp /etc/pointercal /etc/pointercal.calibrated");
#endif
	manut = m;
	startCalibration();
	connect(button_timer, SIGNAL(timeout()), SLOT(rollbackCalibration()));
	connect(b1, SIGNAL(clicked()), b2, SLOT(show()));
	connect(b1, SIGNAL(clicked()), b1, SLOT(hide()));
	connect(b2, SIGNAL(clicked()), SLOT(endCalibration()));
	connect(b2, SIGNAL(clicked()), b2, SLOT(hide()));
}

Calibrate::~Calibrate()
{
	releaseMouse();
}

BtButton *Calibrate::createButton(const char* icon_name, int x, int y)
{
	BtButton *b = new BtButton(this);
	b->setImage(icon_name);
	b->move(x, y);
	return b;
}

void Calibrate::startCalibration()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QWSServer::mouseHandler()->clearCalibration();
	location = QWSPointerCalibrationData::TopLeft;
#endif
	button_test = false;
	grabMouse();
	b1->hide();
	b2->hide();
	box_text->setText(tr("Click the crosshair"));
}

QPoint Calibrate::fromDevice(const QPoint &p)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	qDebug("inside Calibrate::fromDevice");
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

	int vl = qAbs(tl.x() - bl.x());
	int vr = qAbs(tr.x() - br.x());

	int diff = qAbs(vl - vr);

	int avg1 = (tl.x() + bl.x()) / 2;
	int avg2 = (tr.x() + br.x()) / 2;
	if (avg1 > avg2)
		avg2=avg1;

    if ((vl > avg2/10)||(vr > avg2/10))
	{
		qWarning("err>10 per cento ->  left: %d right: %d\n",vl/avg2*100 ,vr/avg2*100);
		return false;
	}
	int ht = qAbs(tl.y() - tr.y());
	int hb = qAbs(br.y() - bl.y());
	diff = qAbs(ht - hb);
	avg1 = (tl.y() + tr.y()) / 2;
	avg2 = (br.y() + bl.y()) / 2;
	if (avg1 > avg2)
		avg2=avg1;
	if ((ht > avg2/10)||(hb > avg2/10))
	{
		qWarning("err>10 per cento top: %d bottom: %d\n",ht/avg2*100 ,hb/avg2*100);
		return false;
	}

	if(tl.x() < tr.x())
	{
		qWarning("err tl.x > tr.x\n");
		return false;
	}

	if(bl.x() < br.x())
	{
		qWarning("err bl.x > br.x\n");
		return false;
	}

	if(tl.y() > bl.y())
	{
		qWarning("err tl.y < bl.y\n");
		return false;
	}

	if(tr.y() > br.y())
	{
		qWarning("err tr.y > br.y\n");
		return false;
	}
#endif
	qDebug("return TRUE Calibrate::sanityCheck");
	return true;
}

void Calibrate::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	int y;

	if (!logo.isNull())
	{
		y = height() / 2 - logo.height() + 55;
		p.drawPixmap((width() - logo.width())/2, y, logo);
	}
	else
		qDebug("logo is Null");

	y = height() / 2 + 15;

	if (!button_test)
	{
		p.drawRect(newPos.x()-1, newPos.y()-8, 2, 7);
		p.drawRect(newPos.x()-1, newPos.y()+1, 2, 7);
		p.drawRect(newPos.x()-8, newPos.y()-1, 7, 2);
		p.drawRect(newPos.x()+1, newPos.y()-1, 7, 2);
	}
	crossPos = newPos;
}

void Calibrate::buttonsTest()
{
	qDebug("Calibrate::buttonsTest");
	button_timer->setSingleShot(true);
	button_timer->start(BUTTON_SEC_TIMEOUT * 1000);
	box_text->setText(tr("Click the OK button"));
	button_test = true;
	b1->show();
	update();
}

void Calibrate::rollbackCalibration()
{
	qDebug("Calibrate::rollbackCalibration");
	if (QFile::exists("/etc/pointercal.calibrated"))
		system("cp /etc/pointercal.calibrated /etc/pointercal");
	else
		qWarning("Cannot found backup calibration!");

	startCalibration();
	trackCrosshair();
	update();
}

void Calibrate::endCalibration()
{
	qDebug("Calibrate::endCalibration");
	if (QFile::exists("/etc/pointercal.calibrated"))
		system("rm /etc/pointercal.calibrated");

	hide();
	close();

	emit fineCalib();
	if (manut == 1)
		delete(this);
}

void Calibrate::trackCrosshair()
{
	QPoint target = fromDevice(cd.screenPoints[location]);
	dx = (target.x() - crossPos.x())/10;
	dy = (target.y() - crossPos.y())/10;
	timer->start(30);
}

void Calibrate::mousePressEvent(QMouseEvent *e)
{
	if (button_test)
		return;
	// map to device coordinates
	qDebug("Calibrate::mousePressEvent");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QPoint devPos = qt_screen->mapToDevice(e->pos(),
	QSize(qt_screen->width(), qt_screen->height()));

	if (penPos.isNull())
		penPos = devPos;
	else
		penPos = QPoint((penPos.x() + devPos.x())/2, (penPos.y() + devPos.y())/2);
#endif
}

void Calibrate::mouseReleaseEvent(QMouseEvent *)
{
	qDebug("Calibrate::mouseReleaseEvent");
	if (timer->isActive() || button_test)
		return;

	bool doMove = true;

#if defined (BTWEB) ||  defined (BT_EMBEDDED)

	cd.devPoints[location] = penPos;

	unsigned char lastLoc;
	if (manut == 0)
		lastLoc = QWSPointerCalibrationData::LastLocation;
	else
		lastLoc = QWSPointerCalibrationData::LastLocation - 1;
	qDebug("il mio stato di manutenzione è: %d - e lastLoc= %d", manut, lastLoc);
	if (location < lastLoc)
	{
		qDebug("location < lastLoc ovvero %d<%d",location,lastLoc);
		location = (QWSPointerCalibrationData::Location)((int)location + 1);
		qDebug("new location: %d", location);
	}
	else
	{
		if (sanityCheck())
		{
			releaseMouse();
			QWSServer::mouseHandler()->calibrate(&cd);
			doMove = false;
			if (!manut)
				buttonsTest();
			else
				endCalibration();
		}
		else
			location = QWSPointerCalibrationData::TopLeft;
    }

	if (doMove)
		trackCrosshair();
#endif
}

void Calibrate::timeout()
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QPoint target = fromDevice(cd.screenPoints[location]);

	bool doneX = false;
	bool doneY = false;
	newPos = QPoint(crossPos.x() + dx, crossPos.y() + dy);

	if (qAbs(crossPos.x() - target.x()) <= qAbs(dx))
	{
		newPos.setX(target.x());
		doneX = true;
	}

	if (qAbs(crossPos.y() - target.y()) <= qAbs(dy))
	{
		newPos.setY(target.y());
		doneY = true;
	}

	if (doneX && doneY)
	{
		penPos = QPoint();
		timer->stop();
	}
	update();
#endif
}

