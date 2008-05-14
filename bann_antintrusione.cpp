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
#include "genericfunz.h" // void getZoneName(...)
#include "fontmanager.h"
#include "btbutton.h"
#include "btlabel.h"
#include "device_cache.h" // btouch_device_cache
#include "../bt_stackopen/common_files/openwebnet.h" // class openwebnet
#include "tastiera.h"

/*****************************************************************
 **zonaAnti
 ****************************************************************/

void zonaAnti::setIcons()
{
	if(isActive()) {
		banner::SetIcons((unsigned char)1, sparzIName);
		banner::SetIcons((unsigned char)3, zonaAttiva);
	} else {
		banner::SetIcons((unsigned char)1, parzIName);
		banner::SetIcons((unsigned char)3, zonaNonAttiva);
	}
}


	zonaAnti::zonaAnti( QWidget *parent, 
			 	const QString & name,
				char* indirizzo,
				char* iconzona, 
				char* IconDisactive, 
				char *IconActive, 
				char *iconSparz, /*char *icon ,char *pressedIcon ,*/
				int period,
				int number )
		: bannOnIcons ( parent, name.ascii() )
{
	char    pippo[MAX_PATH];
	char    pluto[MAX_PATH];

	setAddress(indirizzo);
	qDebug("zonaAnti::zonaAnti()");
	// Mail agresta 22/06
	parzIName = IMG_PATH "btnparzializza.png";
	sparzIName = IMG_PATH "btnsparzializza.png";
	getZoneName(iconzona, &pippo[0], indirizzo, sizeof(pippo));
	//getZoneName(IconDisactive, &pluto[0], indirizzo, sizeof(pluto));
	qDebug("icons %s %s %s", pippo, parzIName, sparzIName);
	zonaAnti::SetIcons(sparzIName, &pippo[0], IconDisactive);
	if (BannerText) {
		QFont aFont;
		FontManager::instance()->getFont( font_items_bannertext, aFont );
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( aFont );
		BannerText->setText( name );
	}
	zonaAttiva = IconActive;
	zonaNonAttiva = IconDisactive;
	setChi("5");
	already_changed = false;
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_zonanti_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	// Button will be disabled later
	//nascondi(BUT2);
	//nascondi(BUT1);
	abilitaParz(true);
	setIcons();
	zonaAnti::Draw();
}

void zonaAnti::SetIcons(char *i1, char *i2, char *i3)
{
	banner::SetIcons((unsigned char)1, i1);
	banner::SetIcons((unsigned char)2, i2);
	banner::SetIcons((unsigned char)3, i3);
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
	return atoi(&(getAddress()[1]));
}

void zonaAnti::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("zonAnti::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		int s;
		switch (ds->get_type()) {
			case device_status::ZONANTI:
				qDebug("Zon.anti status variation");
				ds->read(device_status_zonanti::ON_OFF_INDEX, curr_status);
				s = curr_status.get_val();
				qDebug("stat is %d", s);
				if(!isActive() && s) {
					impostaAttivo(1);
					qDebug("new status = %d", s);
					aggiorna = true;
				} else if(isActive() && !s) {
					impostaAttivo(0);
					qDebug("new status = %d", s);
					aggiorna = true;
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna) {
		if(!already_changed) {
			already_changed = true;
			emit(partChanged(this));
		}
		setIcons();
		Draw();
	}
	delete dsi;
}

char* zonaAnti::getChi()
{
	return("5");
}

void zonaAnti::ToggleParzializza()
{
	qDebug("zonaAnti::ToggleParzializza()");
	impostaAttivo(!isActive());
	if(!already_changed) {
		already_changed = true;
		emit(partChanged(this));
	}
	setIcons();
	Draw();
}

void zonaAnti::abilitaParz(bool ab)
{
	qDebug("zonaAnti::abilitaParz(%d)", ab);
	if(ab) {
		connect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		mostra(BUT1);
	} else {
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
	openwebnet msg_open;
	char    pippo[50];
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#5*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);
}


/*****************************************************************
 **impAnti
 ****************************************************************/


	impAnti::impAnti( QWidget *parent,const char *name,char* indirizzo,char* IconOn, char* IconOff, char* IconInfo, char* IconActive, int ,int  )
:  bann2butLab( parent, name )
{      
	char pippo[MAX_PATH];

	memset(pippo,'\000',sizeof(pippo));
	tasti = NULL;
	if (IconActive)
		strncpy(&pippo[0],IconActive,strstr(IconActive,".")-IconActive-3);

	strcat(pippo,"dis");
	strcat(pippo,strstr(IconActive,"."));
	SetIcons(  IconInfo,IconOff,&pippo[0],IconOn,IconActive);
	setChi("5");
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
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));

}

