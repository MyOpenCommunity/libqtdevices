/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.cpp
 **
 **definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#include "actuators.h"
#include "device_cache.h" // btouch_device_cache
#include "device.h"
#include "btbutton.h"
#include "generic_functions.h" // getPressName, createMsgOpen
#include "fontmanager.h"
#include "btmain.h"
#include "main.h" // BTouch

#include <QFile>
#include <QLabel>
#include <QTimer>
#include <QDebug>

#include <stdlib.h>


grAttuatAutom::grAttuatAutom(QWidget *parent, void *indirizzi, QString IconaDx, QString IconaSx, QString icon, int period, int number)
	: bannOnOff(parent)
{
	SetIcons(IconaDx, IconaSx, QString(), icon, period, number);
	// TODO: togliere questo cast da void*!!!! (bisogna intervenire su xmlconfhandler)
	elencoDisp = *((QList<QString*>*)indirizzi);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
}

void grAttuatAutom::Attiva()
{
	for (int i = 0; i < elencoDisp.size();++i)
		BTouch->sendFrame(createMsgOpen("1", "1", *elencoDisp.at(i)));
}

void grAttuatAutom::Disattiva()
{
	for (int i = 0; i < elencoDisp.size();++i)
		BTouch->sendFrame(createMsgOpen("1", "0", *elencoDisp.at(i)));
}


attuatAutomTemp::attuatAutomTemp(QWidget *parent, char *indirizzo, QString IconaSx, QString IconaDx, QString icon, QString pressedIcon, int period, int number, QList<QString*> *lt)
	: bannOnOff2scr(parent)
{
	SetIcons(IconaDx, IconaSx, icon, pressedIcon, period, number);
	setAddress(indirizzo);
	cntTempi = 0;
	static const char *t[] =  { "1'", "2'", "3'", "4'", "5'", "15'", "30''" };
	int nt = lt->size() ? lt->size() : sizeof(t) / sizeof(char *);
	for (int i = 0; i < nt; i++)
	{
		QString *s;
		if (i < lt->size())
			s = lt->at(i);
		else
			s = new QString(t[i]); // TODO: verificare a cosa serve e se potrebbe essere un memory leak!
		tempi.append(s);
	}

	assegna_tempo_display();
	setSecondaryText(tempo_display);
	dev = btouch_device_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	connect(this, SIGNAL(dxClick()), this, SLOT(Attiva()));
	connect(this, SIGNAL(sxClick()), this, SLOT(CiclaTempo()));
}

QString attuatAutomTemp::leggi_tempo()
{
	if (cntTempi >= ntempi())
		return "";

	return *tempi.at(cntTempi);
}

uchar attuatAutomTemp::ntempi()
{
	return tempi.size();
}

void attuatAutomTemp::status_changed(QList<device_status*> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	bool aggiorna = false;
	qDebug("attuatAutomTemp::status_changed()");
	stat_var curr_status(stat_var::ON_OFF);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			aggiorna = true;
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val())) // Update
				impostaAttivo(curr_status.get_val() != 0);
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
		{
			qDebug("new timed device status variation, ignored");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				if (isActive())
				{
					impostaAttivo(false);
					aggiorna = true;
				}
				break;
			}

			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if ((isActive() && tmpval) || (!isActive() && !tmpval))
			{
				qDebug("already active, ignoring");
				break;
			}
			impostaAttivo((tmpval != 0));
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		}
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	if (aggiorna)
		Draw();
}

void attuatAutomTemp::Attiva()
{
	dev->sendFrame(createMsgOpen("1", QString::number(11+cntTempi), getAddress()));
}

void attuatAutomTemp::CiclaTempo()
{
	cntTempi = (cntTempi+1) % ntempi();
	qDebug("ntempi = %d", ntempi());
	qDebug("cntTempi = %d", cntTempi);
	assegna_tempo_display();
	setSecondaryText(tempo_display);
	Draw();
}

void attuatAutomTemp::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "##");
}

void attuatAutomTemp::assegna_tempo_display()
{
	tempo_display = leggi_tempo();
}

