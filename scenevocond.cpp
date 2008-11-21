#include "scenevocond.h"
#include "device.h"
#include "device_cache.h"
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "timescript.h"
#include "fontmanager.h"
#include "scaleconversion.h"

#include <QDateTime>
#include <QPixmap>
#include <QWidget>
#include <QCursor>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QDir>
#include <QFile>

#include <assert.h>

/*****************************************************************
** Advanced scenario management generic condition
****************************************************************/

// TODO: vedere se puo' essere trasformato in un QWidget!
scenEvo_cond::scenEvo_cond(QWidget *parent) : QFrame(parent)
{
	val = -1;
	for (int i = 0; i < MAX_EVO_COND_IMG; i++)
		img[i] = new QString("");
	hasTimeCondition = false;
}

QString scenEvo_cond::getImg(int index)
{
	if (index >= MAX_EVO_COND_IMG)
		return "";
	return *img[index];
}

void scenEvo_cond::setImg(int index, QString s)
{
	qDebug() << "scenEvo_cond: setting image " << index << " to " << s;
	if (index >= MAX_EVO_COND_IMG)
		return;
	*img[index] = s;
}

int scenEvo_cond::getVal(void)
{
	return val;
}

void scenEvo_cond::setVal(int v)
{
	val = v;
}

const char *scenEvo_cond::getDescription(void)
{
	return "Generic scenEvo condition";
}

void scenEvo_cond::mostra()
{
	// Does nothing by default
	qDebug("scenEvo_cond::mostra()");
}

void scenEvo_cond::SetIcons()
{
	// Does nothing by default
	qDebug("scenEvo_cond::SetIcons()");
}

void scenEvo_cond::Next()
{
	qDebug("scenEvo_cond::Next()");
	emit SwitchToNext();
}

void scenEvo_cond::Prev()
{
	qDebug("scenEvo_cond::Prev()");
	reset();
	emit resetAll();
	emit SwitchToFirst();
}

void scenEvo_cond::OK()
{
	qDebug("scenEvo_cond::OK()");
	save();
	emit SwitchToFirst();
}

void scenEvo_cond::Apply()
{
	qDebug("scenEvo_cond::Apply()");
}

void scenEvo_cond::save()
{
	qDebug("scenEvo_cond::save()");
}

void scenEvo_cond::reset()
{
	qDebug("scenEvo_cond::reset()");
}

int scenEvo_cond::get_serial_number()
{
	return serial_number;
}

void scenEvo_cond::set_serial_number(int n)
{
	serial_number = n;
}

void scenEvo_cond::inizializza(void)
{
}

bool scenEvo_cond::isTrue(void)
{
	return false;
}


/*****************************************************************
 ** Advanced scenario management, time condition
****************************************************************/

scenEvo_cond_h::scenEvo_cond_h(QWidget *parent) : scenEvo_cond(parent)
{
	qDebug("***** scenEvo_cond_h::scenEvo_cond_h");
	h = new QString("");
	m = new QString("");
	s = new QString("");
	ora = NULL;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(scaduta()));
	cond_time = new QDateTime(QDateTime::currentDateTime());
	ora = new timeScript(this, 2 , cond_time);
	hasTimeCondition = true;
}

void scenEvo_cond_h::set_h(QString _h)
{
	*h = _h;
	qDebug() << "scenEvo_cond_h::set_h : " << h;
}

void scenEvo_cond_h::set_m(QString _m)
{
	*m = _m;
	qDebug() << "scenEvo_cond_h::set_m : " << m;
	QTime t(h->toInt(), m->toInt(), 0);
	cond_time->setTime(t);
	ora->setDataOra(QDateTime(QDate::currentDate(), t));
	ora->showTime();
	setupTimer();
}

void scenEvo_cond_h::set_s(QString _s)
{
	*s = _s;
	qDebug() << "scenEvo_cond_h::set_s : " << s;
}

const char *scenEvo_cond_h::getDescription(void)
{
	return "scenEvo hour condition";
}

void scenEvo_cond_h::SetIcons()
{
	qDebug("scenEvo_cond_h::SetIcons()");
	for (int i = 0; i < 6; i++)
		qDebug() << "icon[" << i << "] = " << getImg(i);
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	for (uchar idx = 0; idx < 2; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry(idx*80+50, 80, 60, 60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_SU);
	}

	// Pulsanti giu`
	for (uchar idx = 2; idx < 4; idx++)
	{
		but[idx] = new BtButton(this);
		but[idx]->setGeometry((idx-2)*80+50,190,60,60);
		but[idx]->setAutoRepeat(true);
		but[idx]->setImage(ICON_FRECCIA_GIU);
	}

	// Orologio
	Immagine = new QLabel(this);
	QPixmap Icon1;
	if (Icon1.load(getImg(0)))
		Immagine->setPixmap(Icon1);
	Immagine->setGeometry(90,0,60,60);

	// Pulsante in basso a sinistra, area 6 (SE C'E` L'ICONA)
	if (!getImg(A6_ICON_INDEX).isEmpty())
	{
		qDebug() << "Area 6: loaded icon " << getImg(A6_ICON_INDEX);
		but[A6_BUTTON_INDEX] = new BtButton(this);
		but[A6_BUTTON_INDEX]->setGeometry(0, MAX_HEIGHT - 60, 60, 60);
		but[A6_BUTTON_INDEX]->setImage(getImg(A6_ICON_INDEX));
	}
	else
		but[A6_BUTTON_INDEX] = NULL;

	// Pulsante in basso al centro, area 7
	if (!getImg(A7_ICON_INDEX).isEmpty())
	{
		qDebug() << "Area 7: loaded icon " << getImg(A7_ICON_INDEX);
		but[A7_BUTTON_INDEX] = new BtButton(this);
		but[A7_BUTTON_INDEX]->setGeometry(MAX_WIDTH/2 - 30, MAX_HEIGHT - 60, 60, 60);
		but[A7_BUTTON_INDEX]->setImage(getImg(A7_ICON_INDEX));
	}
	else
		but[A7_BUTTON_INDEX] = NULL;

	// Pulsante in basso a destra, area 8
	if (!getImg(A8_ICON_INDEX).isEmpty())
	{
		qDebug() << "Area 8: loaded icon " << getImg(A8_ICON_INDEX);
		but[A8_BUTTON_INDEX] = new BtButton(this);
		but[A8_BUTTON_INDEX]->setGeometry(MAX_WIDTH - 60, MAX_HEIGHT - 60, 60, 60);
		but[A8_BUTTON_INDEX]->setImage(getImg(A8_ICON_INDEX));
	}
	else
		but[A8_BUTTON_INDEX] = NULL;
	ora->setGeometry(40, 140, 160, 50);
	ora->setFrameStyle(QFrame::Plain);
	ora->setLineWidth(0);
	qDebug("scenEvo_cond_h::SetIcons(), fine");
}

