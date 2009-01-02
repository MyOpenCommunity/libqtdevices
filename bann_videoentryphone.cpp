#include "bann_videoentryphone.h"
#include "openclient.h" // Client
#include "device_status.h"
#include "device.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "global.h" // BTouch, btouch_device_cache
#include "btbutton.h"
#include "fontmanager.h"

#include <QDebug>
#include <QLabel>


// Static pointer to call notifier manager
call_notifier_manager *postoExt::cnm = NULL;

// Static pointer to unknown station
call_notifier *postoExt::unknown_notifier = NULL;


postoExt::postoExt(QWidget *parent, QString d, QString Icona1, QString Icona2, QString Icona3, QString Icona4, QString _where,
	QString _light, QString _key, QString _unknown) : bann4tasLab(parent)
{
	where = _where;
	descr = d;
	light = (_light == "1");
	key = (_key == "1");
	unknown = (_unknown == "1");
	qDebug() << "postoExt::postoExt(), unknown = "  << _unknown;

	qDebug("light = %d, key = %d, unknown = %d", light, key, unknown);
	qDebug() << "descr =" << descr << ", where =" << _where;
	SetIcons(Icona2, Icona3, "", "", Icona1);
	if (key)
	{
		key_icon = Icona2;
		connect(this, SIGNAL(sxPressed()), this, SLOT(open_door_clicked()));
	}
	else
		nascondi(BUT1);
	if (light)
	{
		connect(this, SIGNAL(dxPressed()), this, SLOT(stairlight_pressed()));
		connect(this, SIGNAL(dxReleased()), this, SLOT(stairlight_released()));
		light_icon = Icona3;
	}
	else
		nascondi(BUT2);
	close_icon = Icona4;
	impostaAttivo(2);
	Draw();
	qDebug("creating call_notifier");
	call_notifier *cn = new call_notifier(NULL, this);
	qDebug("cnm = %p", cnm);
	if (!cnm)
	{
		qDebug("Creating call notifier manager");
		cnm = new call_notifier_manager();
		connect(cnm, SIGNAL(frame_captured(call_notifier *)),
				this, SLOT(frame_captured_handler(call_notifier *)));
		connect(cnm, SIGNAL(call_notifier_closed(call_notifier *)),
				this, SLOT(call_notifier_closed(call_notifier *)));
		connect(BTouch->client_monitor, SIGNAL(frameIn(char *)),
				cnm, SLOT(gestFrame(char *)));
	}
	cnm->add_call_notifier(cn);
	if (unknown && !unknown_notifier)
	{
		qDebug("Creating unknown station notifier");
		unknown_notifier = new call_notifier(NULL, NULL);
		cnm->set_unknown_call_notifier(unknown_notifier);
	}
}

void postoExt::frame_captured_handler(call_notifier *cn)
{
	qDebug("postoExt::frame_captured_handler()");
	// Just unfreeze
	BTouch->freeze(false);
}

void postoExt::call_notifier_closed(call_notifier *cn)
{
	qDebug("postoExt::call_notifier_closed()");
}

void postoExt::open_door_clicked()
{
	qDebug("postoExt::open_door_clicked()");
	BTouch->sendFrame(QString("*6*10*%1##").arg(where));
}

void postoExt::stairlight_pressed()
{
	qDebug("postoExt::stairlight_pressed()");
	BTouch->sendFrame(QString("*6*12*%1##").arg(where));
}

void postoExt::stairlight_released()
{
	qDebug("postoExt::stairlight_released()");
	BTouch->sendFrame(QString("*6*11*%1##").arg(where));
}

void postoExt::get_where(QString& out)
{
	out = where;
}

QString postoExt::get_descr()
{
	return descr;
}

bool postoExt::get_light()
{
	return light;
}

bool postoExt::get_key()
{
	return key;
}

void postoExt::get_light_icon(QString& out)
{
	out = light_icon;
}

void postoExt::get_key_icon(QString& out)
{
	out = key_icon;
}

void postoExt::get_close_icon(QString& out)
{
	out = close_icon;
}