attuatAutomTemp::~attuatAutomTemp()
{
	while (!tempi.isEmpty())
		delete tempi.takeFirst();
}


attuatAutomTempNuovoN::attuatAutomTempNuovoN(QWidget *parent, char *indirizzo, QString IconaSx, QString IconaDx, QString icon,
	QString pressedIcon, int period, int number, QList<QString*> *lt)
	: attuatAutomTemp(parent, indirizzo, IconaSx, IconaDx, icon, pressedIcon, period, number, lt)
{
	assegna_tempo_display();
	stato_noto = false;
	setSecondaryText(tempo_display);
	disconnect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(attuatAutomTemp::status_changed(QList<device_status*>)));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomTempNuovoN::status_changed(QList<device_status*> sl)
{
	bool aggiorna = false;
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	qDebug("attuatAutomTempNuovoN::status_changed()");
	stat_var curr_status(stat_var::ON_OFF);

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			aggiorna = true;
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				// Update
				impostaAttivo(curr_status.get_val() != 0);
			}
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
		{
			qDebug("new timed device status variation");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				if (isActive())
				{
					impostaAttivo(false);
					aggiorna = true;
				}
				break;
			}
			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if ((isActive() && tmpval) || (!isActive() && !tmpval))
			{
				qDebug("already active, ignoring");
				break;
			}
			impostaAttivo((tmpval != 0));
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		}
		default:
			qDebug("device status of unknown type (%d)", ds->get_type());
			break;
		}
	}

	qDebug("aggiorna = %d", aggiorna);
	if (aggiorna)
		Draw();
}

void attuatAutomTempNuovoN::Attiva()
{
	dev->sendFrame(QString("*#1*%1*#2*%2##").arg(getAddress()).arg(leggi_tempo()));
}

// *#1*dove*2## 
void attuatAutomTempNuovoN::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "*2##");
}

void attuatAutomTempNuovoN::assegna_tempo_display()
{
	char *ptr;
	char tmp[50];
	int hh , mm, ss;
	// TODO: riscrivere utilizzando c++ e qt!!
	QByteArray buf = leggi_tempo().toAscii();
	strcpy(tmp, buf.constData());

	// Prende solo hh e min
	ptr = strtok(tmp, "*");
	hh = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	mm = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	ss = strtol(ptr, NULL, 10);
	qDebug("tempo = %d %d %d", hh, mm, ss);

	if (!hh && !mm) // Time in secs
		tempo_display.sprintf("%d\"", ss);
	else if (!hh) // Time in mins'
		tempo_display.sprintf("%d'", mm);
	else if (hh < 10) // Time in hh:mm
		tempo_display.sprintf("%d:%02d", hh, mm);
	else// Time in hh h
		tempo_display.sprintf("%dh", hh);
}


#define NTIMEICONS 9

