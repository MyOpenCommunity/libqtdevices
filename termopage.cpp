/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** termopage.cpp
 **
 **definizioni della zona di termoregolazione
 **
 ****************************************************************/

#include "termopage.h"
#include "../bt_stackopen/common_files/openwebnet.h" // class openwebnet
#include "device_cache.h" // btouch_device_cache
#include "btbutton.h"
#include "btlabel.h"
#include "buttons_bar.h"
#include "sottomenu.h"

#include <stdlib.h> // atoi

/*****************************************************************
 **termoPage
 ****************************************************************/
termoPage::termoPage(QWidget *parent, devtype_t _devtype, const char *name, const char *_indirizzo,
		QPtrList<QString> &icon_names,
		QColor SecondForeground, int _type, const char *_ind_centrale) :
	bannTermo(parent, name, SecondForeground, _devtype),
	type(_type),
	ind_centrale(_ind_centrale),
	indirizzo(_indirizzo)
{
	/*
	 * Determine where address for this component.
	 * 
	 * If type is 0, the address if simple, as in 99 zones there is only
	 * one regulator.  When type == 1, setup for 4 zones.
	 * The address if simple even for stand alone temp probes.
	 */
	if (devtype == SINGLE_PROBE || devtype == EXT_SINGLE_PROBE || type == 0)
		setAddress(indirizzo.ascii());
	else
	{
		QString new_address = indirizzo + "#" + ind_centrale;
		setAddress(const_cast<char *>(new_address.ascii()));
	}

	qDebug("termoPage::termoPage: type=%d, ind_centrale=%s, indirizzo=%s, where=%s",
	       type, ind_centrale.ascii(), indirizzo.ascii(), getAddress());

	SetIcons((char *)icon_names.at(0)->ascii(), (char *)icon_names.at(1)->ascii(),
	         (char *)icon_names.at(4)->ascii(), (char *)icon_names.at(5)->ascii());

	strncpy(&manIco[0], icon_names.at(2)->ascii(), sizeof(manIco));
	strncpy(&autoIco[0], icon_names.at(3)->ascii(), sizeof(autoIco));
	connect(this, SIGNAL(dxClick()), SLOT(aumSetpoint()));
	connect(this, SIGNAL(sxClick()), SLOT(decSetpoint()));
	setChi("4");

	stato = device_status_thermr::S_MAN;

	bool fancoil = (devtype == THERMO_99_ZONES_FANCOIL || devtype == THERMO_4_ZONES_FANCOIL);

	switch (devtype)
	{
	case THERMO_99_ZONES:
		dev = btouch_device_cache.get_thermr_device(getAddress(), device_status_thermr::Z99,
			fancoil, ind_centrale.ascii(), indirizzo.ascii());
		break;
	case THERMO_99_ZONES_FANCOIL:
		dev = btouch_device_cache.get_thermr_device(getAddress(), device_status_thermr::Z99,
			fancoil, ind_centrale.ascii(), indirizzo.ascii());
		connect(fancoil_buttons, SIGNAL(clicked(int)), SLOT(handleFancoilButtons(int)));
		break;
	case THERMO_4_ZONES:
		dev = btouch_device_cache.get_thermr_device(getAddress(), device_status_thermr::Z4,
			fancoil, ind_centrale.ascii(), indirizzo.ascii());
		break;
	case THERMO_4_ZONES_FANCOIL:
		dev = btouch_device_cache.get_thermr_device(getAddress(), device_status_thermr::Z4,
			fancoil, ind_centrale.ascii(), indirizzo.ascii());
		connect(fancoil_buttons, SIGNAL(clicked(int)), SLOT(handleFancoilButtons(int)));
		break;
	case SINGLE_PROBE:
		dev = btouch_device_cache.get_temperature_probe(getAddress(), false);
		break;
	case EXT_SINGLE_PROBE:
		dev = btouch_device_cache.get_temperature_probe(getAddress(), true);
		break;
	default:
		qDebug("termoPage::termoPage(): Unknown devtype_t!");
	}

	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), SLOT(status_changed(QPtrList<device_status>)));
	delta_setpoint = 0;
	connect(&setpoint_timer, SIGNAL(timeout()), SLOT(sendSetpoint()));
	sxButton->setAutoRepeat(true);
	dxButton->setAutoRepeat(true);
}