void impAnti::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("impAnti::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		int s;
		switch (ds->get_type()) {
			case device_status::IMPANTI:
				qDebug("Imp.anti status variation");
				ds->read(device_status_impanti::ON_OFF_INDEX, curr_status);
				s = curr_status.get_val();
				qDebug("status = %d", s);
				if(!isActive() && s) {
					impostaAttivo(2);
					nascondi(BUT4);
					nascondi(BUT1);
					mostra(BUT2);
					aggiorna=1;
					qDebug("IMPIANTO INSERITO !!");
					emit(impiantoInserito());
					emit(abilitaParz(false));
					connect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
					insert_timer.start(5000);
					send_part_msg = false;
				} else if(isActive() && !s) {
					impostaAttivo(0);
					nascondi(BUT2);
					mostra(BUT4);
					aggiorna=1;
					qDebug("IMPIANTO DISINSERITO !!");
					emit(abilitaParz(true));
					emit(clearChanged());
					send_part_msg = false;
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}

int impAnti::getIsActive(int zona)
{
	if(le_zone[zona] !=  NULL)
		return le_zone[zona]->isActive();
	else
		return -1;
}

void impAnti::ToSendParz(bool s)
{
	send_part_msg = s;
}

void impAnti::Inserisci()
{
	qDebug("impAnti::Inserisci()");
	int s[MAX_ZONE];
	for(int i=0; i<MAX_ZONE; i++) 
	{
		if(le_zone[i] !=  NULL)
			s[i] = le_zone[i]->isActive();
		else
			s[i] = -1;
	}
	if(tasti)
		delete tasti;
	inserting = true;
	tasti = new tastiera_con_stati(s, NULL, "");
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Insert1(char*)));
	tasti->setBGColor(backgroundColor());
	tasti->setFGColor(foregroundColor());
	tasti->setMode(tastiera::HIDDEN);
	tasti->showTastiera();
	//parentWidget()->hide();
	//    this->hide();
}
void impAnti::Disinserisci()
{
	if(tasti)
		delete tasti;
	tasti = new tastiera(NULL, "");
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(DeInsert(char*)));
	tasti->setBGColor(backgroundColor());
	tasti->setFGColor(foregroundColor());
	tasti->setMode(tastiera::HIDDEN);
	tasti->showTastiera();
	//parentWidget()->hide();

	//    this->hide();
}


void impAnti::Insert1(char* pwd)
{  
	openwebnet msg_open;
	char    pippo[50];

	if (!pwd) 
		goto end;
	passwd = pwd;

	qDebug("impAnti::Insert()");
	if(!send_part_msg) {
		Insert3();
		goto end;
	}
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*50#");
	strcat(pippo,pwd);
	strcat(pippo,"#");
	for(int i=0; i<MAX_ZONE; i++)
		strcat(pippo, le_zone[i] && le_zone[i]->isActive() ? 
				"0" : "1");
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	qDebug("sending part frame %s", pippo);
	emit sendFrame(msg_open.frame_open);
	send_part_msg = false;
	part_msg_sent = true;
end:
	parentWidget()->show();
}

void impAnti::Insert2()
{
	qDebug("impAnti::Insert2()");
	if(!inserting)
		return;
	// 5 seconds between first open ack and open insert messages
	connect(&insert_timer, SIGNAL(timeout()), this, SLOT(Insert3()));
	// single shot timer
	insert_timer.start(6000, TRUE);
}

void impAnti::Insert3()
{
	qDebug("impAnti::Insert3()");
	char *pwd = passwd;
	openwebnet msg_open;
	char    pippo[50];
	emit(clearAlarms());
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*36#");
	strcat(pippo,pwd);
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
	parentWidget()->show();
	inserting = false;
	disconnect(&insert_timer, SIGNAL(timeout()), this, SLOT(Insert3()));
	connect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
	insert_timer.start(5000);
}

void impAnti::DeInsert(char* pwd)
{
	qDebug("impAnti::DeInsert()");
	if (pwd)
	{
		openwebnet msg_open;
		char    pippo[50];

		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*5*36#");
		strcat(pippo,pwd);
		strcat(pippo,"*0##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		emit sendFrame(msg_open.frame_open);
	}    
	parentWidget()->show();
	//    qDebug("disinserisco con %s", pwd);
}

void impAnti::openAckRx()
{
	qDebug("impAnti::openAckRx()");
	if(!inserting) {
		qDebug("Not inserting");
		return;
	}
	if(!part_msg_sent) return;
	part_msg_sent = false;
	qDebug("waiting 5 seconds before sending insert message");
	// Do second step of insert
	Insert2();
}

void impAnti::openNakRx()
{
	qDebug("impAnti::openNakRx()");
	if(!part_msg_sent) return;
	//Agre - per sicurezza provo a continuare per evitare di non inserire l'impianto
	openAckRx();
	part_msg_sent = false;
}

char* impAnti::getChi()
{
	return("5");
}

void impAnti::setZona(zonaAnti *za)
{
	int index = za->getIndex() - 1;
	if((index >= 0) && (index < MAX_ZONE))
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
	insert_timer.stop();
	disconnect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
	emit sendInit("*#5*0##");
}

void impAnti::hide()
{
	qDebug("impAnti::hide()");
	banner::hide();
	if(tasti && tasti->isShown()) {
		qDebug("HIDING KEYBOARD");
		tasti->hide();
	}
}