attuatAutomTempNuovoF::attuatAutomTempNuovoF(QWidget *parent, char *indirizzo, QString IconaCentroSx, QString IconaCentroDx, QString IconaDx, QString t)
	: bannOn2scr(parent)
{
	SetIcons(IconaCentroSx, IconaCentroDx, IconaDx);
	setAddress(indirizzo);
	setSecondaryText("????");

	// TODO: riscrivere utilizzando qt e c++!!
	strncpy(tempo, t.isEmpty() ? t.toAscii().constData() : "0*0*0", sizeof(tempo));
	char *ptr;
	char tmp1[50];
	strcpy(tmp1, tempo);
	ptr = strtok(tmp1, "*");
	h = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	m = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	s = strtol(ptr, NULL, 10);
	val = 0;
	qDebug("tempo = %d %d %d", h, m, s);
	char tmp[50];
	if (!h && !m)
	{
		// Time in secs
		sprintf(tmp, "%d\"", s);
	}
	else if (!h)
	{
		// Time in mins'
		sprintf(tmp, "%d'", m);
	}
	else if (h < 10)
	{
		// Time in hh:mm
		sprintf(tmp, "%d:%02d", h, m);
	}
	else
	{
		// Time in hh h
		sprintf(tmp, "%dh", h);
	}

	myTimer = new QTimer(this);
	stato_noto = false;
	temp_nota = false;
	connect(myTimer,SIGNAL(timeout()),this,SLOT(update()));
	update_ok = false;
	tentativi_update = 0;
	setSecondaryText(tmp);
	connect(this,SIGNAL(dxClick()),this,SLOT(Attiva())); 
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_newtimed(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
	dev = btouch_device_cache.get_dimmer(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			this, SLOT(status_changed(QList<device_status*>)));
}

void attuatAutomTempNuovoF::status_changed(QList<device_status*> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("attuatAutomTempNuovoF::status_changed()");

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		switch (ds->get_type())
		{
		case device_status::LIGHTS:
			qDebug("Light status variation");
			aggiorna = true;
			ds->read(device_status_light::ON_OFF_INDEX, curr_status);
			qDebug("status = %d", curr_status.get_val());
			if ((isActive() && !curr_status.get_val()) || (!isActive() && curr_status.get_val()))
			{
				impostaAttivo(curr_status.get_val() != 0);
				if (!isActive())
					myTimer->stop();
				val = 0;
			}
			break;
		case device_status::DIMMER:
			qDebug("dimmer status variation, ignored");
			break;
		case device_status::DIMMER100:
			qDebug("dimmer 100 status variation, ignored");
			break;
		case device_status::NEWTIMED:
		{
			qDebug("new timed device status variation");
			ds->read(device_status_new_timed::HH_INDEX, curr_hh);
			ds->read(device_status_new_timed::MM_INDEX, curr_mm);
			ds->read(device_status_new_timed::SS_INDEX, curr_ss);
			if (!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val())
			{
				qDebug("hh == mm == ss == 0, ignoring");
				myTimer->stop();
				val = 0;
				break;
			}
			if (curr_hh.get_val()==255 && curr_mm.get_val()==255 && curr_ss.get_val()==255)
			{
				qDebug("hh == mm == ss == 255, ignoring");
				break;
			}
			uchar hnow, mnow, snow;
			hnow = curr_hh.get_val();
			mnow = curr_mm.get_val();
			snow = curr_ss.get_val();
			int tmpval = (hnow * 3600) + (mnow * 60) + snow;
			if (tmpval == val)
				return;
			val = tmpval;
			impostaAttivo((val != 0));
			update_ok = true;
			if (!val)
				myTimer->stop();
			else if (!myTimer->isActive())
				myTimer->start((1000 * val) / NTIMEICONS);
			qDebug("tempo = %d %d %d", hnow, mnow, snow);
			aggiorna = true;
			break;
		}
		default:
			qDebug("WARNING: tipo non previsto");
		}
	}

	if (aggiorna)
	{
		qDebug("invoco Draw con value = %d", value);
		Draw();
	}
}

void attuatAutomTempNuovoF::update()
{
	if (!update_ok)
	{
		if (tentativi_update++ > 2)
		{
			qDebug("Esauriti i tentativi di aggiornamento\n");
			myTimer->stop();
			return;
		}
		update_ok = false;
	}
	dev->sendInit(QString("*#1*%1*2##").arg(getAddress()));
	// imposto il timer corretto
	if (!temp_nota)
	{
		int v = h * 3600 + m * 60 + s;
		myTimer->start((1000 * v) / NTIMEICONS);
		temp_nota = true;
	}
}

void attuatAutomTempNuovoF::Attiva()
{
	dev->sendFrame(QString("*#1*%1*#2*%2##").arg(getAddress()).arg(tempo));
	tentativi_update = 0;
	update_ok = false;
	//programma un aggiornamento immediato
	temp_nota = false;
	myTimer->start(100);
	Draw();
}

// *#1*dove*2## 
void attuatAutomTempNuovoF::inizializza(bool forza)
{
	dev->sendInit("*#1*" + getAddress() + "*2##");
}

// Chiede lo stato dell'attuatore con una frame vecchia
void attuatAutomTempNuovoF::chiedi_stato()
{
	emit richStato("*#1*" + getAddress() + "##");
}