void termoPage::handleFancoilButtons(int button_number)
{
	qDebug("termoPage::handleFancoilButtons()");

	// FIXME: magic numbers below should be defined in OpenMsg class
	int speed;
	switch (button_number)
	{
	case 0: // MIN SPEED
		speed = 1;
		break;
	case 1: // MEDIUM_SPEED
		speed = 2;
		break;
	case 2: // MAXIMUM SPEED
		speed = 3;
		break;
	case 3: // AUTO SPEED
		speed = 0;
		break;
	default:
		qDebug("bannTermo::handleFancoilButtons --> Errore, ID pulsante NON definito!");
		break;
	}

	QString command = QString("*#4*%1*#11*%2##").arg(indirizzo).arg(speed);
	openwebnet open_cmd;
	open_cmd.CreateMsgOpen((char *)command.ascii(), command.length());
	emit sendFrame(open_cmd.frame_open);

	/*
	 * Read back the set value to force an update to other devices
	 * monitoring this dimension.
	 */
	command = QString("*#4*%1*11##").arg(indirizzo);
	openwebnet open_query;
	open_query.CreateMsgOpen((char *)command.ascii(), command.length());
	emit sendFrame(open_query.frame_open);

	fancoil_buttons->setToggleStatus(button_number);
}


void termoPage::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_stat(stat_var::STAT);
	stat_var curr_local(stat_var::LOCAL);
	stat_var curr_sp(stat_var::SP);
	stat_var curr_temp(stat_var::TEMPERATURE);

	openwebnet msg_open;
	int delta = 0;
	bool aggiorna = false;
	qDebug("termoPage::status_changed()");
	QPtrListIterator<device_status> *dsi = new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0)
	{
		switch (ds->get_type())
		{
			case device_status::THERMR:
				qDebug("Th. regulator status variation");
				ds->read(device_status_thermr::STAT_INDEX, curr_stat);
				ds->read(device_status_thermr::LOCAL_INDEX, curr_local);
				ds->read(device_status_thermr::SP_INDEX, curr_sp);

				if (delta_setpoint == 1)
				{
					//delta = atoi(strstr(&setpoint[0],".")+1);
					//delta+=atoi(&setpoint[0])*10;
					int idx = qsetpoint.find(".");
					const char *ptr = qsetpoint.ascii() + idx +1;
					delta = atoi( ptr ) ;
					delta+=atoi( qsetpoint.ascii() ) * 10;
					curr_sp.set_val(delta);
					ds->write_val((int)device_status_thermr::SP_INDEX, curr_sp);
					ds->read(device_status_thermr::SP_INDEX, curr_sp);
					delta_setpoint = 0;
				}
				qDebug("stat = %d", curr_stat.get_val());
				qDebug("loc = %d", curr_local.get_val());
				qDebug("sp = %d", curr_sp.get_val());
				if (curr_stat.initialized())
				{
					switch(curr_stat.get_val()) {
						case device_status_thermr::S_MAN:
							qDebug("stato S_MAN");
							stato = device_status_thermr::S_MAN;
							nascondi(ICON);
							tempImp->show();
							aggiorna = true;
							if (devtype == THERMO_99_ZONES || devtype == THERMO_99_ZONES_FANCOIL) 
							{
								mostra(BUT1);
								mostra(BUT2);

								if (isShown())
								{
									((sottoMenu*)parentWidget())->setNavBarMode(4,&autoIco[0]);
									((sottoMenu*)parentWidget())->forceDraw();
								}
							}
							break;
						case device_status_thermr::S_AUTO:
							qDebug("stato S_AUTO");
							stato = device_status_thermr::S_AUTO;
							nascondi(ICON);
							tempImp->show();
							aggiorna = true;
							if (devtype == THERMO_99_ZONES || devtype == THERMO_99_ZONES_FANCOIL) 
							{
								nascondi(BUT1);
								nascondi(BUT2);

								if (isShown())
								{
									((sottoMenu*)parentWidget())->setNavBarMode(4,&manIco[0]);
									((sottoMenu*)parentWidget())->forceDraw();
								}
							}
							break;
						case device_status_thermr::S_ANTIGELO:
						case device_status_thermr::S_TERM:
						case device_status_thermr::S_GEN:
							qDebug("stato S_TERM");
							mostra(ICON);
							tempImp->hide();
							impostaAttivo(1);
							aggiorna= true;
							stato = device_status_thermr::S_TERM;
							
							if (devtype == THERMO_99_ZONES || devtype == THERMO_99_ZONES_FANCOIL) 
							{
								nascondi(BUT1);
								nascondi(BUT2);
							}

							if (isShown())
							{
								((sottoMenu*)parentWidget())->setNavBarMode(3, (char *)"");
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						case device_status_thermr::S_OFF:
							qDebug("stato S_OFF");
							mostra(ICON);
							tempImp->hide();
							impostaAttivo(0);
							aggiorna= true;
							stato = device_status_thermr::S_OFF;

							if (devtype == THERMO_99_ZONES || devtype == THERMO_99_ZONES_FANCOIL) 
							{
								nascondi(BUT1);
								nascondi(BUT2);
							}

							if (isShown())
							{

								((sottoMenu*)parentWidget())->setNavBarMode(3, (char *)"");
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						default:
							qDebug("unk status");
							break;
					}
				}
				if(curr_local.initialized()) 
				{
					qDebug("loc = %d", curr_local.get_val());
					switch(curr_local.get_val()) 
					{
						case 0:
							val_imp=3;
							isOff=0;
							isAntigelo=0;
							break;
						case 1:
							val_imp=4;
							isOff=0;
							isAntigelo=0;
							break;
						case 2:	
							val_imp=5;
							isOff=0;
							isAntigelo=0;
							break;
						case 3:	
							val_imp=6;
							isOff=0;
							isAntigelo=0;
							break;
						case 11:	
							val_imp=2;
							isOff=0;
							isAntigelo=0;
							break;
						case 12:	
							val_imp=1;
							isOff=0;
							isAntigelo=0;
							break;
						case 13:	
							val_imp=0;
							isOff=0;
							isAntigelo=0;
							break;
						case 4:	
							val_imp=0;
							isOff=1;
							isAntigelo=0;
							break;
						case 5:	
							val_imp=0;
							isOff=0;
							isAntigelo=1;
							break;
					}
				}
				if((curr_sp.initialized()) && (delta_setpoint == 0))
				{
					//Set Point
					float icx;
					char	tmp[10];
					icx = curr_sp.get_val();
					qDebug("temperatura setpoint: %d",(int)icx);
					qsetpoint = "";
					if (icx>=1000)
					{
						qsetpoint = "-";
						icx=icx-1000;
					}
					icx/=10;
					sprintf(tmp,"%.1f",icx);
					qsetpoint += tmp;
					qsetpoint += "\272C";
					aggiorna=1;
					break;
				}
				break;
			case device_status::TEMPERATURE_PROBE:
				ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
				qDebug("temperature probe status variation");
				//Temperatura misurata
				float icx;
				char	tmp[10];

				icx = curr_temp.get_val();
				qDebug("temperatura misurata: %d",(int)icx);
				qtemp = "";
				if (icx>=1000)
				{
					qtemp = "-";
					icx=icx-1000;
				}
				icx/=10;
				sprintf(tmp,"%.1f",icx);
				qDebug("-1: tmp: %.1f - %s",icx, &tmp[0]);
				qtemp += tmp;
				qDebug("-1: temp: %s", qtemp.ascii());
				qtemp +="\272C";
				qDebug("-2: temp: %s", qtemp.ascii());
				aggiorna=1;
				break;
			case device_status::FANCOIL:
			{
				stat_var speed_var(stat_var::FANCOIL_SPEED);
				ds->read((int)device_status_fancoil::SPEED_INDEX, speed_var);

				// Set the fancoil Button in the buttons bar
				int button_to_set_up = -1;
				switch (speed_var.get_val())
				{
				case 0: // auto
					button_to_set_up = 3;
					break;
				case 1: // min
					button_to_set_up = 0;
					break;
				case 2: // medium
					button_to_set_up = 1;
					break;
				case 3: // max
					button_to_set_up = 2;
					break;
				default:
					qDebug("Fancoil speed val out of range (%d)", speed_var.get_val());
				}
				if (button_to_set_up != -1)
					fancoil_buttons->setToggleStatus(button_to_set_up);

				qDebug("termoPage::status_changed: fancoil new speed=%d", speed_var.get_val());
				break;
			}
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

char* termoPage::getChi()
{
	return("4");
}

char* termoPage::getAutoIcon()
{
	return &autoIco[0];
}

char* termoPage::getManIcon()
{
	return &manIco[0];
}

void termoPage::autoMan()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));


	if    (stato==device_status_thermr::S_MAN)
	{
		strcat(pippo,"*4*311*#");
		strcat(pippo,getAddress());
	}
	else 
	{
		int idx = qsetpoint.find(".");
		const char *ptr = qsetpoint.ascii() + idx +1;
		int icx = atoi( ptr );
		
		QChar qc = qsetpoint.ref(0);
		if( qc == '-')
			icx=-icx;
		
		icx+=atoi( qsetpoint.ascii() ) * 10;
		if (icx<0)
			icx=(abs(icx))+1000;
		strcat(pippo,"*#4*#");
		strcat(pippo,getAddress());
		strcat(pippo,"*#14*");
		sprintf(pippo,"%s%04d",pippo,icx);
		strcat(pippo,"*3");
	}

	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
}

void termoPage::hide()
{
	disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));
	banner::hide();
}

