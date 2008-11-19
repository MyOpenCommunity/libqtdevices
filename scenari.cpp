/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** scenari.cpp
 **
 **
 **definizioni dei vari scenari implementati
 ****************************************************************/

#include "scenari.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "scenevocond.h"
#include "btbutton.h"
#include "generic_functions.h" // setCfgValue
#include "fontmanager.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <openwebnet.h> // class openwebnet

#include <QDir>
#include <QDebug>
#include <QLabel>


/*****************************************************************
 **scenario
 ****************************************************************/

scenario::scenario(sottoMenu *parent, char *indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void scenario::Attiva()
{
	openwebnet msg_open;
	char cosa[15];

	strncpy(cosa,getAddress(),sizeof(cosa));
	if (strstr(cosa,"*"))
	{
		memset(index(cosa,'*'),'\000',sizeof(cosa)-(index(cosa,'*')-cosa));
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("0", cosa,strstr(getAddress(),"*")+1,"");
		BTouch->sendFrame(msg_open.frame_open);
	}
}
void scenario::inizializza(bool forza){}


/*****************************************************************
 **gesModScen
 ****************************************************************/

gesModScen::gesModScen(QWidget *parent, char *indirizzo, QString IcoSx, QString IcoDx, QString IcoCsx,
	QString IcoCdx, QString IcoDes, QString IcoSx2, QString IcoDx2) :  bann4tasLab(parent)
{
	icon_on = IcoSx;
	icon_stop = IcoSx2;
	icon_info = IcoDx;
	icon_no_info = IcoDx2;

	SetIcons(IcoSx, IcoDx, IcoCsx, IcoCdx, IcoDes);
	nascondi(BUT3);
	nascondi(BUT4);

	memset(cosa,'\000',sizeof(cosa));
	memset(dove,'\000',sizeof(dove));
	qDebug("gesModScen::gesModScen(): indirizzo = %s", indirizzo);
	if (strstr(indirizzo,"*"))
	{
		strncpy(cosa, indirizzo, strstr(indirizzo,"*")-indirizzo);
		strcpy(dove, strstr(indirizzo,"*")+1);
	}
	sendInProgr = 0;
	in_progr = 0;

	setAddress(dove);
	impostaAttivo(2);
	connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(csxClick()),this,SLOT(startProgScen()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(cancScen()));

	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_modscen_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void gesModScen::attivaScenario()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo, '\000',sizeof(pippo));
	strcat(pippo, "*0*");
	strcat(pippo, cosa);
	strcat(pippo, "*");
	strcat(pippo, dove);
	strcat(pippo, "##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendFrame(msg_open.frame_open);
}

void gesModScen::enterInfo()
{
	nascondi(ICON);
	mostra(BUT4);
	mostra(BUT3);
	SetIcons(1, icon_no_info);
	disconnect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));
	Draw();
}

void gesModScen::exitInfo()
{
	mostra(ICON);
	nascondi(BUT4);
	nascondi(BUT3);
	SetIcons(1, icon_info);
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	disconnect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));
	Draw();
}

void gesModScen::startProgScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*0*40#");
	strcat(pippo,cosa);
	strcat(pippo,"*");
	strcat(pippo,dove);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendFrame(msg_open.frame_open);
	sendInProgr = 1;
}

void gesModScen::stopProgScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo, '\000',sizeof(pippo));
	strcat(pippo, "*0*41#");
	strcat(pippo, cosa);
	strcat(pippo, "*");
	strcat(pippo, dove);
	strcat(pippo, "##");
	msg_open.CreateMsgOpen((char*)pippo,strlen((char*)pippo));
	dev->sendFrame(msg_open.frame_open);
	sendInProgr = 0;
}

void gesModScen::cancScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo, '\000',sizeof(pippo));
	strcat(pippo, "*0*42#");
	strcat(pippo, cosa);
	strcat(pippo, "*");
	strcat(pippo, dove);
	strcat(pippo, "##");
	msg_open.CreateMsgOpen((char*)pippo, strlen((char*)pippo));
	dev->sendFrame(msg_open.frame_open);
}

void gesModScen::status_changed(QList<device_status*> sl)
{
	stat_var curr_status(stat_var::STAT);
	qDebug("gesModScen::status_changed");
	bool aggiorna = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::MODSCEN:
			qDebug("Modscen status change");
			ds->read((int)device_status_modscen::STAT_INDEX, curr_status);
			switch(curr_status.get_val())
			{
			case device_status_modscen::PROGRAMMING_START:
				qDebug("Programming start");
				if (sendInProgr)
				{
					SetIcons(0, icon_stop);
					disconnect(this,SIGNAL(sxClick()), this,SLOT(attivaScenario()));
					connect(this,SIGNAL(sxClick()), this,SLOT(stopProgScen()));
					in_progr = 0;
				}
				else
				{
					in_progr = 1;
					exitInfo();
				}
				aggiorna = 1;
				break;
			case device_status_modscen::LOCKED:
				qDebug("Locked");
				bloccato = 1;
				exitInfo();
				aggiorna = 1;
				break;
			case device_status_modscen::BUSY:
				qDebug("Busy");
				in_progr = 1;
				exitInfo();
				aggiorna = 1;
				break;
			case device_status_modscen::PROGRAMMING_STOP:
				qDebug("Programming stop");
				SetIcons(0, icon_on);
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				aggiorna = 1;
				in_progr = 0;
				break;
			case device_status_modscen::UNLOCKED:
				qDebug("Unlocked");
				SetIcons(0, icon_on);
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				aggiorna = 1;
				bloccato = 0;
				break;
			default:
				qDebug("Unknown status %d", curr_status.get_val());
			}
			break;
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
	{
		if (bloccato || in_progr)
			nascondi(BUT2);
		else
			mostra(BUT2);
		Draw();
	}
}

