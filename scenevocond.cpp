#include "scenevocond.h"
#include "generic_functions.h" // setCfgValue
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "main.h" // bt_global::config
#include "skinmanager.h"
#include "icondispatcher.h" // icons_cache
#include "xml_functions.h" //getTextChild
#include "scenevodevicescond.h"

#include <QLabel>
#include <QLocale>
#include <QDebug>

#define BOTTOM_BORDER 10


/*****************************************************************
** Advanced scenario management generic condition
****************************************************************/

scenEvo_cond::scenEvo_cond()
{
	hasTimeCondition = false;
}

const char *scenEvo_cond::getDescription()
{
	return "Generic scenEvo condition";
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

void scenEvo_cond::inizializza()
{
}

bool scenEvo_cond::isTrue()
{
	return false;
}


/*****************************************************************
 ** Advanced scenario management, time condition
****************************************************************/

scenEvo_cond_h::scenEvo_cond_h(int _item_id, const QDomNode &config_node, bool has_next) :
	time_edit(this)
{
	qDebug("***** scenEvo_cond_h::scenEvo_cond_h");

	item_id = _item_id;

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(scaduta()));

	const int BUT_DIM = 60;
	// Top icon
	QLabel *image = new QLabel(this);
	image->setPixmap(bt_global::skin->getImage("watch"));
	image->setGeometry(width()/2 - BUT_DIM/2, 0, BUT_DIM, BUT_DIM);

	QString h = getTextChild(config_node, "hour");
	QString m = getTextChild(config_node, "minute");
	time_edit.setMaxHours(24);
	time_edit.setMaxMinutes(60);
	const int TIME_EDIT_W = width() / 2;
	// +10 below is for compatibility reasons
	const int TIME_EDIT_H = height() / 2 + 10;
	time_edit.setGeometry(width()/2 - TIME_EDIT_W/2, height()/2 - TIME_EDIT_H/2, TIME_EDIT_W, TIME_EDIT_H);
	time_edit.setTime(QTime(h.toInt(), m.toInt(), 0));

	bottom_left = new BtButton(this);
	bottom_left->setGeometry(0, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);
	bottom_left->setImage(bt_global::skin->getImage("ok"));
	connect(bottom_left, SIGNAL(released()), SLOT(OK()));

	bottom_right = new BtButton(this);
	bottom_right->setGeometry(width() - BUT_DIM, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);

	if (has_next)
	{
		bottom_center = new BtButton(this);
		bottom_center->setGeometry(width()/2 - BUT_DIM/2, height() - (BUT_DIM + BOTTOM_BORDER), BUT_DIM, BUT_DIM);
		bottom_center->setImage(bt_global::skin->getImage("back"));
		connect(bottom_center, SIGNAL(released()),SLOT(Prev()));

		bottom_right->setImage(bt_global::skin->getImage("forward"));
		connect(bottom_right, SIGNAL(released()), SLOT(Next()));
	}
	else
	{
		bottom_right->setImage(bt_global::skin->getImage("back"));
		connect(bottom_right, SIGNAL(released()), SLOT(Prev()));
	}

	hasTimeCondition = true;
	cond_time.setHMS(h.toInt(), m.toInt(), 0);
	setupTimer();
}

const char *scenEvo_cond_h::getDescription()
{
	return "scenEvo hour condition";
}

void scenEvo_cond_h::setupTimer()
{
	QTime now = QTime::currentTime();
	int msecsto = now.msecsTo(cond_time);

	while (msecsto <= 0) // Do it tomorrow
		msecsto += 24 * 60 * 60 * 1000;

	while (msecsto >=  24 * 60 * 60 * 1000)
		msecsto -= 24 * 60 * 60 * 1000;

	qDebug("(re)starting timer with interval = %d", msecsto);
	timer.stop();
	timer.start(msecsto);
}

void scenEvo_cond_h::Apply()
{
	BtTime tmp = time_edit.time();
	cond_time.setHMS(tmp.hour(), tmp.minute(), 0);
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
#ifdef CONFIG_BTOUCH
	data["condH/hour"] = cond_time.toString("hh");
	data["condH/minute"] = cond_time.toString("mm");
	setCfgValue(data, SCENARIO_EVOLUTO, get_serial_number());
#else
	data["scen/time/hour"] = cond_time.toString("hh");
	data["scen/time/minute"] = cond_time.toString("mm");
	setCfgValue(data, item_id);
#endif
}

void scenEvo_cond_h::reset()
{
	time_edit.setTime(cond_time);
}

bool scenEvo_cond_h::isTrue()
{
	QTime cur = QDateTime::currentDateTime().time();
	return ((cond_time.hour() == cur.hour()) &&
			(cond_time.minute() == cur.minute()));
}

/*****************************************************************
** Advanced scenario management, device condition
****************************************************************/