void scenEvo_cond_h::mostra()
{
	qDebug("scenEvo_cond_h::mostra()");
	for (uchar idx = 0; idx < 8; idx++)
		if (but[idx])
			but[idx]->show();
	ora->show();
	Immagine->show();

	show();
	disconnect(but[0] ,SIGNAL(clicked()), ora, SLOT(aumOra()));
	disconnect(but[1] ,SIGNAL(clicked()), ora, SLOT(aumMin()));
	disconnect(but[2] ,SIGNAL(clicked()), ora, SLOT(diminOra()));
	disconnect(but[3] ,SIGNAL(clicked()), ora, SLOT(diminMin()));
	connect(but[0] , SIGNAL(clicked()), ora, SLOT(aumOra()));
	connect(but[1] , SIGNAL(clicked()), ora, SLOT(aumMin()));
	connect(but[2] ,SIGNAL(clicked()), ora, SLOT(diminOra()));
	connect(but[3] ,SIGNAL(clicked()), ora, SLOT(diminMin()));

	if (but[A6_BUTTON_INDEX])
	{
		disconnect(but[A6_BUTTON_INDEX], SIGNAL(released()), this, SLOT(OK()));
		connect(but[A6_BUTTON_INDEX], SIGNAL(released()), this, SLOT(OK()));
	}
	if (getImg(3).isEmpty())
	{
	// cimg4 is empty
		if (but[A8_BUTTON_INDEX])
		{
			qDebug("connecting A8 to Prev");
			disconnect(but[A8_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Prev()));
			connect(but[A8_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Prev()));
		}
	}
	else
	{
		if (but[A7_BUTTON_INDEX])
		{
			qDebug("connecting A7 to Prev");
			disconnect(but[A7_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Prev()));
			connect(but[A7_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Prev()));
		}
		if (but[A8_BUTTON_INDEX])
		{
			qDebug("connecting A8 to Next");
			disconnect(but[A8_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Next()));
			connect(but[A8_BUTTON_INDEX], SIGNAL(released()),this, SLOT(Next()));
		}
	}
}

void scenEvo_cond_h::setupTimer()
{
	QTime now = QTime::currentTime();
	int msecsto = now.msecsTo(cond_time->time());

	while (msecsto <= 0) // Do it tomorrow
		msecsto += 24 * 60 * 60 * 1000;

	while (msecsto >=  24 * 60 * 60 * 1000)
		msecsto -= 24 * 60 * 60 * 1000;

	qDebug("(re)starting timer with interval = %d", msecsto);
	timer->stop();
	timer->setSingleShot(true);
	timer->start(msecsto);
}

void scenEvo_cond_h::Apply()
{
	*cond_time = ora->getDataOra();
	setupTimer();
}

void scenEvo_cond_h::OK()
{
	qDebug("scenEvo_cond_h::OK()");
	Apply();
	scenEvo_cond::OK();
}

void scenEvo_cond_h::scaduta()
{
	qDebug("scenEvo_cond_h::scaduta()");
	emit verificata();
	setupTimer();
}

void scenEvo_cond_h::save()
{
	qDebug("scenEvo_cond_h::save()");

	QMap<QString, QString> data;
	data["hour"] = cond_time->time().toString("hh");
	data["minute"] = cond_time->time().toString("mm");
	setCfgValue(data, SCENARIO_EVOLUTO, get_serial_number());
}

void scenEvo_cond_h::reset()
{
	qDebug("scenEvo_cond_h::reset()");
	ora->setDataOra(*cond_time);
	ora->showTime();
}

bool scenEvo_cond_h::isTrue(void)
{
	QTime cur = QDateTime::currentDateTime().time();
	return ((cond_time->time().hour() == cur.hour()) &&
			(cond_time->time().minute() == cur.minute()));
}

/*****************************************************************
** Advanced scenario management, device condition
****************************************************************/

scenEvo_cond_d::scenEvo_cond_d(QWidget *parent) : scenEvo_cond(parent)
{
	qDebug("scenEvo_cond_d::scenEvo_cond_d()");
	descr = new QString("");
	where = new QString("");
	trigger = new QString("");
	memset(but, 0, sizeof(but));
	qDebug("scenEvo_cond_d::scenEvo_cond_d(), end");
}

