#include "bann_scenario.h"
#include "device.h"
#include "scenevocond.h"
#include "btbutton.h"
#include "generic_functions.h" // setCfgValue
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::devices_cache
#include "automation_device.h"
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "scenario_device.h"
#include "xml_functions.h" // getTextChild

#include <QDir>
#include <QDebug>
#include <QLabel>
#include <QTimerEvent>
#include <QDomNode>

#define PPTSCE_INTERVAL 1000


BannSimpleScenario::BannSimpleScenario(QWidget *parent, const QDomNode &config_node) :
	bannOnSx(parent)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	SetIcons(bt_global::skin->getImage("on"), 1);

	QString where = getTextChild(config_node, "where");
	dev = bt_global::add_device_to_cache(new ScenarioDevice(where));

	scenario_number = getTextChild(config_node, "what").toInt();

	connect(this, SIGNAL(click()), SLOT(activate()));
}

void BannSimpleScenario::activate()
{
	dev->activateScenario(scenario_number);
}


#if 1

bannScenario::bannScenario(QWidget *parent, QString where, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(where);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannScenario::Attiva()
{
	char cosa[15];
	QByteArray buf_addr = getAddress().toAscii();
	strncpy(cosa, buf_addr.constData(),sizeof(cosa));
	if (strstr(cosa,"*"))
	{
		memset(index(cosa,'*'),'\000',sizeof(cosa)-(index(cosa,'*')-cosa));
		sendFrame(createMsgOpen("0", cosa, strstr(buf_addr.constData(),"*")+1));
	}
}


gesModScen::gesModScen(QWidget *parent, QString where, QString IcoSx, QString IcoDx, QString IcoCsx,
	QString IcoCdx, QString IcoDes, QString IcoSx2, QString IcoDx2) :  bann4tasLab(parent)
{
	icon_on = IcoSx;
	icon_stop = IcoSx2;
	icon_info = IcoDx;
	icon_no_info = IcoDx2;

	SetIcons(IcoSx, IcoDx, IcoCsx, IcoCdx, IcoDes);
	nascondi(BUT3);
	nascondi(BUT4);

	qDebug() << "gesModScen::gesModScen(): indirizzo =" << where;

	int pos = where.indexOf('*');
	if (pos != -1)
	{
		cosa = where.left(pos);
		dove = where.mid(pos + 1);
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
	dev = bt_global::devices_cache.get_modscen_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void gesModScen::attivaScenario()
{
	dev->sendFrame("*0*" + cosa + "*" + dove + "##");
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
	dev->sendFrame("*0*40#" + cosa + "*" + dove + "##");
	sendInProgr = 1;
}

void gesModScen::stopProgScen()
{
	dev->sendFrame("*0*41#" + cosa + "*" + dove + "##");
	sendInProgr = 0;
}

void gesModScen::cancScen()
{
	dev->sendFrame("*0*42#" + cosa + "*" + dove + "##");
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

#endif


int scenEvo::next_serial_number = 1;

scenEvo::scenEvo(QWidget *parent, QList<scenEvo_cond*> c, QString i1, QString i2,
	QString i3, QString i4, QString act, int enable) : bann3But(parent), condList(c)
{
	current_condition = 0;

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
	scenEvo_cond *co = condList.at(current_condition);
	qDebug("Invoco %p->mostra()", co);
	connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->showPage();
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
	scenEvo_cond *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition + 1 < static_cast<unsigned>(condList.size()))
	{
		++current_condition;
		scenEvo_cond *cond = condList.at(current_condition);
		qDebug("cond = %p", cond);
		if (cond)
		{
			connect(cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			cond->inizializza();
			cond->showPage();
		}
	}
	else
	{
		current_condition = 0;
		Draw();
		show();
	}
	old_cond->hide();
}

void scenEvo::prevCond()
{
	qDebug("scenEvo::prevCond()");
	scenEvo_cond *old_cond = condList.at(current_condition);
	disconnect(old_cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(old_cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(old_cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));

	if (current_condition > 0)
	{
		--current_condition;
		scenEvo_cond *cond = condList.at(current_condition);
		qDebug("cond = %p", cond);
		if (cond)
		{
			connect(cond, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(cond, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(cond, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			cond->showPage();
		}
	}
	else
	{
		Draw();
		show();
	}
	old_cond->hide();
}

void scenEvo::firstCond()
{
	qDebug("scenEvo::firstCond()");
	scenEvo_cond *co = condList.at(current_condition);
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	current_condition = 0;
	Draw();
	emit pageClosed();
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
	emit pageClosed();
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
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::TEXT));
		BannerText->setText(qtesto);
	}
	if (SecondaryText)
	{
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(bt_global::font->get(FontManager::TEXT));
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

	qDebug() << "scenEvo::trig(), act = " << action;
	sendFrame(action);
}

void scenEvo::inizializza(bool forza)
{
	qDebug("scenEvo::inizializza()");
	for (int i = 0; i < condList.size(); ++i)
	{
		scenEvo_cond *co = condList.at(i);
		qDebug() << "Ciclo n. " << i << co->getDescription();
		co->inizializza();
	}
}

scenEvo::~scenEvo()
{
	while (!condList.isEmpty())
		delete condList.takeFirst();
}


scenSched::scenSched(QWidget *parent, QString Icona1, QString Icona2, QString Icona3, QString Icona4,
	QString act_enable, QString act_disable, QString act_start, QString act_stop) : bann4But(parent)
{
	qDebug("scenSched::scenSched()");
	action_enable = act_enable;
	action_disable = act_disable;
	action_start = act_start;
	action_stop = act_stop;

	SetIcons(Icona1, Icona3, Icona4, Icona2);
	if (!act_enable.isNull())
	{
		// sx
		qDebug("BUT1 attivo");
		connect(this, SIGNAL(sxClick()), this, SLOT(enable()));
	}
	else
		nascondi(BUT1);
	if (!act_start.isNull())
	{
		// csx
		qDebug("BUT3 attivo");
		connect(this, SIGNAL(csxClick()), this, SLOT(start()));
	}
	else
		nascondi(BUT3);
	if (!act_stop.isNull())
	{
		// cdx
		qDebug("BUT4 attivo");
		connect(this, SIGNAL(cdxClick()), this, SLOT(stop()));
	}
	else
		nascondi(BUT4);
	if (!act_disable.isNull())
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
	sendFrame(action_enable);
	Draw();
}

void scenSched::start()
{
	qDebug("scenSched::start()");
	sendFrame(action_start);
	Draw();
}

void scenSched::stop()
{
	qDebug("scenSched::stop()");
	sendFrame(action_stop);
	Draw();
}

void scenSched::disable()
{
	qDebug("scenSched::disable()");
	sendFrame(action_disable);
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
		BannerText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		BannerText->setFont(bt_global::font->get(FontManager::TEXT));
		BannerText->setText(qtesto);
	}
}


PPTSce::PPTSce(QWidget *parent, QString where, int cid) : bann4But(parent)
{
	dev = bt_global::add_device_to_cache(new PPTSceDevice(where));
	connect(this, SIGNAL(sxClick()), dev, SLOT(turnOff()));
	connect(this, SIGNAL(dxClick()), dev, SLOT(turnOn()));

	// For csx e cdx buttons we have to send a frame every X mseconds when the
	// button is down and send another frame when the button is raised.
	increase_timer = 0;
	decrease_timer = 0;
	connect(this, SIGNAL(csxPressed()), SLOT(startIncrease()));
	connect(this, SIGNAL(cdxPressed()), SLOT(startDecrease()));
	connect(this, SIGNAL(csxReleased()), SLOT(stop()));
	connect(this, SIGNAL(cdxReleased()), SLOT(stop()));

	SkinContext context(cid);
	QString img_on = bt_global::skin->getImage("pptsce_on");
	QString img_off = bt_global::skin->getImage("pptsce_off");
	QString img_inc = bt_global::skin->getImage("pptsce_increase");
	QString img_decr = bt_global::skin->getImage("pptsce_decrease");
	SetIcons(img_off, img_on, img_inc, img_decr);
	Draw();
}

void PPTSce::startIncrease()
{
	if (!increase_timer)
	{
		dev->increase();
		increase_timer = startTimer(PPTSCE_INTERVAL);
	}
}

void PPTSce::startDecrease()
{
	if (!decrease_timer)
	{
		dev->decrease();
		decrease_timer = startTimer(PPTSCE_INTERVAL);
	}
}

void PPTSce::timerEvent(QTimerEvent *e)
{
	if (e->timerId() == increase_timer)
		dev->increase();
	else
		dev->decrease();
}

void PPTSce::stop()
{
	if (increase_timer)
	{
		killTimer(increase_timer);
		increase_timer = 0;
		dev->stop();
	}
	if (decrease_timer)
	{
		killTimer(decrease_timer);
		decrease_timer = 0;
		dev->stop();
	}
}