scenEvo_cond_d::scenEvo_cond_d(int _item_id, const QDomNode &config_node)
{
	item_id = _item_id;

	QLabel *area1_ptr = new QLabel(this);
	area1_ptr->setGeometry(0, 0, BUTTON_DIM, BUTTON_DIM);

	QLabel *area2_ptr = new QLabel(this);
	area2_ptr->setGeometry(BUTTON_DIM, BUTTON_DIM/2 - TEXT_Y_DIM/2, width() - BUTTON_DIM, TEXT_Y_DIM);
	area2_ptr->setFont(bt_global::font->get(FontManager::TEXT));
	area2_ptr->setAlignment(Qt::AlignCenter);
	area2_ptr->setText(getTextChild(config_node, "descr"));

	// create condition buttons
	BtButton *condition_up = new BtButton(this);
	condition_up->setGeometry(width()/2 - BUTTON_DIM/2, 80, BUTTON_DIM, BUTTON_DIM);
	condition_up->setImage(bt_global::skin->getImage("plus"));
	connect(condition_up, SIGNAL(clicked()), SLOT(Up()));

	BtButton *condition_down = new BtButton(this);
	condition_down->setGeometry(width()/2 - BUTTON_DIM/2, 190, BUTTON_DIM, BUTTON_DIM);
	condition_down->setImage(bt_global::skin->getImage("minus"));
	connect(condition_down, SIGNAL(clicked()), SLOT(Down()));

	// create bottom (navigation) buttons
	BtButton *b = new BtButton(this);
	b->setGeometry(0, height() - (BUTTON_DIM + BOTTOM_BORDER), BUTTON_DIM, BUTTON_DIM);
	b->setImage(bt_global::skin->getImage("ok"));
	connect(b, SIGNAL(clicked()), SLOT(OK()));

	b = new BtButton(this);
	b->setGeometry(width() - BUTTON_DIM, height() - (BUTTON_DIM + BOTTOM_BORDER), BUTTON_DIM, BUTTON_DIM);
	b->setImage(bt_global::skin->getImage("back"));
	connect(b, SIGNAL(clicked()), SLOT(Prev()));

	QString trigger = getTextChild(config_node, "trigger");
	// Create actual device condition
	DeviceCondition *dc;
#ifdef CONFIG_BTOUCH
	int condition_type = getTextChild(config_node, "value").toInt();
#else
	int condition_type = getTextChild(config_node, "objectID").toInt();
#endif
	qDebug("#### Condition type = %d", condition_type);
	QString icon;
	QString w = getTextChild(config_node, "where");
	bool external = false;
	DeviceConditionDisplay *condition_display;

	switch (condition_type)
	{
	case 1:
		condition_display = new DeviceConditionDisplayOnOff(this);
		dc = new DeviceConditionLight(condition_display, trigger, w);
		icon = bt_global::skin->getImage("light");
		break;
	case 2:
		condition_display = new DeviceConditionDisplayDimming(this);
		dc = new DeviceConditionDimming(condition_display, trigger, w);
		icon = bt_global::skin->getImage("dimmer");
		break;
	case 7:
		external = true;
		w += "00";
	case 3:
	case 8:
		condition_display = new DeviceConditionDisplayTemperature(this);
		dc = new DeviceConditionTemperature(condition_display, trigger, w, external);
		condition_up->setAutoRepeat(true);
		condition_down->setAutoRepeat(true);
		icon = bt_global::skin->getImage("probe");
		break;
	case 9:
		condition_display = new DeviceConditionDisplayOnOff(this);
		dc = new DeviceConditionAux(condition_display, trigger, w);
		icon = bt_global::skin->getImage("aux");
		break;
	case 4:
		condition_display = new DeviceConditionDisplayVolume(this);
		dc = new DeviceConditionVolume(condition_display, trigger, w);
		icon = bt_global::skin->getImage("amplifier");
		break;
	case 6:
		condition_display = new DeviceConditionDisplayDimming(this);
		dc = new DeviceConditionDimming100(condition_display, trigger, w);
		icon = bt_global::skin->getImage("dimmer");
		break;
	default:
		qDebug("Unknown device condition");
		dc = NULL;
	}
	area1_ptr->setPixmap(*bt_global::icons_cache.getIcon(icon));

	if (dc)
	{
		condition_display->setGeometry(40,140,160,50);
		connect(dc, SIGNAL(condSatisfied()), SIGNAL(condSatisfied()));
	}
	actual_condition = dc;
}

const char *scenEvo_cond_d::getDescription()
{
	return "scenEvo device condition";
}

void scenEvo_cond_d::Up()
{
	qDebug("scenEvo_cond_d::Up()");
	actual_condition->Up();
}

void scenEvo_cond_d::Down()
{
	qDebug("scenEvo_cond_d::Down()");
	actual_condition->Down();
}

void scenEvo_cond_d::Apply()
{
	actual_condition->OK();
}

void scenEvo_cond_d::OK()
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
#ifdef CONFIG_BTOUCH
	setCfgValue("condDevice/trigger", s, SCENARIO_EVOLUTO, get_serial_number());
#else
	setCfgValue("scen/device/trigger", s, item_id);
#endif
	reset();
	inizializza();
}

void scenEvo_cond_d::reset()
{
	qDebug("scenEvo_cond_d::reset()");
	actual_condition->reset();
}

void scenEvo_cond_d::inizializza()
{
	if (actual_condition)
		actual_condition->inizializza();
}

bool scenEvo_cond_d::isTrue()
{
	return actual_condition ? actual_condition->isTrue() : false;
}

