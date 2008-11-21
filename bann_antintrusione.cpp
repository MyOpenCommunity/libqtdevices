/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** bann_antintrusione.cpp
 **
 **
 **definizioni dei vari banner antintrusione implementati
 ****************************************************************/

#include "bann_antintrusione.h"
#include "main.h" // MAX_PATH, IMG_PATH
#include "generic_functions.h" // void getZoneName(...)
#include "fontmanager.h"
#include "btbutton.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "tastiera.h"

#include <QDebug>
#include <QTimer>
#include <QLabel>

#include <stdlib.h>


/*****************************************************************
 **zonaAnti
 ****************************************************************/

zonaAnti::zonaAnti(QWidget *parent, const QString & name, char *indirizzo, QString iconzona, QString IconDisactive,
	QString IconActive) : bannOnIcons(parent)
{
	setAddress(indirizzo);
	qDebug("zonaAnti::zonaAnti()");
	// Mail agresta 22/06
	parzIName = IMG_PATH "btnparzializza.png";
	sparzIName = IMG_PATH "btnsparzializza.png";

	SetIcons(1, sparzIName);
	SetIcons(2, getZoneName(iconzona, indirizzo));
	SetIcons(3, IconDisactive);

	if (BannerText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_bannertext, aFont);
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(aFont);
		BannerText->setText(name);
	}

	zonaAttiva = IconActive;
	zonaNonAttiva = IconDisactive;
	already_changed = false;
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_zonanti_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));

	abilitaParz(true);
	SetIcons();
	zonaAnti::Draw();
}

void zonaAnti::SetIcons()
{
	if (isActive())
	{
		SetIcons(1, sparzIName);
		SetIcons(3, zonaAttiva);
	}
	else
	{
		SetIcons(1, parzIName);
		SetIcons(3, zonaNonAttiva);
	}
}

void zonaAnti::Draw()
{
	qDebug("sxButton = %p", sxButton);
	sxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
		sxButton->setPressedPixmap(*pressIcon[1]);
	BannerIcon->setPixmap(*Icon[2]);
	BannerIcon2->setPixmap(*Icon[3]);
}

int zonaAnti::getIndex(void)
{
	QString addr = getAddress();
	addr.remove(0, 1);
	return addr.toInt();
}