void termoPage::show()
{
	disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));    /*mi assicuro che ci sia un solo collegamento*/
	connect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));
	banner::show();
}



void termoPage::aumSetpoint()
{
	int idx = qsetpoint.find(".");
	const char *ptr = qsetpoint.ascii() + idx +1;
	int icx = atoi( ptr );
	icx+=atoi( qsetpoint.ascii() ) * 10;
	
	if (icx>395)
		return;
	else
		icx=(abs(icx))+5;
	SetSetpoint(icx);
	setpoint_timer.start(2000);
	delta_setpoint = 1;
}

void termoPage::decSetpoint()
{
	int idx = qsetpoint.find(".");
	const char *ptr = qsetpoint.ascii() + idx +1;
	int icx = atoi( ptr );
	icx+=atoi( qsetpoint.ascii() ) * 10;
	
	if (icx<35)
		return;
	else
		icx=(abs(icx))-5;
	SetSetpoint(icx);
	setpoint_timer.start(2000);
	delta_setpoint = 1;
}

void termoPage::SetSetpoint(float icx)
{
	char tmp[10];

	qDebug("NEW temperatura setpoint: %d",(int)icx);
	qsetpoint = "";
	if (icx>=1000)
	{
		qsetpoint = "-";
		icx=icx-1000;
	}
	icx/=10;
	sprintf(tmp,"%.1f",icx);
	qsetpoint += tmp;
	qsetpoint +="\272C";
	Draw();
}

void termoPage::sendSetpoint()
{
	openwebnet msg_open;
	char pippo[50];

	qDebug("termoPage::sendSetpoint()");
	memset(pippo,'\000',sizeof(pippo));
	int idx = qsetpoint.find(".");
	const char *ptr = qsetpoint.ascii() + idx +1;
	int icx = atoi( ptr );
	icx+=atoi( qsetpoint.ascii() ) * 10;
	
	memset(pippo,'\000',sizeof(pippo));
	/// FRAME VERSO LA CENTRALE
// 	strcat(pippo,"*#4*#");
	strcat(pippo,getAddress());
	strcat(pippo,"*#14*");
	sprintf(pippo,"%s%04d",&pippo[0],icx);
	strcat(pippo,"*3##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
	setpoint_timer.stop();
}


void termoPage::inizializza(bool forza)
{ 
	// See frame_interpreter.cpp
}

