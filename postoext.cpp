/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** postoext.cpp
 **
 **
 **
 ****************************************************************/

#include "postoext.h"
#include "openclient.h" // Client
#include "device_cache.h" // btouch_device_cache
#include "videocitof.h" // class call_notifier
#include "btmain.h"
#include "main.h" // BTouch

#include <QDebug>


/*****************************************************************
 ** Posto Esterno
 ****************************************************************/	

// Static pointer to call notifier manager
call_notifier_manager *postoExt::cnm = NULL;

// Static pointer to unknown station
call_notifier *postoExt::unknown_notifier = NULL;

// TODO: capire a che serve name!
postoExt::postoExt(QWidget *parent, const char *name, QString Icona1, QString Icona2, QString Icona3, QString Icona4, char *_where,
	QString _light, QString _key, QString _unknown) : bann4tasLab(parent)
{
	where = _where;
	descr = name;
	light = (_light == "1");
	key = (_key == "1");
	unknown = (_unknown == "1");
	qDebug() << "postoExt::postoExt(), unknown = "  << _unknown;

	qDebug("light = %d, key = %d, unknown = %d", light, key, unknown);
	qDebug("descr = %s, where = %s", name, _where);
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
		connect(btouch_device_cache.get_client_monitor(),
				SIGNAL(frameIn(char *)), cnm, SLOT(gestFrame(char *)));
	}
	cnm->add_call_notifier(cn);
	if (unknown && !unknown_notifier)
	{
		qDebug("Creating unknown station notifier");
		unknown_notifier = new call_notifier(NULL, NULL);
		cnm->set_unknown_call_notifier(unknown_notifier);
	}
}

void postoExt::gestFrame(char *s)
{
	qDebug("postoExt::gestFrame()");
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

void postoExt::get_descr(QString& out)
{
	out = descr;
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
