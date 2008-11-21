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
#include "generic_functions.h" // createMsgOpen
#include "device_cache.h" // btouch_device_cache
#include "device.h"

#include <openwebnet.h> // class openwebnet

/*****************************************************************
 ** automCancAttuatVC
 ****************************************************************/

automCancAttuatVC::automCancAttuatVC(QWidget *parent, char *indirizzo, QString IconaSx, QString IconaDx)
	: bannButIcon(parent)
{
	qDebug("automCancAttuatVC::automCancAttuatVC()");
	SetIcons(IconaSx, QString(), IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void automCancAttuatVC::Attiva()
{
	dev->sendFrame(createMsgOpen("6", "10", getAddress()));
}


/*****************************************************************
 ** automCancAttuatIll
 ****************************************************************/

automCancAttuatIll::automCancAttuatIll(QWidget *parent, char *indirizzo, QString IconaSx, QString IconaDx, QString t)
	: bannButIcon(parent)
{
	qDebug("automCancAttuatIll::automCancAttuatIll()");
	SetIcons(IconaSx, QString(), IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	time = t.isEmpty() ? t : QString("*0*0*18");
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
	msg_open.CreateWrDimensionMsgOpen("1", getAddress().toAscii().data(), "2", v, 1);
	dev->sendFrame(msg_open.frame_open);
}