void attuatAutomTempNuovoF::SetIcons(QString i1, QString i2, QString i3)
{
	qDebug("attuatAutomTempNuovoF::SetIcons");

	// TODO: verificare se e' possibile utilizzare qualcuna delle SetIcons o
	// perlomeno la icon_dispatcher!
	if (!Icon[0])
		Icon[0] = new QPixmap();

	int pos = i2.indexOf(".");
	if (pos != -1)
		Icon[0]->load(i2.left(pos) + "off.png");
	else
		qWarning() << "Cannot find dot on image " << i2;

	if (!Icon[1])
		Icon[1] = new QPixmap();

	if (pos != -1)
		Icon[1]->load(i2.left(pos) + "on.png");
	else
		qWarning() << "Cannot find dot on image " << i2;

	if (!Icon[2])
		Icon[2] = new QPixmap();
	Icon[2]->load(i3);
	QString pressIconName = getPressName(i3);
	if (QFile::exists(pressIconName))
	{
		if (!pressIcon[2])
			pressIcon[2] = new QPixmap();
		pressIcon[2]->load(pressIconName);
	}

	for (int i = 0; i < NTIMEICONS; i++)
	{
		if (!Icon[3 + i])
			Icon[3 + i] = new QPixmap();

		pos = i1.indexOf(".");
		if (pos != -1)
			Icon[0]->load(i1.left(pos) + QString::number(i) + ".png");
		else
			qWarning() << "Cannot find dot on image " << i2;
	}
}

void attuatAutomTempNuovoF::Draw()
{
	qDebug("attuatAutomTempNuovoF::Draw(), attivo = %d, value = %d", attivo, val);
	if (attivo == 1)
	{
		int index = ((10 * val * (NTIMEICONS-1))/((h * 3600) + (m * 60) + s));
		index = (index % 10) >= 5 ? index/10 + 1 : index/10;
		if (index >= NTIMEICONS)
			index = NTIMEICONS - 1;
		qDebug("index = %d", index);
		if (Icon[3 + index] && BannerIcon2)
		{
			BannerIcon->setPixmap(*Icon[3 + index]);
			qDebug("* Icon[%d]", 3 + index);
		}
		if (Icon[1] && BannerIcon)
		{
			BannerIcon2->setPixmap(*Icon[1]);
			qDebug("** Icon[%d]", 1);
		}
	}
	else
	{
		if (Icon[0] && BannerIcon)
		{
			BannerIcon->setPixmap(*Icon[3]);
			qDebug("*** Icon[3]");
		}
		if (Icon[3] && BannerIcon2)
		{
			BannerIcon2->setPixmap(*Icon[0]);
			qDebug("**** Icon[0]");
		}
	}
	if ((dxButton) && (Icon[2]))
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
	if (SecondaryText)
	{
		QFont aFont;
		FontManager::instance()->getFont(font_items_secondarytext, aFont);
		SecondaryText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		SecondaryText->setFont(aFont);
		SecondaryText->setText(qtestoSecondario);
	}
}


attuatPuls::attuatPuls(QWidget *parent, QString where, QString IconaSx, QString icon, char tipo, int period, int number)
: bannPuls(parent)
{
	SetIcons(IconaSx, QString(), icon, QString(), period,number);
	setAddress(where);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
	type = tipo;
	impostaAttivo(1);
}

void attuatPuls::Attiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		BTouch->sendFrame(createMsgOpen("1", "1", getAddress()));
		break;
	case  VCT_SERR:
		BTouch->sendFrame(createMsgOpen("6", "10", getAddress()));
		break;
	case  VCT_LS:
		BTouch->sendFrame(createMsgOpen("6", "12", getAddress()));
		break;
	}
}

void attuatPuls::Disattiva()
{
	switch (type)
	{
	case  AUTOMAZ:
		BTouch->sendFrame(createMsgOpen("1", "0", getAddress()));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		BTouch->sendFrame(createMsgOpen("6", "11", getAddress()));
		break;
	}
}

void attuatPuls::inizializza(bool forza)
{
}