call_notifier::call_notifier(QWidget *parent, postoExt *ms) : QFrame(parent)
{
	qDebug("call_notifier::call_notifier()");
	QString where;
	my_station = ms;
	if (ms)
		ms->get_where(where);
	else
		// Unknown station, ANY where is ok
		where = "ANY";

    station_dev = btouch_device_cache.get_doorphone_device(where);
	if (!station_dev)
	{
		qDebug("Bad thing, cannot create device");
		return;
    }
	myTimer = new QTimer(this);
	myTimer->setSingleShot(true);
	connect(myTimer, SIGNAL(timeout()), this, SLOT(close()));
	// Pass incoming frames on to device
	connect(this, SIGNAL(frame_available(char *)),
		station_dev, SLOT(frame_rx_handler(char *)));
	// Get status changed events
	connect(station_dev, SIGNAL(status_changed(QList<device_status*>)),
		this, SLOT(status_changed(QList<device_status*>)));
	station_dev->init();

	QString _txt1(tr("Unknown"));
	QString _txt2(tr("Staircase light"));
	QString _txt3(tr("Door lock"));
	SetIcons(_txt1, _txt2, _txt3);
}

void call_notifier::get_where(QString& out)
{
	qDebug("call_notifier::get_where()");
	my_station->get_where(out);
}

void call_notifier::status_changed(QList<device_status*> dsl)
{
	// When we get here, we captured a call frame for sure, no need to
	// read status
	qDebug("call_notifier::status_changed()");
	emit frame_captured(this);
}

void call_notifier::showFullScreen()
{
	qDebug("call_notifier::showFullScreen()");
	QFrame::showFullScreen();
	myTimer->start(30000);
}

// FIXME: direct connection ?
void call_notifier::frame_available_handler(char *f)
{
	qDebug("call_notifier::frame_available_handler()");
	emit frame_available(f);
}

void call_notifier::stairlight_pressed()
{
	qDebug("call_notifier::stairlight_pressed()");
	if (my_station)
		my_station->stairlight_pressed();
	myTimer->start(30000);
}

void call_notifier::stairlight_released()
{
	qDebug("call_notifier::stairlight_released()");
	if (my_station)
		my_station->stairlight_released();
	myTimer->start(30000);
}

void call_notifier::open_door_clicked()
{
	qDebug("call_notifier::open_door_clicked()");
	if (my_station)
		my_station->open_door_clicked();
	myTimer->start(30000);
}

void call_notifier::close()
{
	qDebug("call_notifier::close()");
	myTimer->stop();
	hide();
	emit closed(this);
}

