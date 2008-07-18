/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiradio.cpp
 **
 **definizioni delle sorgenti radio implementate
 **
 ****************************************************************/

#include "sorgentiradio.h"
#include "main.h" // ICON_CICLA
#include "radio.h"
#include "../bt_stackopen/common_files/openwebnet.h" // class openwebnet
#include "device_cache.h" // btouch_device_cache
#include "device.h"

/*****************************************************************
 **sorgente_Radio
 ****************************************************************/
banradio::banradio( QWidget *parent,const char *name,char* indirizzo, int nbut, const QString & ambdescr)
: bannCiclaz( parent, name, nbut )
{
	SetIcons( ICON_CICLA,ICON_IMPOSTA,ICON_FFWD,ICON_REW);
	setAddress(indirizzo);
	myRadio = new radio(NULL,"radio", ambdescr);
	myRadio->setRDS("");
	myRadio->setFreq(0.00);
	//myRadio->setAmbDescr(ambdescr);

	//     myRadio-> setBGColor(parentWidget(TRUE)->backgroundColor() );
	//     myRadio-> setFGColor(parentWidget(TRUE)->foregroundColor() );
	QWidget *grandad = this->parentWidget(FALSE)->parentWidget(FALSE);

	myRadio->setStaz((uchar)1);

	if(nbut == 4) 
	{
		// Old difson
		old_diffson = true;
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
	} 
	else
		old_diffson = false;

	connect(this  ,SIGNAL(dxClick()),myRadio,SLOT(showRadio()));
	connect(this , SIGNAL(dxClick()),this,SLOT(startRDS()));
	if(!old_diffson)
		connect(this, SIGNAL(dxClick()), this, SLOT(richFreq()));

	//connect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(show()));
	connect(myRadio,SIGNAL(Closed()),myRadio,SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()),this,SLOT(stopRDS()));

	connect(myRadio,SIGNAL(decFreqAuto()),this,SLOT(decFreqAuto()));
	connect(myRadio,SIGNAL(aumFreqAuto()),this,SLOT(aumFreqAuto()));
	connect(myRadio,SIGNAL(decFreqMan()),this,SLOT(decFreqMan()));
	connect(myRadio,SIGNAL(aumFreqMan()),this,SLOT(aumFreqMan()));
	connect(myRadio,SIGNAL(changeStaz()),this,SLOT(changeStaz()));
	connect(myRadio,SIGNAL(memoFreq(uchar)),this,SLOT(memoStaz(uchar)));
	connect(myRadio,SIGNAL(richFreq()),this,SLOT(richFreq()));

	//     myRadio->hide();
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_radio_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));

	// Get freezed events
	connect(parent, SIGNAL(frez(bool)), myRadio, SLOT(freezed(bool)));

}

void banradio::grandadChanged(QWidget *newGrandad)
{
	qDebug("banradio::grandadChanged (%p)", newGrandad);
	QWidget *grandad = this->parentWidget(FALSE)->parentWidget(FALSE);
	//disconnect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	disconnect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
	grandad = newGrandad;
	//connect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
}

void banradio::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_freq(stat_var::FREQ);
	stat_var curr_staz(stat_var::STAZ);
	stat_var curr_rds0(stat_var::RDS0);
	stat_var curr_rds1(stat_var::RDS1);
	bool aggiorna = false;
	qDebug("bannradio::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	int tmp;
	float freq;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::RADIO:
			{
				qDebug("Radio status variation");
				ds->read(device_status_radio::FREQ_INDEX, curr_freq);
				ds->read(device_status_radio::STAZ_INDEX, curr_staz);
				ds->read(device_status_radio::RDS0_INDEX, curr_rds0);
				ds->read(device_status_radio::RDS1_INDEX, curr_rds1);
				freq = (float)curr_freq.get_val()/1000.0F;
				myRadio->setFreq(freq);
				myRadio->setStaz((uchar)curr_staz.get_val());
				QString qrds;
				tmp = curr_rds0.get_val();
				qDebug("rds0 = 0x%08x", tmp);
				qrds = tmp;
				qrds.truncate (4);
				tmp = curr_rds1.get_val();
				qDebug("rds1 = 0x%08x", tmp);
				qrds += tmp;
				qrds.truncate (8);
				qDebug("*** setting rds to %s", qrds.ascii());
				myRadio->setRDS( qrds );
				aggiorna=1;
				break;
			}
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		myRadio->draw();
	delete dsi;
}