void gesModScen::inizializza(bool forza)
{
	nascondi(BUT2);
}


/*****************************************************************
 ** Scenario evoluto
 ****************************************************************/	

int scenEvo::next_serial_number = 1;

scenEvo::scenEvo(QWidget *parent, QList<scenEvo_cond*> *c, QString i1, QString i2,
	QString i3, QString i4, QString act, int enable) : bann3But(parent)
{
	if (c)
	{
		condList = *c;
		current_condition = 0;
	}

	serial_number = next_serial_number++;
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		qDebug() << "connecting condition: " << co->getDescription();
		co->set_serial_number(serial_number);
		connect(co, SIGNAL(verificata()), this, SLOT(trig()));
		connect(co, SIGNAL(condSatisfied()), this, SLOT(trigOnStatusChanged()));
		connect(co, SIGNAL(okAll()), this, SLOT(saveAndApplyAll()));
		connect(co, SIGNAL(resetAll()), this, SLOT(resetAll()));
	}

	action = act;
	SetIcons(i1, i2, i3, i4);
	impostaAttivo(enable);
	connect(this,SIGNAL(sxClick()), this, SLOT(toggleAttivaScev()));
	connect(this,SIGNAL(dxClick()), this, SLOT(configScev()));
	connect(this,SIGNAL(centerClick()), this, SLOT(forzaScev()));
}

void scenEvo::toggleAttivaScev()
{
	qDebug("scenEvo::toggleAttivaScev");
	impostaAttivo(!isActive());
	Draw();
	const char *s = isActive() ? "1" : "0";
	setCfgValue("enable", s, SCENARIO_EVOLUTO, serial_number);
}

void scenEvo::configScev()
{
	qDebug("scenEvo::configScev");
	hide();
	scenEvo_cond *co = condList.at(current_condition);
	qDebug("Invoco %p->mostra()", co);
	connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->mostra();
}

void scenEvo::forzaScev()
{
	qDebug("scenEvo::forzaScev");
	// Forced trigger
	trig(true);
	Draw();
}

void scenEvo::nextCond()
{
	qDebug("scenEvo::nextCond()");
	scenEvo_cond *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();

	if (current_condition + 1 < static_cast<unsigned>(condList.size()))
	{
		++current_condition;
		co = condList.at(current_condition);
		qDebug("co = %p", co);
		if (co)
		{
			connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			co->inizializza();
			co->mostra();
		}
	}
	else
	{
		current_condition = 0;
		Draw();
		show();
	}
}

void scenEvo::prevCond()
{
	qDebug("scenEvo::prevCond()");
	scenEvo_cond *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();

	if (current_condition > 0)
	{
		--current_condition;
		co = condList.at(current_condition);
		qDebug("co = %p", co);
		if (co)
		{
			connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			co->mostra();
		}
	}
	else
	{
		Draw();
		show();
	}
}

void scenEvo::firstCond()
{
	qDebug("scenEvo::firstCond()");
	scenEvo_cond *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();
	current_condition = 0;
	Draw();
	show();
}

void scenEvo::saveAndApplyAll()
{
	qDebug("scenEvo::saveAndApplyAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->Apply();
		co->save();
	}
}

void scenEvo::resetAll()
{
	qDebug("scenEvo::resetAll()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->reset();
	}
}

void scenEvo::Draw()
{
	// Icon[0] => left button (inactive)
	// pressIcon[0] => pressed left button (inactive)
	// Icon[1] => left button (active)
	// pressIcon[1] => pressed left button (active)
	// Icon[3] => center button
	// pressIcon[3] => pressed center button
	// Icon[2] => right button
	// pressIcon[2] => pressed right button
	qDebug("scenEvo::Draw(%p)", this);
	current_condition = 0;
	if (sxButton && Icon[0] && Icon[1])
	{
		int sxb_index = isActive() ? 0 : 1;
		sxButton->setPixmap(*Icon[sxb_index]);
		if (pressIcon[sxb_index])
			sxButton->setPressedPixmap(*pressIcon[sxb_index]);
	}
	if (dxButton && Icon[2])
	{
		dxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (csxButton && Icon[3])
	{
		csxButton->setPixmap(*Icon[3]);
		if (pressIcon[3])
			csxButton->setPressedPixmap(*pressIcon[3]);
	}
	if (BannerText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_bannertext, aFont);
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(aFont);
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_secondarytext, aFont);
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(aFont);
		SecondaryText->setText(qtestoSecondario);
	}
}