// Private methods
void call_notifier::SetIcons(QString _txt1, QString _txt2, QString _txt3)
{
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
	area1_ptr = new QLabel(this);
	area1_ptr->setGeometry(0, (MAX_HEIGHT/4)-2*LABEL_HEIGHT,
				MAX_WIDTH, LABEL_HEIGHT);
	QFont aFont;
	FontManager::instance()->getFont(font_videocitof_area1, aFont);
	area1_ptr->setFont(aFont);
	area1_ptr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	QString s;
	if (my_station)
		s = my_station->get_descr();
	else
		s = _txt1;
	area1_ptr->setText(s);
	if (my_station && my_station->get_light())
	{
		area2_but = new BtButton(this);
		area2_but->setGeometry(0, (MAX_HEIGHT/2)-BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
		connect(area2_but, SIGNAL(pressed()), this,
			SLOT(stairlight_pressed()));
		connect(area2_but, SIGNAL(released()), this,
			SLOT(stairlight_released()));
		area3_ptr = new QLabel(this);
		area3_ptr->setGeometry(BUTTON_DIM,
				(MAX_HEIGHT/2)-(BUTTON_DIM/2)-(LABEL_HEIGHT/2),
				LABEL_WIDTH, LABEL_HEIGHT);
		FontManager::instance()->getFont(font_videocitof_area3, aFont);
		area3_ptr->setFont(aFont);
		area3_ptr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		s = _txt2;
		area3_ptr->setText(s);
	}
	else
	{
		area2_but = NULL;
		area3_ptr = NULL;
	}

	if (my_station && my_station->get_key())
	{
		area4_but = new BtButton(this);
		area4_but->setGeometry(0, ((3*MAX_HEIGHT)/4)-BUTTON_DIM,
				BUTTON_DIM, BUTTON_DIM);
		connect(area4_but, SIGNAL(clicked()), this, SLOT(open_door_clicked()));
		area5_ptr = new QLabel(this);
		area5_ptr->setGeometry(BUTTON_DIM,
				((3*MAX_HEIGHT)/4)-(BUTTON_DIM/2)-(LABEL_HEIGHT/2),
				LABEL_WIDTH, LABEL_HEIGHT);
		FontManager::instance()->getFont(font_videocitof_area5, aFont);
		area5_ptr->setFont(aFont);
		area5_ptr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		s = _txt3;
		area5_ptr->setText(s);
	}
	else
	{
		area4_but = NULL;
		area5_ptr = NULL;
	}
	area6_but = new BtButton(this);
	area6_but->setGeometry(0, MAX_HEIGHT-BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
	connect(area6_but, SIGNAL(clicked()), this, SLOT(close()));
	SetButtonsIcons();
}

void call_notifier::SetButtonIcon(QString icon_name, BtButton *b)
{
	if (!b)
		return;

	b->setImage(icon_name);
}

void call_notifier::SetButtonsIcons()
{
	QString a2_icon, a4_icon, a6_icon;
	if (!my_station)
	{
		// FIXME !!, BAH
		a2_icon = IMG_PATH "btnlucescale.png";
		a4_icon = IMG_PATH "btnserratura.png";
		a6_icon = IMG_PATH "arrlf.png";
	}
	else
	{
		my_station->get_light_icon(a2_icon);
		my_station->get_key_icon(a4_icon);
		my_station->get_close_icon(a6_icon);
	}
	SetButtonIcon(a2_icon, area2_but);
	SetButtonIcon(a4_icon, area4_but);
	SetButtonIcon(a6_icon, area6_but);
}


// Call notifier manager implementation
call_notifier_manager::call_notifier_manager()
{
	qDebug("call_notifier_manager::call_notifier_manager()");
	unknown_station_notifier = NULL;
	known_station = false;
}

void call_notifier_manager::add_call_notifier(call_notifier *cn)
{
	qDebug("call_notifier_manager::add_call_notifier()");
	connect(this, SIGNAL(frame_available(char *)),
		cn, SLOT(frame_available_handler(char *)));
	connect(cn, SIGNAL(frame_captured(call_notifier *)),
		this, SLOT(frame_captured_handler(call_notifier *)));
	connect(cn, SIGNAL(closed(call_notifier *)),
		this, SIGNAL(call_notifier_closed(call_notifier *)));
}

void call_notifier_manager::set_unknown_call_notifier(call_notifier *cn)
{
	qDebug("call_notifier_manager::set_unknown_call_notifier");
	unknown_station_notifier = cn;
	connect(cn, SIGNAL(frame_captured(call_notifier *)),
		this, SLOT(frame_captured_handler(call_notifier *)));
	connect(cn, SIGNAL(closed(call_notifier *)),
		this, SIGNAL(call_notifier_closed(call_notifier *)));
}

void call_notifier_manager::gestFrame(char *f)
{
	qDebug("call_notifier_manager::gestFrame()");
	known_station = false;
	emit frame_available(f);
	// Has one of the known stations' call_notifier objects captured
	// this frame ?
	if ((!known_station) && unknown_station_notifier)
	{
		qDebug("forwarding frame to unknown status call notifier");
		unknown_station_notifier->frame_available_handler(f);
	}
}

void call_notifier_manager::frame_captured_handler(call_notifier *cn)
{
	qDebug("frame_captured_handler()");
	known_station = true;
	// A frame has been captured by a call notifier (cn)
	cn->showFullScreen();
	emit frame_captured(cn);
}