void banradio::pre_show()
{
	openwebnet msg_open;
	char    pippo[50];

	if(old_diffson) {
		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*#16*");
		strcat(pippo,getAddress());
		strcat(pippo,"*6##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

		dev->sendInit(msg_open.frame_open);
	}

	QWidget::show();
}

void banradio::show()
{

	pre_show();

	if(!old_diffson)
	{
		if (parentWidget()->parentWidget()->parentWidget(TRUE))
			nascondi(BUT2);
		else
			mostra(BUT2);
	}
	mostra(BUT2);
}

void banradio::hide()
{
	qDebug("banradio::hide()");
	if (myRadio->isShown())
		stopRDS();
	myRadio->hide();
	QWidget::hide();
}

void banradio::SetTextU( const QString & qtext )
{
	banner::SetTextU( qtext );
	myRadio->setNameU( qtext );
}

void banradio::ciclaSorg()
{
	openwebnet msg_open;
	qDebug("banradio::ciclaSorg()");
	char pippo[50];
	char amb[3];

	sprintf(amb, getAddress());
	memset(pippo,'\000',sizeof(pippo));
	sprintf(pippo,"*22*22#4#1*5#2#%c##", amb[2]);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	dev->sendFrame(msg_open.frame_open);
}

void banradio::decBrano()
{
	openwebnet msg_open;

	msg_open.CreateMsgOpen("16","6101",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void banradio::aumBrano()
{
	openwebnet msg_open;

	msg_open.CreateMsgOpen("16","6001",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}
void banradio::aumFreqAuto()
{
	openwebnet msg_open;

	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5000",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}
void banradio::decFreqAuto()
{
	openwebnet msg_open;

	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5100",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void banradio::aumFreqMan()
{
	openwebnet msg_open;
	float f;

	f=myRadio->getFreq();
	if (f<108.00)
		f+=0.05;
	else
		f=87.50;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5001",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}

void banradio::decFreqMan()
{
	openwebnet msg_open;
	float f;

	f=myRadio->getFreq();
	if (f>87.50)
		f-=0.05;
	else
		f=108.00;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5101",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}
void banradio::changeStaz()
{
	openwebnet msg_open;
	char amb[2];

	sprintf(amb, getAddress());
	if(!old_diffson)
		if(amb[1] == '0')
			amb[1] = '1';

	msg_open.CreateMsgOpen("16","6001",amb,"");
	dev->sendFrame(msg_open.frame_open);
}

void banradio::memoStaz(uchar st)
{
	openwebnet msg_open;
	char    pippo[50],pippa[10];
	unsigned int ic;
	// double f;

	memset(pippo,'\000',sizeof(pippo));
	memset(pippa,'\000',sizeof(pippa));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*#10*");
	memset(pippa,'\000',sizeof(pippa));
	//     f=myRadio->getFreq()*1000;
	//     sprintf(pippa,"%.0f",f);
	//     qDebug("pippa2  :%s",&pippa[0]);
	//     strcat(pippo,pippa);
	//     strcat(pippo,"*");
	ic=(unsigned int)st;
	sprintf(pippa,"%01hu",st);
	//   qDebug("pippa1  :%01hu",&pippa[0]);
	strcat(pippo,pippa);
	strcat(pippo,"##");
	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	dev->sendFrame(msg_open.frame_open);
}
void banradio::startRDS()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*101*");
	strcat(pippo,getAddress());
	//     strcat(pippo,"*8##");
	strcat(pippo,"##");

	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	dev->sendFrame(msg_open.frame_open);
}

void banradio::stopRDS()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*102*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");

	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	dev->sendFrame(msg_open.frame_open);
}

void banradio::richFreq()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*6##");

	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	dev->sendFrame(msg_open.frame_open);
}

void banradio::setBGColor(int r, int g , int b)
{
	setBGColor(QColor(r,g,b));
}
void banradio::setFGColor(int r, int g, int b)
{
	setFGColor(QColor(r,g,b));
}
void banradio::setBGColor(QColor c)
{
	if (c.isValid())
	{
		myRadio->setBGColor(c);
		banner::setBGColor(c);
	}
}
void banradio::setFGColor(QColor c)
{
	if (c.isValid())
	{
		myRadio->setFGColor(c);
		banner::setFGColor(c);
	}
}



void banradio::inizializza(bool forza)
{ 
}


/*****************************************************************
 ** Sorgente radio diffusione sonora multicanale
 ****************************************************************/
sorgenteMultiRadio::sorgenteMultiRadio( QWidget *parent, const char *name, char* indirizzo, char* Icona1, char* Icona2, char *Icona3, char *ambDescr)
: banradio( parent, name, indirizzo, 3, ambDescr)
{       
	qDebug("sorgenteMultiRadio::sorgenteMultiRadio() : %s %s %s", Icona1, Icona2, Icona3);
	
	SetIcons(Icona1, Icona2, NULL, Icona3);
	
	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	indirizzo_semplice = QString(indirizzo);
	indirizzi_ambienti.clear();
	//connect(this, SIGNAL(dxClick()), this, SLOT(configura()));
	//connect(this, SIGNAL(csxClick()), this, SLOT(cicla()));
	multiamb = false;
}

void sorgenteMultiRadio::attiva()
{
	qDebug("sorgenteMultiRadio::attiva()");
	char pippo[50];
	openwebnet msg_open;
  
	if(!multiamb) {
		memset(pippo,'\000',sizeof(pippo));
		sprintf(pippo,"*22*35#4#%d#%d*4#%d##",indirizzo_ambiente, indirizzo_semplice.toInt(), indirizzo_ambiente);
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		dev->sendFrame(msg_open.frame_open);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	} else {
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI",
				indirizzi_ambienti.count());
		for ( QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it ) {
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*22*0#4#");
			strcat(pippo,(*it));
			strcat(pippo,"*6");
			strcat(pippo,"##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*#16*1000*11##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*22*1#4#");
			strcat(pippo,(*it));
			strcat(pippo,"*2#");
			strcat(pippo, indirizzo_semplice);
			strcat(pippo,"##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
			memset(pippo,'\000',sizeof(pippo));
			strcat(pippo,"*#16*1000*11##");
			msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
			dev->sendFrame(msg_open.frame_open);
		}
	}
}

void sorgenteMultiRadio::ambChanged(const QString & ad, bool multi, char *indamb)
{
	// FIXME: PROPAGA LA VARIAZIONE DI DESCRIZIONE AMBIENTE
	qDebug("sorgenteMultiRadio::ambChanged(%s, %d, %s)", ad.ascii(), multi, indamb);
	if(!multi) {
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		QString *dove = new QString(
				QString::number(100 + 
					indirizzo_ambiente * 10 +
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	} else {
		multiamb = true;
		QString *dove = new QString(
				QString::number(100 + 
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	}
	myRadio->setAmbDescr( ad );
}

void sorgenteMultiRadio::show()
{
	banradio::pre_show();
}

void sorgenteMultiRadio::addAmb(char *a)
{
	qDebug("sorgenteMultiRadio::addAmb(%s)", a);
	indirizzi_ambienti += QString(a);
}