void scenEvo_cond_d::set_descr(QString d)
{
	qDebug() << "scenEvo_cond_d::set_descr("<< d << ")";
	*descr = d;
}

void scenEvo_cond_d::set_where(QString w)
{
	qDebug() << "scenEvo_cond_d::set_where(" << w << ")";
	*where = w;
}

void scenEvo_cond_d::get_where(QString& out)
{
	out = *where;
}

void scenEvo_cond_d::set_trigger(QString t)
{
	qDebug() << "scenEvo_cond_d::set_trigger(" << t << ")";
	*trigger = t;
}

const char *scenEvo_cond_d::getDescription(void)
{
	return "scenEvo device condition";
}

void scenEvo_cond_d::mostra()
{
	qDebug("scenEvo_cond_d::mostra()");
	for (uchar idx = 0; idx < 8; idx++)
		if (but[idx])
			but[idx]->show();

	area1_ptr->show();
	area2_ptr->setText(*descr);
	area2_ptr->show();
	if (actual_condition)
		actual_condition->show();
	show();
}

void scenEvo_cond_d::SetButtonIcon(int icon_index, int button_index)
{
	if (getImg(icon_index).isEmpty())
	{
		but[button_index] = NULL;
		return;
	}

	if (QFile::exists(getImg(icon_index)))
	{
		if (but[button_index])
			but[button_index]->setImage(getImg(icon_index));
	}
}

void scenEvo_cond_d::SetIcons()
{
	QFont aFont;
	qDebug("scenEvo_cond_d::SetIcons()");
	QPixmap *Icon1 = new QPixmap();
	for (int i=0; i<6; i++)
		qDebug() << "icon[" << i << "] = " << getImg(i);
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
	area1_ptr = new QLabel(this);
	area1_ptr->setGeometry(0, 0, BUTTON_DIM, BUTTON_DIM);
	area2_ptr = new QLabel(this);
	area2_ptr->setGeometry(BUTTON_DIM, BUTTON_DIM/2 - TEXT_Y_DIM/2,	TEXT_X_DIM, TEXT_Y_DIM);
	FontManager::instance()->getFont(font_scenEvoCond_Area2, aFont);
	area2_ptr->setFont(aFont);
	area2_ptr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	BtButton *b = new BtButton(this);
	but[A3_BUTTON_INDEX] = b;
	b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 80, BUTTON_DIM, BUTTON_DIM);
	connect(b, SIGNAL(clicked()), this, SLOT(Up()));
	b = new BtButton(this);
	but[A4_BUTTON_INDEX] = b;
	b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 190, BUTTON_DIM, BUTTON_DIM);
	connect(b, SIGNAL(clicked()), this, SLOT(Down()));
	b = new BtButton(this);
	but[A5_BUTTON_INDEX] = b;
	b->setGeometry(0, MAX_HEIGHT - BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
	connect(b, SIGNAL(clicked()), this, SLOT(OK()));
	b = new BtButton(this);
	but[A6_BUTTON_INDEX] = b;
	b->setGeometry(MAX_WIDTH - BUTTON_DIM, MAX_HEIGHT - BUTTON_DIM,
		BUTTON_DIM, BUTTON_DIM);
	connect(b, SIGNAL(clicked()), this, SLOT(Prev()));
	// area #1
	if (QFile::exists(getImg(A1_ICON_INDEX)))
		Icon1->load(getImg(A1_ICON_INDEX));
	area1_ptr->setPixmap(*Icon1);
	delete Icon1;
	// area #3
	SetButtonIcon(A3_ICON_INDEX, A3_BUTTON_INDEX);
	// area #4
	SetButtonIcon(A4_ICON_INDEX, A4_BUTTON_INDEX);
	// area #5
	SetButtonIcon(A5_ICON_INDEX, A5_BUTTON_INDEX);
	// area #8
	SetButtonIcon(A6_ICON_INDEX, A6_BUTTON_INDEX);
	// Create actual device condition
	device_condition *dc;
	qDebug("#### Condition type = %d", getVal());
	switch (getVal())
	{
	case 1:
		dc = new device_condition_light_status(this, trigger);
		break;
	case 2:
		dc = new device_condition_dimming(this, trigger);
		break;
	case 3:
	case 7:
	case 8:
		dc = new device_condition_temp(this, trigger);
		but[A3_BUTTON_INDEX]->setAutoRepeat(true);
		but[A4_BUTTON_INDEX]->setAutoRepeat(true);
		break;
	case 9:
		dc = new device_condition_aux(this, trigger);
		break;
	case 4:
		dc = new device_condition_volume(this, trigger);
		break;
	case 6:
		dc = new device_condition_dimming_100(this, trigger);
		break;
	default:
		qDebug("Unknown device condition");
		dc = NULL;
	}

	if (dc)
	{
		dc->setGeometry(40,140,160,50);
		connect(dc, SIGNAL(condSatisfied()), this, SIGNAL(condSatisfied()));
		dc->setup_device(*where);
	}
	actual_condition = dc;
	qDebug("scenEvo_cond_d::SetIcons(), end");
}

void scenEvo_cond_d::Up(void)
{
	qDebug("scenEvo_cond_d::Up()");
	actual_condition->Up();
}

void scenEvo_cond_d::Down(void)
{
	qDebug("scenEvo_cond_d::Down()");
	actual_condition->Down();
}

