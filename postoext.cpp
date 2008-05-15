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
#include "openclient.h" // class openwebnet, Client
#include "device_cache.h" // btouch_device_cache
#include "videocitof.h" // class call_notifier

/*****************************************************************
 ** Posto Esterno
 ****************************************************************/	

// Static pointer to call notifier manager
call_notifier_manager *postoExt::cnm = NULL;

// Static pointer to unknown station
call_notifier *postoExt::unknown_notifier = NULL;

postoExt::postoExt(QWidget *parent, const char *name, char* Icona1,char *Icona2, char *Icona3, char* Icona4, char *_where, char *_light, char *_key, char *_unknown, char* _txt1, char* _txt2, char* _txt3) : bann4tasLab( parent, name )
{
	where = _where;
	descr = name;
	light = !strcmp(_light, "1");
	key = !strcmp(_key, "1");
	unknown = !strcmp(_unknown, "1");
	qDebug("postoExt::postoExt(), unknown = %s", _unknown);
	qDebug("I1 = %s, I2 = %s, I3 = %s, I4 = %s", 
			Icona1, Icona2, Icona3, Icona4);

	qDebug("light = %d, key = %d, unknown = %d", light, key, unknown);
	qDebug("descr = %s, where = %s", name, _where);
	qDebug("txt1 = %s, txt2 = %s, txt3 = %s", _txt1, _txt2, _txt3);
	//SetIcons(Icona1, Icona3, Icona4, Icona2);
	SetIcons (Icona2, Icona3, "", "", Icona1);
	if(key) {
		key_icon = Icona2;
		connect(this, SIGNAL(sxPressed()), this, SLOT(open_door_clicked()));
	} else
		nascondi(BUT1);
	if(light) {
		connect(this, SIGNAL(dxPressed()), this, SLOT(stairlight_pressed()));
		connect(this, SIGNAL(dxReleased()), this, SLOT(stairlight_released()));
		light_icon = Icona3;
	} else
		nascondi(BUT2);
	connect(parent, SIGNAL(frez(bool)), this, SIGNAL(freezed(bool)));
	close_icon = Icona4;
	impostaAttivo(2);
	Draw();
	qDebug("creating call_notifier");
	call_notifier *cn = new call_notifier((QWidget *)NULL, 
			(char *)"call notifier", this, _txt1, _txt2, _txt3);
	qDebug("setting BG and FG colors");
	cn->setBGColor(backgroundColor());
	cn->setFGColor(foregroundColor());
	qDebug("cnm = %p", cnm);
	if(!cnm) {
		qDebug("Creating call notifier manager");
		cnm = new call_notifier_manager();
		connect(cnm, SIGNAL(frame_captured(call_notifier *)),
				this, SLOT(frame_captured_handler(call_notifier *)));
		connect(cnm, SIGNAL(call_notifier_closed(call_notifier *)),
				this, SLOT(call_notifier_closed(call_notifier *)));
		connect(btouch_device_cache.get_client_monitor(), 
				SIGNAL(frameIn(char *)),
				cnm, SLOT(gestFrame(char *)));
		connect(this, SIGNAL(freezed(bool)), cnm, SIGNAL(freezed(bool)));
	}
	cnm->add_call_notifier(cn);
	if(unknown && !unknown_notifier) {
		qDebug("Creating unknown station notifier");
		unknown_notifier = new call_notifier(NULL, "unk call notif", NULL, _txt1, _txt2, _txt3);
		cnm->set_unknown_call_notifier(unknown_notifier);
		unknown_notifier->setBGColor(backgroundColor());
		unknown_notifier->setFGColor(foregroundColor());
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
	//emit(svegl(1));
	emit(freeze(0));
}

void postoExt::call_notifier_closed(call_notifier *cn)
{
	qDebug("postoExt::call_notifier_closed()");
	//emit(svegl(0));
}

void postoExt::open_door_clicked(void)
{
	qDebug("postoExt::open_door_clicked()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char tmp[100];
	sprintf(tmp, "*6*10*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);
}

void postoExt::stairlight_pressed(void)
{
	qDebug("postoExt::stairlight_pressed()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char tmp[100];
	sprintf(tmp, "*6*12*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);
}

void postoExt::stairlight_released(void)
{
	qDebug("postoExt::stairlight_released()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char tmp[100];
	sprintf(tmp, "*6*11*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);
}

void postoExt::get_where(QString& out)
{
	out = where;
}

void postoExt::get_descr(QString& out)
{
	out = descr;
}

bool postoExt::get_light(void)
{
	return light;
}

bool postoExt::get_key(void)
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