void zonaAnti::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("zonAnti::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		int s;
		switch (ds->get_type())
		{
		case device_status::ZONANTI:
			qDebug("Zon.anti status variation");
			ds->read(device_status_zonanti::ON_OFF_INDEX, curr_status);
			s = curr_status.get_val();
			qDebug("stat is %d", s);
			if (!isActive() && s)
			{
				impostaAttivo(1);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
			else if (isActive() && !s)
			{
				impostaAttivo(0);
				qDebug("new status = %d", s);
				aggiorna = true;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
	{
		if (!already_changed)
		{
			already_changed = true;
			emit partChanged(this);
		}
		SetIcons();
		Draw();
	}
}

void zonaAnti::ToggleParzializza()
{
	qDebug("zonaAnti::ToggleParzializza()");
	impostaAttivo(!isActive());
	if (!already_changed)
	{
		already_changed = true;
		emit(partChanged(this));
	}
	SetIcons();
	Draw();
}

void zonaAnti::abilitaParz(bool ab)
{
	qDebug("zonaAnti::abilitaParz(%d)", ab);
	if (ab)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		mostra(BUT1);
	}
	else
	{
		disconnect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		nascondi(BUT1);
	}
	Draw();
}

void zonaAnti::clearChanged()
{
	already_changed = false;
}

void zonaAnti::inizializza(bool forza)
{
	dev->sendInit("*#5*" + getAddress() + "##");
}


/*****************************************************************
 **impAnti
 ****************************************************************/


impAnti::impAnti(QWidget *parent,char* indirizzo, QString IconOn, QString IconOff, QString IconInfo, QString IconActive)
	: bann2butLab(parent)
{
	tasti = NULL;
	QString disactive_icon_path;
	if (!IconActive.isNull())
		disactive_icon_path = IconActive.left(IconActive.indexOf('.') - 3);

	disactive_icon_path += "dis" + IconActive.mid(IconActive.indexOf('.'));

	SetIcons(IconInfo, IconOff, disactive_icon_path, IconOn, IconActive);
	send_part_msg = false;
	inserting = false;
	memset(le_zone, 0, sizeof(le_zone));
	nascondi(BUT2);
	impostaAttivo(0);
	Draw();
	// BUT2 and 4 are actually both on the left of the banner.
	connect(this,SIGNAL(dxClick()),this,SLOT(Disinserisci()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Inserisci()));
	connect(this,SIGNAL(sxClick()), parentWidget(),SIGNAL(goDx()));

	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_impanti_device();
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void impAnti::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("impAnti::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		int s;
		switch (ds->get_type())
		{
		case device_status::IMPANTI:
			qDebug("Imp.anti status variation");
			ds->read(device_status_impanti::ON_OFF_INDEX, curr_status);
			s = curr_status.get_val();
			qDebug("status = %d", s);
			if (!isActive() && s)
			{
				impostaAttivo(2);
				nascondi(BUT4);
				nascondi(BUT1);
				mostra(BUT2);
				aggiorna=1;
				qDebug("IMPIANTO INSERITO !!");
				emit impiantoInserito();
				emit abilitaParz(false);
				QTimer::singleShot(5000, this, SLOT(inizializza()));
				send_part_msg = false;
			}
			else if (isActive() && !s)
			{
				impostaAttivo(0);
				nascondi(BUT2);
				mostra(BUT4);
				aggiorna=1;
				qDebug("IMPIANTO DISINSERITO !!");
				emit abilitaParz(true);
				emit clearChanged();
				send_part_msg = false;
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

int impAnti::getIsActive(int zona)
{
	if (le_zone[zona] != NULL)
		return le_zone[zona]->isActive();
	else
		return -1;
}

void impAnti::ToSendParz(bool s)
{
	send_part_msg = s;
	if (!send_part_msg)
		emit clearChanged();
}

void impAnti::Inserisci()
{
	qDebug("impAnti::Inserisci()");
	int s[MAX_ZONE];
	for (int i=0; i<MAX_ZONE; i++)
	{
		if (le_zone[i] !=  NULL)
			s[i] = le_zone[i]->isActive();
		else
			s[i] = -1;
	}

	if (tasti)
		delete tasti;
	inserting = true;
	tasti = new tastiera_con_stati(s, NULL);
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Insert1(char*)));
	tasti->setMode(tastiera::HIDDEN);
	tasti->showFullScreen();
}

void impAnti::Disinserisci()
{
	if (tasti)
		delete tasti;
	tasti = new tastiera(NULL);
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(DeInsert(char*)));
	tasti->setMode(tastiera::HIDDEN);
	tasti->showFullScreen();
}

void impAnti::Insert1(char *pwd)
{
	if (!pwd)
	{
		parentWidget()->show();
		return;
	}
	passwd = pwd;

	qDebug("impAnti::Insert()");
	if (!send_part_msg)
	{
		Insert3();
		parentWidget()->show();
		return;
	}
	QString f = QString("*5*50#") + pwd + "#";
	for (int i = 0; i < MAX_ZONE; i++)
		f += le_zone[i] && le_zone[i]->isActive() ? "0" : "1";
	f += "*0##";
	qDebug() << "sending part frame" << f;
	dev->sendFrame(f);

	send_part_msg = false;
	part_msg_sent = true;
	parentWidget()->show();
}

void impAnti::Insert2()
{
	qDebug("impAnti::Insert2()");
	if (!inserting)
		return;
	// 6 seconds between first open ack and open insert messages
	QTimer::singleShot(6000, this, SLOT(Insert3()));
}

void impAnti::Insert3()
{
	qDebug("impAnti::Insert3()");
	emit clearAlarms();
	dev->sendFrame(QString("*5*36#") + passwd + "*0##");
	parentWidget()->show();
	inserting = false;
	QTimer::singleShot(5000, this, SLOT(inizializza()));
}

void impAnti::DeInsert(char *pwd)
{
	qDebug("impAnti::DeInsert()");
	if (pwd)
		dev->sendFrame(QString("*5*36#") + pwd + "*0##");
	parentWidget()->show();
}

void impAnti::openAckRx()
{
	qDebug("impAnti::openAckRx()");
	if (!inserting)
	{
		qDebug("Not inserting");
		return;
	}
	if (!part_msg_sent) return;
	part_msg_sent = false;
	qDebug("waiting 5 seconds before sending insert message");
	// Do second step of insert
	Insert2();
}

void impAnti::openNakRx()
{
	qDebug("impAnti::openNakRx()");
	if (!part_msg_sent)
		return;
	//Agre - per sicurezza provo a continuare per evitare di non inserire l'impianto
	openAckRx();
	part_msg_sent = false;
}

void impAnti::setZona(zonaAnti *za)
{
	int index = za->getIndex() - 1;
	if ((index >= 0) && (index < MAX_ZONE))
		le_zone[index] = za;
}

void impAnti::partChanged(zonaAnti *za)
{
	qDebug("impAnti::partChanged");
	send_part_msg = true;
}

void impAnti::inizializza(bool forza)
{
	qDebug("impAnti::inizializza()");
	dev->sendInit("*#5*0##");
}

void impAnti::hideEvent(QHideEvent *event)
{
	qDebug("impAnti::hideEvent()");
	if (tasti && !tasti->isHidden())
	{
		qDebug("HIDING KEYBOARD");
		tasti->hide();
	}
}
