/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** automatismi.cpp
 **
 **definizioni dei vari automatismi implementati
 **
 ****************************************************************/

#include "automatismi.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"

#include <openwebnet.h> // class openwebnet

/*****************************************************************
 ** automCancAttuatVC
 ****************************************************************/

automCancAttuatVC::automCancAttuatVC(QWidget *parent, const char *name, char* indirizzo, char* IconaSx, char* IconaDx)
	: bannButIcon(parent, name)
{
	qDebug("automCancAttuatVC::automCancAttuatVC() : %s %s", IconaSx, IconaDx);
	SetIcons(IconaSx, NULL, IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
}

void automCancAttuatVC::Attiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();
	// FIXME: CHECK FRAME !!
	msg_open.CreateMsgOpen("6", "10",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}


/*****************************************************************
 ** automCancAttuatIll
 ****************************************************************/

automCancAttuatIll::automCancAttuatIll(QWidget *parent, const char *name, char* indirizzo, char* IconaSx, char* IconaDx, QString *t)
	: bannButIcon(parent, name)
{
	qDebug("automCancAttuatIll::automCancAttuatIll() :%s %s", IconaSx, IconaDx);
	SetIcons(IconaSx, NULL, IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	time = t ? *t : QString("*0*0*18");
	Draw();
}

void automCancAttuatIll::Attiva()
{
	openwebnet msg_open;
	char val[100];
	char *v[] = { val , };

	QByteArray buf = time.toAscii();
	strncpy(val, buf.constData(), sizeof(val));

	msg_open.CreateNullMsgOpen();
	msg_open.CreateWrDimensionMsgOpen("1", getAddress(), "2", v, 1);
	dev->sendFrame(msg_open.frame_open);
}