void scenEvo::trigOnStatusChanged()
{
	qDebug("scenEvo::trigOnStatusChanged()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		if (co->hasTimeCondition)
			return;
	}
	trig();
}

void scenEvo::trig(bool forced)
{
	if (action.isEmpty())
	{
		qDebug("scenEvo::trig(), act = NULL, non faccio niente");
		return;
	}

	if (!forced)
	{
		if (!isActive())
		{
			qDebug("scenEvo::trig(), non abilitato, non faccio niente");
			return;
		}
		for (int i = 0; i < condList.size(); ++i)
		{
			scenEvo_cond *co = condList.at(i);
			if (!co->isTrue())
			{
				qDebug("Condizione %p (%s), non verificata, non faccio niente",
						co, co->getDescription());
				return;
			}
		}
	}
	QByteArray buf = action.toAscii();
	qDebug("scenEvo::trig(), act = %s", buf.constData());
	openwebnet msg_open;
	msg_open.CreateMsgOpen(buf.data(), buf.length());
	BTouch->sendFrame(msg_open.frame_open);
}

void scenEvo::inizializza(bool forza)
{
	qDebug("scenEvo::inizializza()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->inizializza();
	}
}

void scenEvo::hideEvent(QHideEvent *event)
{
	qDebug("scenEvo::hideEvent()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		co->hide();
	}
}

scenEvo::~scenEvo()
{
	while (!condList.isEmpty())
		delete condList.takeFirst();
}

/*****************************************************************
 ** Scenario schedulato
 ****************************************************************/	
scenSched::scenSched(QWidget *parent, QString Icona1, QString Icona2, QString Icona3, QString Icona4,
	char *aen, char *adis, char *astart, char *astop) : bann4But(parent)
{
	action_enable = aen;
	action_disable = adis;
	action_start = astart;
	action_stop = astop;
	qDebug("scenSched::scenSched(), enable = %s, start = %s, stop = %s, "
			"disable = %s", aen, adis, astart, astop);
	SetIcons(Icona1, Icona3, Icona4, Icona2);
	if (aen[0])
	{
		// sx
		qDebug("BUT1 attivo");
		connect(this, SIGNAL(sxClick()), this, SLOT(enable()));
	}
	else
		nascondi(BUT1);
	if (astart[0])
	{
		// csx
		qDebug("BUT3 attivo");
		connect(this, SIGNAL(csxClick()), this, SLOT(start()));
	}
	else
		nascondi(BUT3);
	if (astop[0])
	{
		// cdx
		qDebug("BUT4 attivo");
		connect(this, SIGNAL(cdxClick()), this, SLOT(stop()));
	}
	else
		nascondi(BUT4);
	if (adis[0])
	{
		// dx
		qDebug("BUT2 attivo");
		connect(this, SIGNAL(dxClick()), this, SLOT(disable()));
	}
	else
		nascondi(BUT2);
	Draw();
}

void scenSched::enable()
{
	qDebug("scenSched::enable()");
	openwebnet msg_open;
	QByteArray buf = action_enable.toAscii();
	msg_open.CreateMsgOpen(buf.data(),buf.length());
	BTouch->sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::start()
{
	qDebug("scenSched::start()");
	openwebnet msg_open;
	QByteArray buf = action_start.toAscii();
	msg_open.CreateMsgOpen(buf.data(),buf.length());
	BTouch->sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::stop()
{
	qDebug("scenSched::stop()");
	openwebnet msg_open;
	QByteArray buf = action_stop.toAscii();
	msg_open.CreateMsgOpen(buf.data(),buf.length());
	BTouch->sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::disable()
{
	qDebug("scenSched::disable()");
	openwebnet msg_open;
	QByteArray buf = action_disable.toAscii();
	msg_open.CreateMsgOpen(buf.data(),buf.length());
	BTouch->sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::Draw()
{
	// Icon[0] => left button
	// pressIcon[0] => pressed left button
	// Icon[1] => center left button 
	// pressIcon[1] => pressed center left button
	// Icon[3] => center right button
	// pressIcon[3] => pressed center right button
	// Icon[2] => right button
	// pressIcon[2] => pressed right button
	qDebug("scenSched::Draw()");
	if (sxButton && Icon[0])
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}
	if (csxButton && Icon[1])
	{
		csxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			csxButton->setPressedPixmap(*pressIcon[1]);
	}
	if (cdxButton && Icon[3])
	{
		cdxButton->setPixmap(*Icon[3]);
		if (pressIcon[3])
			cdxButton->setPressedPixmap(*pressIcon[3]);
	}
	if (dxButton && Icon[2])
	{
		dxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (BannerText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_bannertext, aFont);
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(aFont);
		BannerText->setText(qtesto);
	}
}