void scenEvo_cond_d::Apply(void)
{
	actual_condition->OK();
}

void scenEvo_cond_d::OK(void)
{
	qDebug("scenEvo_cond_d::OK()");
	// Save ALL conditions here (not just this one)
	emit okAll();
	emit SwitchToFirst();
}

void scenEvo_cond_d::save()
{
	qDebug("scenEvo_cond_d::save()");
	QString s;
	actual_condition->get_condition_value(s);
	setCfgValue("trigger", s, SCENARIO_EVOLUTO, get_serial_number());
	reset();
	inizializza();
}

void scenEvo_cond_d::reset()
{
	qDebug("scenEvo_cond_d::reset()");
	actual_condition->reset();
}

void scenEvo_cond_d::inizializza(void)
{
	if (actual_condition)
		actual_condition->inizializza();
}

bool scenEvo_cond_d::isTrue(void)
{
	return actual_condition ? actual_condition->isTrue() : false;
}

/*****************************************************************
 ** Actual generic device condition
****************************************************************/
device_condition::device_condition(QWidget *p, QString *s)
{
	qDebug() << "device_condition::device_condition(" << s << ")";
	parent = p;
	satisfied = false;
}

int device_condition::get_min()
{
	return 0;
}

int device_condition::get_max()
{
	return 0;
}

int device_condition::get_step()
{
	return 1;
}

int device_condition::get_divisor()
{
	return 1;
}

bool device_condition::show_OFF_on_zero()
{
	return false;
}

int device_condition::get_condition_value(void)
{
	return cond_value;
}

void device_condition::set_condition_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	cond_value = v;
}

void device_condition::set_condition_value(QString s)
{
	qDebug() << "device_condition::set_condition_value (" << s << ")";
	set_condition_value(s.toInt());
}

void device_condition::get_condition_value(QString& out)
{
	qDebug("device_condition::get_condition_value(QString&)");
	char tmp[100];
	sprintf(tmp, "%d", get_condition_value());
	out = tmp;
}

void device_condition::show()
{
	qDebug("device_condition::show()");
	frame->show();
}

void device_condition::Draw()
{
	QString tmp;
	int v = get_current_value();
	if (show_OFF_on_zero() && !v)
		tmp = tr("OFF");
	else
		tmp = QString("%1%2").arg(v).arg(get_unit());
	((QLabel *)frame)->setText(tmp);
}

void device_condition::setGeometry(int x, int y, int sx, int sy)
{
	frame->setGeometry(x, y, sx, sy);
}

void device_condition::Up()
{
	qDebug("device_condition::Up()");
	int val = get_current_value();
	val += get_step();
	set_current_value(val);
	qDebug("val = %d", get_current_value());
	Draw();
	show();
}

void device_condition::Down()
{
	qDebug("device_condition::Down()");
	int val = get_current_value();
	val -= get_step();
	set_current_value(val);
	Draw();
	show();
}

void device_condition::OK()
{
	qDebug("device_condition::OK()");
	set_condition_value(get_current_value());
}

int device_condition::get_current_value()
{
	return current_value;
}

int device_condition::set_current_value(int v)
{
	if (v > get_max())
		v = get_max();
	if (v < get_min())
		v = get_min();
	current_value = v;
	return current_value;
}

scenEvo_cond_d *device_condition::get_parent()
{
	return (scenEvo_cond_d *)parent;
}

QString device_condition::get_unit()
{
	return "";
}

void device_condition::inizializza()
{
	qDebug("device_condition::inizializza()");
	dev->init(true);
}

void device_condition::reset()
{
	qDebug("device_condition::reset()");
	set_current_value(get_condition_value());
	Draw();
}

bool device_condition::isTrue()
{
	return satisfied;
}

void device_condition::setup_device(QString s)
{
	qDebug() << "device_condition::setup_device(" << s << ")";
	dev->set_where(s);
	// Add the device to cache, or replace it with the instance found in cache
	dev = btouch_device_cache.add_device(dev);
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
}

void device_condition::set_pul(bool p)
{
	dev->set_pul(p);
}

void device_condition::set_group(int g)
{
	dev->set_group(g);
}


/*****************************************************************
** Actual light status device condition
****************************************************************/
device_condition_light_status::device_condition_light_status(QWidget *parent, QString *c) :
	device_condition(parent, c)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_light_status, aFont);
	l->setFont(aFont);

	frame = l;
	set_condition_value(*c);
	set_current_value(device_condition::get_condition_value());
	dev = new light(QString(""));
	Draw();
}

QString device_condition_light_status::get_string()
{
	return get_current_value() ? tr("ON") : tr("OFF");
}

void device_condition_light_status::Draw()
{
	((QLabel *)frame)->setText(get_string());
}

void device_condition_light_status::status_changed(QList<device_status*> sl)
{
	int trig_v = device_condition::get_condition_value();
	stat_var curr_status(stat_var::ON_OFF);
	qDebug("device_condition_light_status::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("Light status variation");
			qDebug("trigger value = %d, current value = %d\n", trig_v,
			curr_status.get_val());
			if (trig_v == curr_status.get_val())
			{
				qDebug("light condition (%d) satisfied", trig_v);
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
				qDebug("light condition (%d) NOT satisfied", trig_v);
				satisfied = false;
			}
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("new dimmer status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

int device_condition_light_status::get_max()
{
	return 1;
}

void device_condition_light_status::set_condition_value(QString s)
{
	qDebug("device_condition_light_status::set_condition_value");
	int v = 0;
	if (s == "1")
		v = 1;
	else if (s == "0")
		v = 0;
	else
		qDebug() << "Unknown condition value " << s << " for device_condition_light_status";
	device_condition::set_condition_value(v);
}

void device_condition_light_status::get_condition_value(QString& out)
{
	out = device_condition::get_condition_value() ? "1" : "0";
}


/*****************************************************************
** Actual dimming value device condition
****************************************************************/
device_condition_dimming::device_condition_dimming(QWidget *parent, QString *c) :
	device_condition(parent, c)
{
	qDebug() << "device_condition_dimming::device_condition_dimming(" << c << ")";
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_device_condition_dimming, aFont);
	l->setFont(aFont);

	frame = l;
	if (*c == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		set_condition_value_min((QString) c->at(0));
		QByteArray buf = c->toAscii();
		set_condition_value_max(buf.constData()+2);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	// A dimmer is actually a light
	dev = new dimm(QString(""));
	Draw();
}

QString device_condition_dimming::get_current_value()
{
	char val[50];
	int val_m = get_condition_value_min();
	sprintf(val, "%d", val_m);
	return val;
}

int device_condition_dimming::get_min()
{
	return 0;
}

int device_condition_dimming::get_max()
{
	return 100;
}

int device_condition_dimming::get_step()
{
	return 10;
}


void device_condition_dimming::Up()
{
	qDebug("device_condition_dimming::Up()");

	int val = get_current_value_min();
	switch (val)
	{
	case 0:
		set_current_value_min(2);
		set_current_value_max(4);
		break;
	case 2:
		set_current_value_min(5);
		set_current_value_max(7);
		break;
	case 5:
		set_current_value_min(8);
		set_current_value_max(10);
		break;
	default:
		break;
	}
	Draw();
	show();
}

void device_condition_dimming::Down()
{
	qDebug("device_condition_dimming::Down()");
	int val = get_current_value_min();
	switch (val)
	{
	case 2:
		set_current_value_min(0);
		set_current_value_max(0);
		break;
	case 5:
		set_current_value_min(2);
		set_current_value_max(4);
		break;
	case 8:
		set_current_value_min(5);
		set_current_value_max(7);
		break;
	default:
		break;
	}
	Draw();
	show();
}

void device_condition_dimming::Draw()
{
	QString tmp;
	qDebug("device_condition_dimming::Draw()");
	int v = get_current_value_min();
	if (!v)
		tmp = tr("OFF");
	else
	{
		QString u = get_unit();
		int M = get_current_value_max();
		tmp = QString("%1%2 - %3%4").arg(v*10).arg(u).arg(M*10).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming::OK()
{
	qDebug("device_condition_dimming::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_dimming::reset()
{
	qDebug("device_condition_dimming::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

QString device_condition_dimming::get_unit()
{
	return "%";
}

bool device_condition_dimming::show_OFF_on_zero()
{
	return true;
}

void device_condition_dimming::set_condition_value_min(int s)
{
	qDebug("device_condition_dimming::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_dimming::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_dimming::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_dimming::get_condition_value_min()
{
	return min_val;
}

void device_condition_dimming::set_condition_value_max(int s)
{
	qDebug("device_condition_dimming::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_dimming::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_dimming::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_dimming::get_condition_value_max()
{
	return max_val;
}

int device_condition_dimming::get_current_value_min()
{
	return current_value_min;
}

void device_condition_dimming::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_dimming::get_current_value_max()
{
	return current_value_max;
}

void device_condition_dimming::set_current_value_max(int max)
{
	current_value_max = max;
}

void device_condition_dimming::set_condition_value(QString s)
{
	device_condition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_dimming::get_condition_value()");
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_dimming::status_changed(QList<device_status*> sl)
{
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);

	qDebug("device_condition_dimming::status_changed()");
	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation, ignored");
			break;
			case device_status::DIMMER:
			ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
			qDebug("dimmer status variation");
			qDebug("level = %d", curr_lev.get_val()/10);
			qDebug("trigger value min is %d - max is %d, val10 = %d", trig_v_min, trig_v_max, curr_lev.get_val()/10);
			if ((curr_lev.get_val()/10 >= trig_v_min) && (curr_lev.get_val()/10 <= trig_v_max))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
				qDebug("Condition not triggered");
				satisfied = false;
			}
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

/*****************************************************************
 ** Actual dimming 100 value device condition
****************************************************************/
device_condition_dimming_100::device_condition_dimming_100(QWidget *parent, QString *c) :
device_condition(parent, c)
{
	char sup[10];
	qDebug() << "device_condition_dimming_100::device_condition_dimming_100(" << c << ")";
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_light_status, aFont);
	l->setFont(aFont);
	frame = l;
	if (*c == "0")
	{
		set_condition_value_min(0);
		set_condition_value_max(0);
	}
	else
	{
		QByteArray buf = c->toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	// A dimmer is actually a light
	dev = new dimm100(QString(""));
	Draw();
}

QString device_condition_dimming_100::get_current_value()
{
	char val[50];
	int val_m = get_condition_value_min();
	sprintf(val, "%d", val_m);
	return val;
}

int device_condition_dimming_100::get_min()
{
	return 0;
}

int device_condition_dimming_100::get_max()
{
	return 100;
}

int device_condition_dimming_100::get_step()
{
	return 10;
}

void device_condition_dimming_100::Up()
{
	qDebug("device_condition_dimming_100::Up()");

	int val = get_current_value_min();
	switch (val)
	{
	case 0:
		set_current_value_min(1);
		set_current_value_max(20);
		break;
	case 1:
		set_current_value_min(21);
		set_current_value_max(40);
		break;
	case 21:
		set_current_value_min(41);
		set_current_value_max(70);
		break;
	case 41:
		set_current_value_min(71);
		set_current_value_max(100);
		break;
	default:
		break;
	}
	Draw();
	show();
}

void device_condition_dimming_100::Down()
{
	qDebug("device_condition_dimming_100::Down()");
	int val = get_current_value_min();
	switch (val)
	{
	case 1:
		set_current_value_min(0);
		set_current_value_max(0);
		break;
	case 21:
		set_current_value_min(1);
		set_current_value_max(20);
		break;
	case 41:
		set_current_value_min(21);
		set_current_value_max(40);
		break;
	case 71:
		set_current_value_min(41);
		set_current_value_max(70);
		break;
	default:
		break;
	}
	Draw();
	show();
}

void device_condition_dimming_100::Draw()
{
	QString tmp;
	qDebug("device_condition_dimming_100::Draw()");
	QString u = get_unit();
	int v = get_current_value_min();
	if (!v)
		tmp = tr("OFF");
	else
	{
		int M = get_current_value_max();
		tmp = QString("%1%2 - %3%4").arg(v).arg(u).arg(M).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming_100::OK()
{
	qDebug("device_condition_dimming_100::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_dimming_100::reset()
{
	qDebug("device_condition_dimming_100::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

QString device_condition_dimming_100::get_unit()
{
	return "%";
}

bool device_condition_dimming_100::show_OFF_on_zero()
{
	return true;
}

void device_condition_dimming_100::set_condition_value_min(int s)
{
	qDebug("device_condition_dimming_100::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_dimming_100::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_dimming_100::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_min()
{
	return min_val;
}

void device_condition_dimming_100::set_condition_value_max(int s)
{
	qDebug("device_condition_dimming_100::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_dimming_100::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_dimming_100::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_max()
{
	return max_val;
}

int device_condition_dimming_100::get_current_value_min()
{
	return current_value_min;
}

void device_condition_dimming_100::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_dimming_100::get_current_value_max()
{
	return current_value_max;
}

void device_condition_dimming_100::set_current_value_max(int max)
{
	current_value_max = max;
}

void device_condition_dimming_100::set_condition_value(QString s)
{
	device_condition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming_100::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_dimming_100::get_condition_value()");
	if (get_condition_value_min() == 0)
		sprintf(tmp, "0");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_dimming_100::status_changed(QList<device_status*> sl)
{
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);
	int val10;
	qDebug("device_condition_dimming_100::status_changed()");
	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation, ignored");
			break;
		case device_status::DIMMER:
			ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
			qDebug("dimmer status variation, ignored");
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation");
			ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
			ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
			qDebug("level = %d, speed = %d", curr_lev.get_val(),
			curr_speed.get_val());
			val10 = curr_lev.get_val();
			qDebug("trigger value min is %d - max is %d, val = %d", trig_v_min, trig_v_max, val10);
			if ((val10 >= trig_v_min) && (val10 <= trig_v_max))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
				qDebug("Condition not triggered");
				satisfied = false;
			}
			break;
		case device_status::NEWTIMED:
			qDebug("new timed device status variation, ignored");
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

/*****************************************************************
** Actual volume device condition
****************************************************************/
device_condition_volume::device_condition_volume(QWidget *parent, QString *c) :
	device_condition(parent, c)
{
	char sup[10];
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_light_status, aFont);
	l->setFont(aFont);
	frame = l;
	if (*c == "-1")
	{
		set_condition_value_min(-1);
		set_condition_value_max(-1);
	}
	else
	{
		QByteArray buf = c->toAscii();
		sprintf(sup, "%s", buf.constData());
		strtok(sup, "-");
		set_condition_value_min(sup);
		sprintf(sup, "%s", strchr(buf.constData(), '-')+1);
		set_condition_value_max(sup);
	}
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
	dev = new sound_device(QString(""));
}

void device_condition_volume::set_condition_value_min(int s)
{
	qDebug("device_condition_volume::set_condition_value_min(%d)", s);
	min_val = s;
}

void device_condition_volume::set_condition_value_min(QString s)
{
	qDebug() << "device_condition_volume::set_condition_value_min(" << s << ")";
	min_val = s.toInt();
}

int device_condition_volume::get_condition_value_min()
{
	return min_val;
}

void device_condition_volume::set_condition_value_max(int s)
{
	qDebug("device_condition_volume::set_condition_value_max(%d)", s);
	max_val = s;
}

void device_condition_volume::set_condition_value_max(QString s)
{
	qDebug() << "device_condition_volume::set_condition_value_max(" << s << ")";
	max_val = s.toInt();
}

int device_condition_volume::get_condition_value_max()
{
	return max_val;
}

int device_condition_volume::get_current_value_min()
{
	return current_value_min;
}

void device_condition_volume::set_current_value_min(int min)
{
	current_value_min = min;
}

int device_condition_volume::get_current_value_max()
{
	return current_value_max;
}

void device_condition_volume::set_current_value_max(int max)
{
	current_value_max = max;
}

int device_condition_volume::get_min()
{
	return 0;
}

int device_condition_volume::get_max()
{
	return 31;
}

void device_condition_volume::set_condition_value(QString s)
{
	int v = s.toInt();
	qDebug("setting condition value to %d", v);
	device_condition::set_condition_value(v);
}

void device_condition_volume::get_condition_value(QString& out)
{
	char tmp[100];
	qDebug("device_condition_volume::get_condition_value()");
	if (get_condition_value_min() == -1)
		sprintf(tmp, "-1");
	else
		sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
	out =  tmp;
}

void device_condition_volume::Up()
{
	qDebug("device_condition_volume::Up()");
	int v_m = get_current_value_min();
	int v_M = get_current_value_max();
	qDebug("v_m = %d - v_M = %d", v_m, v_M);
	if (v_m == -1)
	{
		v_m = 0;
		v_M = 31;
	}
	else if (v_m == 0)
	{
		if (v_M == 31)
			v_M = 6;
		else
		{
			v_m = 7;
			v_M = 12;
		}
	}
	else if (v_m == 7)
	{
		v_m = 13;
		v_M = 22;
	}
	else
	{
		v_m = 23;
		v_M = 31;
	}
	qDebug("new value m = %d - M = %d", v_m, v_M);
	set_current_value_min(v_m);
	set_current_value_max(v_M);
	Draw();
	show();
}

void device_condition_volume::Down()
{
	qDebug("device_condition_volume::Down()");
	int v_m = get_current_value_min();
	int v_M = get_current_value_max();
	qDebug("v_m = %d - v_M = %d", v_m, v_M);
	if (v_m == 23)
	{
		v_m = 13;
		v_M = 22;
	}
	else if (v_m == 13)
	{
		v_m = 7;
		v_M = 12;
	}
	else if (v_m == 7)
	{
		v_m = 0;
		v_M = 6;
	}
	else if (v_m == 0)
	{
		if (v_M == 6)
			v_M = 31;
		else
		{
			v_m = -1;
			v_M = -1;
		}
	}
	qDebug("new value m = %d - M = %d", v_m, v_M);
	set_current_value_min(v_m);
	set_current_value_max(v_M);
	Draw();
	show();
}

void device_condition_volume::OK()
{
	qDebug("device_condition_volume::OK()");
	set_condition_value_min(get_current_value_min());
	set_condition_value_max(get_current_value_max());
}

void device_condition_volume::Draw()
{
	QString tmp;
	QString u = get_unit();
	int val_min = get_current_value_min();
	int val_max = get_current_value_max();
	qDebug("device_condition_volume::Draw(), val_min = %d - val_max = %d", val_min, val_max);
	if (val_min == -1)
		tmp = tr("OFF");
	else if (val_min == 0 && val_max == 31)
		tmp = tr("ON");
	else
	{
		int vmin = (val_min == 0 ? 0 : (10 * (val_min <= 15 ? val_min/3 : (val_min-1)/3) + 1));
		int vmax = 10 * (val_max <= 15 ? val_max/3 : (val_max-1)/3);

		tmp = QString("%1%2 - %3%4").arg(vmin).arg(u).arg(vmax).arg(u);
	}
	((QLabel *)frame)->setText(tmp);
}

void device_condition_volume::status_changed(QList<device_status*> sl)
{
	int trig_v_min = get_condition_value_min();
	int trig_v_max = get_condition_value_max();
	stat_var curr_volume(stat_var::AUDIO_LEVEL);
	stat_var curr_stato(stat_var::ON_OFF);
	qDebug("device_condition_volume::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::AMPLIFIER:
			qDebug("Amplifier status change");
			qDebug("Confition value_min = %d - value_max = %d", trig_v_min, trig_v_max);
			ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_volume);
			ds->read(device_status_amplifier::ON_OFF_INDEX, curr_stato);
			qDebug("volume = %d - stato = %d", curr_volume.get_val(), curr_stato.get_val());
			if ((trig_v_min == -1) && (curr_stato.get_val() == 0))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else if ((curr_stato.get_val() == 1) && (curr_volume.get_val() >= trig_v_min) && (curr_volume.get_val() <= trig_v_max))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
			qDebug("Condition not triggered");
			satisfied = false;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}

QString device_condition_volume::get_unit()
{
	return "%";
}

void device_condition_volume::reset()
{
	qDebug("device_condition_volume::reset()");
	set_current_value_min(get_condition_value_min());
	set_current_value_max(get_condition_value_max());
	Draw();
}

/*****************************************************************
** Actual temperature device condition
****************************************************************/
device_condition_temp::device_condition_temp(QWidget *parent, QString *c) :
	device_condition(parent, c)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_light_status, aFont);
	l->setFont(aFont);
	frame = l;

	temp_scale = readTemperatureScale();
	switch (temp_scale)
	{
	case CELSIUS:
		max_temp = bt2Celsius(CONDITION_MAX_TEMP);
		min_temp = bt2Celsius(CONDITION_MIX_TEMP);
		set_condition_value(*c);
		break;
	case FAHRENHEIT:
		max_temp = bt2Fahrenheit(CONDITION_MAX_TEMP);
		min_temp = bt2Fahrenheit(CONDITION_MIX_TEMP);
		set_condition_value(QString::number(bt2Fahrenheit((*c).toInt())));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp_scale = CELSIUS;
		max_temp = bt2Celsius(CONDITION_MAX_TEMP);
		min_temp = bt2Celsius(CONDITION_MIX_TEMP);
		set_condition_value(*c);
	}
	step = 5;

	set_current_value(device_condition::get_condition_value());

	Draw();
	dev = new temperature_probe_notcontrolled(QString(""), false);
}

int device_condition_temp::get_min()
{
	return min_temp;
}

int device_condition_temp::get_max()
{
	return max_temp;
}

int device_condition_temp::get_step()
{
	return step;
}

int device_condition_temp::get_divisor()
{
	return 10;
}

QString device_condition_temp::get_unit()
{
	switch (temp_scale)
	{
	case CELSIUS:
		return TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	case FAHRENHEIT:
		return TEMP_DEGREES"F \2611"TEMP_DEGREES"F";
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		return TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	}
}

void device_condition_temp::Draw()
{
	QString u = get_unit();
	// val is an integer either in Celsius or in Fahrenheit degrees
	int val = get_current_value();
	qDebug("device_condition_temp::Draw(), val = %d", val);

	QString tmp = QString::number(val);
	tmp.insert(tmp.length() - 1, ".");
	tmp += u;

	((QLabel *)frame)->setText(tmp);
}

void device_condition_temp::set_condition_value(QString s)
{
	// s is in bticino 4-digit form, we should translate to an int
	int val = s.toInt();
	switch (temp_scale)
	{
	case CELSIUS:
		val = bt2Celsius(val);
		break;
	case FAHRENHEIT:
		val = bt2Celsius(val);
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		val = bt2Celsius(val);
	}
	device_condition::set_condition_value(val);
}

void device_condition_temp::get_condition_value(QString& out)
{
	// transform an int value to a string in bticino 4-digit form
	int val = device_condition::get_condition_value();
	switch (temp_scale)
	{
	case CELSIUS:
		out = QString::number(celsius2Bt(val));
		break;
	case FAHRENHEIT:
		out = QString::number(fahrenheit2Bt(val));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		out = QString::number(celsius2Bt(val));
	}

	// if the val is positive, QString::number strips the leading zeros
	// we need to put them back to be in 4-digit format
	if (val > 0)
		out.prepend("0");
}

void device_condition_temp::status_changed(QList<device_status*> sl)
{
	// get_condition_value() returns an int, which is Celsius or Fahrenheit
	int trig_v = device_condition::get_condition_value();
	stat_var curr_temp(stat_var::TEMPERATURE);
	qDebug("device_condition_temp::status_changed()");
	qDebug("trig_v = %d", trig_v);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::TEMPERATURE_PROBE:
			qDebug("Temperature changed");
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			qDebug("Current temperature %d", curr_temp.get_val());
			int measured_temp;
			switch (temp_scale)
			{
			case CELSIUS:
				measured_temp = bt2Celsius(curr_temp.get_val());
				break;
			case FAHRENHEIT:
				measured_temp = bt2Fahrenheit(curr_temp.get_val());
				break;
			default:
				qWarning("Wrong temperature scale, defaulting to celsius");
				measured_temp = bt2Celsius(curr_temp.get_val());
			}

			if ((measured_temp >= (trig_v - 10)) &&  (measured_temp <= (trig_v + 10)))
			{
				qDebug("Condition triggered");
				if (!satisfied)
				{
					satisfied = true;
					emit condSatisfied();
				}
			}
			else
			{
				qDebug("Condition not triggered");
				satisfied = false;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}
}


/*****************************************************************
** Aux device condition
****************************************************************/

device_condition_aux::device_condition_aux(QWidget *parent, QString *c) :
	device_condition(parent, c), device_initialized(false), device_value(-1)
{
	QLabel *l = new QLabel(parent);
	l->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QFont aFont;
	FontManager::instance()->getFont(font_scenEvoCond_light_status, aFont);
	l->setFont(aFont);

	frame = l;
	set_condition_value(*c);
	set_current_value(device_condition::get_condition_value());
	dev = new aux_device(QString(""));

	Draw();
}

void device_condition_aux::setup_device(QString s)
{
	device_condition::setup_device(s);
	// The device can be replaced by "add_device" method of device_cache, thus
	// the connection must be after that.
	connect(dev, SIGNAL(status_changed(stat_var)), SLOT(status_changed(stat_var)));
}

void device_condition_aux::Draw()
{
	((QLabel *)frame)->setText(get_current_value() ? tr("ON") : tr("OFF"));
}

void device_condition_aux::check_condition(bool emit_signal)
{
	int trig_v = device_condition::get_condition_value();
	if (trig_v == device_value)
	{
		qDebug("aux condition (%d) satisfied", trig_v);
		if (!satisfied)
		{
			satisfied = true;
			if (emit_signal)
				emit condSatisfied();
		}
	}
	else
	{
		qDebug("aux condition (%d) NOT satisfied", trig_v);
		satisfied = false;
	}
}

void device_condition_aux::status_changed(stat_var status)
{
	qDebug("device_condition_aux::status_changed");
	device_value = status.get_val();
	// We emit signal condSatisfied only if the device is initialized.
	check_condition(device_initialized);
	device_initialized = true;
}

int device_condition_aux::get_max()
{
	return 1;
}

void device_condition_aux::set_condition_value(QString s)
{
	qDebug("device_condition_aux::set_condition_value");
	int v = 0;
	if (s == "1")
		v = 1;
	else if (s == "0")
		v = 0;
	else
		qDebug() << "Unknown condition value " << s << " for device_condition_aux";
	device_condition::set_condition_value(v);
	check_condition(false);
}

void device_condition_aux::status_changed(QList<device_status*> sl)
{
	assert(!"Old status changed on device_condition_aux not implemented!");
}

void device_condition_aux::OK()
{
	qDebug("device_condition_aux::OK()");
	device_condition::OK();
	check_condition(false);
}
