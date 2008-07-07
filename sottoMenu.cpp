/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **sottoMenu.cpp
 **
 **Sottomenù sottoMenu
 **
 ****************************************************************/
#include "main.h"
#include "bannsettings.h"

#include <qpixmap.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qregexp.h>

#include "sottomenu.h"
#include "postoext.h"
#include "setitems.h"
#include "btbutton.h"
#include "diffsonora.h"
#include "versio.h"
#include "xmlconfhandler.h"
#include "bannfrecce.h"
#include "dimmers.h"
#include "actuators.h"
#include "scenari.h"
#include "amplificatori.h"
#include "poweramplifier.h"
#include "automatismi.h"
#include "sorgentiradio.h"
#include "sorgentiaux.h"
#include "sorgentimedia.h"
#include "carico.h"
#include "bann_antintrusione.h"
#include "bannfullscreen.h"

sottoMenu::sottoMenu( QWidget *parent, const char *name, uchar navBarMode,int wi,int hei, uchar n)
: QWidget( parent, name )
{
	numRighe=n;
	hasNavBar=navBarMode;
	width=wi;
	height=hei;
	freez=FALSE;

	setGeometry(0,0,width,height);
	setFixedSize(QSize(width, height));

	#ifdef IPHONE_MODE
		setMouseTracking(true);
	#endif

	bannNavigazione  = NULL;

	if (navBarMode)
	{
		bannNavigazione  = new bannFrecce(this,"bannerfrecce",navBarMode);

		bannNavigazione  ->setGeometry( 0 , height-height/numRighe ,width , height/numRighe );
		connect(bannNavigazione  ,SIGNAL(backClick()),this,SIGNAL(Closed()));
		connect(bannNavigazione  ,SIGNAL(upClick()),this,SLOT(goUp()));
		connect(bannNavigazione  ,SIGNAL(downClick()),this,SLOT(goDown()));
		connect(bannNavigazione  ,SIGNAL(forwardClick()),this,SIGNAL(goDx()));

		#ifdef IPHONE_MODE
			connect(this, SIGNAL(goUP()), bannNavigazione  ,SIGNAL(upClick()));
			connect(this, SIGNAL(goDO()), bannNavigazione  ,SIGNAL(downClick()));
		#endif
	}

	elencoBanner.setAutoDelete( TRUE );
	indice=0;
	indicold=100;
	#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor (QCursor (blankCursor));
	if (!parentWidget()) showFullScreen();
	#endif
}

void sottoMenu::setNavBarMode(uchar navBarMode, char* IconBut4)
{
	qDebug("strcmp(IconBut4,&iconName[0]) : %s - %s", IconBut4, iconName);
	if(navBarMode!=hasNavBar)
	{
		if(bannNavigazione)
		{
			bannNavigazione->hide();
			delete( bannNavigazione );
			bannNavigazione=NULL;
		}
		if (navBarMode)
		{
			bannNavigazione  = new bannFrecce(this,"bannerfrecce",navBarMode, IconBut4);

			bannNavigazione -> setGeometry( 0, height-height/NUM_RIGHE/*numRighe*/, width, height/NUM_RIGHE/*numRighe */);
			bannNavigazione -> setBGColor(backgroundColor());
			bannNavigazione -> setFGColor(foregroundColor());
			connect(bannNavigazione, SIGNAL(backClick()), this, SIGNAL(Closed()));
			connect(bannNavigazione, SIGNAL(upClick()), this, SLOT(goUp()));
			connect(bannNavigazione, SIGNAL(downClick()), this, SLOT(goDown()));
			connect(bannNavigazione, SIGNAL(forwardClick()), this, SIGNAL(goDx()));

		}
		hasNavBar=navBarMode;
		setModeIcon(IconBut4);
	}
	else if (strcmp(IconBut4,iconName))
	{
		qDebug("strcmp(IconBut4,&iconName[0]) : %s - %s", iconName, IconBut4);
		setModeIcon(IconBut4);
	}
}

void sottoMenu::setModeIcon(char* iconBut4)
{
	strncpy(iconName, iconBut4, MAX_PATH);
	bannNavigazione -> SetIcons(1, iconName);
	bannNavigazione -> Draw();
}

void sottoMenu::setBGColor(int r, int g, int b)
{	
	setBGColor (QColor :: QColor(r,g,b));    
}
void sottoMenu::setFGColor(int r, int g, int b)
{
	setFGColor (QColor :: QColor(r,g,b));
}

void sottoMenu::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	if ( bannNavigazione)
		bannNavigazione->setBGColor(c);
}
void sottoMenu::setFGColor(QColor c)	
{
	setPaletteForegroundColor(c);
	if ( bannNavigazione)
		bannNavigazione->setFGColor(c);
}

void sottoMenu::setAllBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	if ( bannNavigazione)
		bannNavigazione->setBGColor(c);

    for (banner *b = elencoBanner.first(); b; b = elencoBanner.next())
		b->setBGColor(c);
}

void sottoMenu::setAllFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	if ( bannNavigazione)
		bannNavigazione->setFGColor(c);

    for (banner *b = elencoBanner.first(); b; b = elencoBanner.next())
		b->setFGColor(c);
}

int sottoMenu::setBGPixmap(char* backImage)
{
	QPixmap Back; 
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);      
		return (0);
	}    
	return (1);
}


int sottoMenu::addItemU(char tipo, const QString & qdescrizione, void *indirizzo,
		QPtrList<QString> &icon_names,
		int periodo, int numFrame,
		QColor SecondForeground,
		char *descr1, char *descr2, char *descr3, char *descr4,
		int par3, int par4,
		QPtrList<QString> *lt, QPtrList<scenEvo_cond> *lc,
		QString action, QString light, QString key, QString unknown,
		QValueList<int> sstart, QValueList<int> sstop,
		QString txt1, QString txt2, QString txt3)
{
	const char * descrizione = qdescrizione.ascii();

	char *IconaSx = (char *)safeAt(icon_names, 0)->ascii();
	char *IconaDx = (char *)safeAt(icon_names, 1)->ascii();
	char *icon = (char *)safeAt(icon_names, 2)->ascii();
	char *pressedIcon = (char *)safeAt(icon_names, 3)->ascii();
	char *icoEx1 = (char *)safeAt(icon_names, 4)->ascii();
	char *icoEx2 = (char *)safeAt(icon_names, 5)->ascii();
	char *icoEx3 = (char *)safeAt(icon_names, 6)->ascii();

	qDebug("sottoMenu::addItem (%p)", lt);
	switch (tipo){
		case ATTUAT_AUTOM:   elencoBanner.append(new attuatAutom(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
		case DIMMER:   elencoBanner.append(new dimmer(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon, icoEx1)); break;
		case DIMMER_100: elencoBanner.append(new dimmer100(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon, icoEx1, par3, par4)); break;
		case SCENARIO:   elencoBanner.append(new scenario(this,descrizione ,(char*)indirizzo,IconaSx)); break;
		case GR_ATTUAT_AUTOM: elencoBanner.append(new grAttuatAutom(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;
		case GR_DIMMER: elencoBanner.append(new grDimmer(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;
		case GR_DIMMER100: elencoBanner.append(new grDimmer100(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,pressedIcon, periodo, numFrame, sstart, sstop)); break;
		case CARICO: elencoBanner.append(new carico(this,descrizione ,(char*)indirizzo,IconaSx)); break;
		case ATTUAT_AUTOM_INT: elencoBanner.append(new attuatAutomInt(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
		case ATTUAT_AUTOM_INT_SIC: elencoBanner.append(new attuatAutomIntSic(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame)); break;
		case ATTUAT_AUTOM_TEMP: elencoBanner.append(new attuatAutomTemp(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame,lt)); break;
		case ATTUAT_AUTOM_TEMP_NUOVO_N: elencoBanner.append(new attuatAutomTempNuovoN(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,periodo,numFrame, lt)); break;
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
						if(!lt->count())
							lt->append(new QString(""));
						elencoBanner.append(new attuatAutomTempNuovoF(this,descrizione, (char*)indirizzo,IconaSx, IconaDx, icon, lt->at(0)->ascii())); break;
		case GR_ATTUAT_INT: elencoBanner.append(new grAttuatInt(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,periodo,numFrame)); break;
		case ATTUAT_AUTOM_PULS:   elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/AUTOMAZ,periodo,numFrame)); break;
		case ATTUAT_VCT_LS: elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/VCT_LS,periodo,numFrame)); break;
		case ATTUAT_VCT_SERR: elencoBanner.append(new attuatPuls(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,/*icon,*/VCT_SERR,periodo,numFrame)); break;
		case AUTOM_CANC_ATTUAT_VC: elencoBanner.append(new automCancAttuatVC(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx)); break;
		case AUTOM_CANC_ATTUAT_ILL: elencoBanner.append(new automCancAttuatIll(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx, (lt->at(0)))); break;
		case SET_DATA_ORA : elencoBanner.append(new setDataOra(this,descrizione )); break;
		case POWER_AMPLIFIER:   elencoBanner.append(new PowerAmplifier(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon,icoEx1)); break;
		case POWER_AMPLIFIER_PRESET: elencoBanner.append(new PowerAmplifierPreset(this, descrizione)); break;
		case POWER_AMPLIFIER_TREBLE: elencoBanner.append(new PowerAmplifierTreble(this, descrizione, SecondForeground)); break;
		case POWER_AMPLIFIER_BASS: elencoBanner.append(new PowerAmplifierBass(this, descrizione, SecondForeground)); break;
		case POWER_AMPLIFIER_BALANCE: elencoBanner.append(new PowerAmplifierBalance(this, descrizione, SecondForeground)); break;
		case POWER_AMPLIFIER_LOUD: elencoBanner.append(new PowerAmplifierLoud(this, descrizione)); break;
		case AMPLIFICATORE:   elencoBanner.append(new amplificatore(this,descrizione ,(char*)indirizzo,IconaSx, IconaDx,icon, pressedIcon)); break;
		case SORGENTE_AUX : elencoBanner.append(new sorgente_aux(this,descrizione ,(char*)indirizzo)); break;
		/// New Multimedia Audio Source.  NOTE: numFrame parametere name is not significative: it's the (cut down) where address
		case SORGENTE_MULTIM:
			elencoBanner.append(new BannerSorgenteMultimedia(this, descrizione, (char *)indirizzo, numFrame, 4));
			break;
		case SORGENTE_MULTIM_MC:
			elencoBanner.append(new BannerSorgenteMultimediaMC(this, descrizione, (char *)indirizzo, numFrame, IconaSx, IconaDx, icon));
			break;
		case SORGENTE_RADIO : elencoBanner.append(new banradio(this,descrizione ,(char*)indirizzo)); break;
		case GR_AMPLIFICATORI: elencoBanner.append(new grAmplificatori(this,descrizione ,indirizzo,IconaSx, IconaDx,icon,pressedIcon)); break;
		case SET_SVEGLIA: elencoBanner.append(new impostaSveglia(this,descrizione ,(contdiff*)indirizzo, IconaSx,IconaDx, icon, pressedIcon, periodo, numFrame,icoEx1,par3)); break;
		case CALIBRAZIONE:
				  {
					  calibration *c = new calibration(this,descrizione ,IconaSx);
					  elencoBanner.append(c);
					  connect(c, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
					  connect(c, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
					  break;
				  }
		/// New parameters for termoPage (that is in items.cpp)
		// WARNING! par3 is <type> tag of TERMO page, par4 is <ind_centrale>
		/* Ecco i nuovi Componenti
		TERMO_FANCOIL=52,           Thermoregulation zone with fan-coil control
		TERMO_4Z=53,                4 Zones Thermoregulation
		TERMO_4Z_FANCOIL=54,        4 Zones Thermoregulation with fan-coil control
		TERMO_EXTPROBE=55,          External not-controlled probe
		TERMO_PROBE=56,             Not-controlled probe
		*/
		case ZONANTINTRUS:
			elencoBanner.append(new zonaAnti(this,descrizione, (char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
			break;
		case IMPIANTINTRUS:
			elencoBanner.append(new impAnti(this,descrizione ,(char*)indirizzo, IconaSx, IconaDx, icon, pressedIcon));
			break;
		case SUONO:
			elencoBanner.append(new impBeep(this,descrizione , IconaSx,IconaDx, icon));
			break;
		case CONTRASTO:
			elencoBanner.append(new impContr(this,descrizione , IconaSx,IconaDx));
			break;
		case VERSIONE:
			elencoBanner.append(new machVers(this,descrizione , (versio*)indirizzo, IconaSx));
			break;
		case PROTEZIONE: elencoBanner.append(new impPassword(this,descrizione ,IconaDx,icon,pressedIcon, IconaSx, periodo)); break;
		case MOD_SCENARI:  elencoBanner.append(new gesModScen(this, descrizione ,(char*)indirizzo, IconaSx,IconaDx,icon,pressedIcon, icoEx1, icoEx2, icoEx3));break;
		case SCENARIO_EVOLUTO: elencoBanner.append(new scenEvo(this, descrizione, lc, IconaSx, IconaDx, icon, pressedIcon, NULL, NULL, NULL, action, par3));
				       break;
		case SCENARIO_SCHEDULATO: elencoBanner.append(new scenSched(this, descrizione, IconaSx, IconaDx, icon, pressedIcon, descr1, descr2, descr3, descr4)); break;
					  // FIXME: VERIFICA I PARAMETRI !!!
		case POSTO_ESTERNO: elencoBanner.append(new postoExt(this, descrizione, IconaSx, IconaDx, icon, pressedIcon, (char *)indirizzo, (char *)light.ascii(), (char *)key.ascii(), (char *)unknown.ascii())); break;
		case SORG_RADIO:
				    elencoBanner.append(new sorgenteMultiRadio(this, descrizione, (char *)indirizzo, IconaSx, IconaDx, icon, descr1));
				    break;
		case SORG_AUX:
				    elencoBanner.append(new sorgenteMultiAux(this, descrizione, (char *)indirizzo, IconaSx, IconaDx, icon, descr1));
				    break;
		default:
			qDebug("********** sottoMenu::addItem(): unknown item type!!! ************\n");
			return 0;
	}
	connectLastBanner();

	elencoBanner.getLast()->SetTextU( qdescrizione );
	elencoBanner.getLast()->setAnimationParams(periodo,numFrame);
	elencoBanner.getLast()->setBGColor(backgroundColor());
	elencoBanner.getLast()->setFGColor(foregroundColor());
	elencoBanner.getLast()->setId(tipo);
	for (int idx=elencoBanner.count()-2;idx>=0;idx--)
	{
		if (elencoBanner.at(idx)->getId()==tipo)
		{
			elencoBanner.getLast()->setSerNum(elencoBanner.at(idx)->getSerNum()+1);
			idx=-1;
		}
	}

	//     draw();
	return(1);
}

void sottoMenu::appendBanner(banner *b)
{
	elencoBanner.append(b);
	connectLastBanner();
}

void sottoMenu::connectLastBanner()
{
	connect(this, SIGNAL(gestFrame(char*)), elencoBanner.getLast(), SLOT(gestFrame(char*)));
	connect(this, SIGNAL(parentChanged(QWidget *)), elencoBanner.getLast(), SLOT(grandadChanged(QWidget *)));
	connect(elencoBanner.getLast(), SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
	connect(elencoBanner.getLast(), SIGNAL(sendInit(char *)), this, SIGNAL(sendInit(char *)));
	connect(elencoBanner.getLast(), SIGNAL(sendFramew(char*)), this, SIGNAL(sendFramew(char*)));
	connect(elencoBanner.getLast(), SIGNAL(freeze(bool)), this, SIGNAL(freeze(bool)));
	connect(elencoBanner.getLast(), SIGNAL(svegl(bool)), this, SIGNAL(svegl(bool)));
	connect(this, SIGNAL(frez(bool)), elencoBanner.getLast(), SIGNAL(freezed(bool)));
	connect(elencoBanner.getLast(), SIGNAL(richStato(char*)), this, SIGNAL(richStato(char*)));
	connect(elencoBanner.getLast(), SIGNAL(killMe(banner*)), this, SLOT(killBanner(banner*)));
}

void sottoMenu::addItem(banner *b)
{
	elencoBanner.append(b);
	connectLastBanner();

	connect(this, SIGNAL(hideChildren()), elencoBanner.getLast(), SLOT(hide()));
	elencoBanner.getLast()->SetTextU(elencoBanner.getLast()->name()); // name() torna il nome passato alla classe QWidget. non verra' tradotto...
	int periodo, numFrame;
	elencoBanner.getLast()->getAnimationParams(periodo, numFrame);
	elencoBanner.getLast()->setAnimationParams(periodo,numFrame);
	elencoBanner.getLast()->setBGColor(backgroundColor());
	elencoBanner.getLast()->setFGColor(foregroundColor());
}

void sottoMenu::showItem(int id)
{
	indice = id;
	forceDraw();
}

void sottoMenu::draw()
{
	uint idx,idy;
	qDebug("sottoMenu::draw() (%s)", name());
	if (!(indicold==indice))
	{
		//qDebug("indicold=%d - indice=%d",indicold,indice);
		for (idy=0;idy<elencoBanner.count();idy++)
			elencoBanner.at(idy)->hide();
		if (hasNavBar)
		{
			if (banner *bann = elencoBanner.at(indice))
				bannNavigazione->setCustomButton(bann->customButton());
			for (idx=0;idx<numRighe;idx++)
			{
				if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>numRighe) ) 
				{   			
					int tmp = (indice+idx) %(elencoBanner.count());
					int x, y, h;
					x = 0;
					y = idx*(height-MAX_HEIGHT/NUM_RIGHE)/numRighe; 
					h = (height-MAX_HEIGHT/NUM_RIGHE)/numRighe; 
					qDebug("elencoBanner.at(%d)->setGeometry(%d, %d, %d, %d",
							tmp, x, y, width, h);
					elencoBanner.at(tmp)->setGeometry(0, y, width, h); 

					elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
					elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
				}
			}		
			qDebug("Invoking bannNavigazione->setGeometry(%d, %d, %d, %d)",
					0, height-MAX_HEIGHT/NUM_RIGHE,
					width, MAX_HEIGHT/NUM_RIGHE);
			bannNavigazione  ->setGeometry( 0 ,height-MAX_HEIGHT/NUM_RIGHE,width , MAX_HEIGHT/NUM_RIGHE);		

			bannNavigazione->Draw();
			bannNavigazione->show();	
		}
		else
		{
			for (idx=0;idx<numRighe;idx++)
			{
				if  ( (elencoBanner.at(indice+idx)) || (elencoBanner.count()>=numRighe) ) 
				{   
					elencoBanner.at( (indice+idx) %(elencoBanner.count()))->setGeometry(0,idx*QWidget::height()/numRighe,QWidget::width(),QWidget::height()/numRighe);
					elencoBanner.at( (indice+idx) %(elencoBanner.count()))->Draw();
					elencoBanner.at( (indice+idx) %(elencoBanner.count()))->show();
				}
			}
		}
		indicold=indice;
	}

}

void sottoMenu::forceDraw()
{
	indicold=indice+1;
	draw();
}

void sottoMenu::goUp()
{
	if (elencoBanner.count()>(numRighe))
	{
		indicold=indice;
		indice=(++indice)%(elencoBanner.count());
		draw();
	}
}

void sottoMenu::goDown()
{
	qDebug("sottoMenu::goDown(), numRighe = %d", numRighe);
	if (elencoBanner.count()>(numRighe))
	{
		indicold=indice;
		if (--indice<0)
			indice=elencoBanner.count()-1;
		qDebug("indice = %d\n", indice);
		draw();
	}
}
void sottoMenu::setNumRighe(uchar n)
{
	numRighe=n;
}

banner* sottoMenu::getLast()
{
	return(elencoBanner.last());
}

banner* sottoMenu::getCurrent()
{
	return(elencoBanner.at(indice));
}

banner* sottoMenu::getNext()
{
	if (indice==((int)elencoBanner.count()-1))
		return(elencoBanner.at(0));
	return(elencoBanner.at(indice+1));
}

banner* sottoMenu::getPrevious()
{
	if (indice==0)
		return(elencoBanner.at(elencoBanner.count()-1));
	return(elencoBanner.at(indice-1));
}

void sottoMenu::inizializza()
{
	qDebug("sottoMenu::inizializza()");
	iniTim = new QTimer(this,"iniTimer");
	iniTim->start(300,TRUE);
	QObject::connect(iniTim,SIGNAL(timeout()), this,SLOT(init()));

}

void sottoMenu::init()
{
	qDebug("sottoMenu::init()");
	for (char idx=0;idx<elencoBanner.count();idx++)
		elencoBanner.at (idx) -> inizializza();
}

void sottoMenu::init_dimmer()
{
	qDebug("sottoMenu::init_dimmer()");
	for (char idx=0;idx<elencoBanner.count();idx++)
	{
		switch(elencoBanner.at(idx)->getId())
		{
			case DIMMER:
			case DIMMER_100:
			case ATTUAT_AUTOM:
			case ATTUAT_AUTOM_TEMP:
			case ATTUAT_AUTOM_TEMP_NUOVO_N:
			case ATTUAT_AUTOM_TEMP_NUOVO_F:
				elencoBanner.at (idx) -> inizializza(true);
				break;
			default:
				break;
		}
	}
}

bool sottoMenu::setPul(char* chi, char* where)
{
	unsigned char p=0;

	for (char idx=0;idx<elencoBanner.count();idx++)
	{
		if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
		{
			elencoBanner.at(idx)->setPul();
			p=1;
		}
	}
	if (p)
		return TRUE;
	return FALSE;
}

bool sottoMenu::setGroup(char* chi, char* where, bool* group)
{
	unsigned char p=0;

	for (char idx=0;idx<elencoBanner.count();idx++)
	{	
		if ( (!strcmp(elencoBanner.at(idx)->getChi(),chi)) && (!strcmp(elencoBanner.at(idx)->getAddress(),where)) )
		{
			elencoBanner.at(idx)->setGroup(group);
			p=1;
		}
	}
	if (p)
		return TRUE;
	return FALSE;
}

void sottoMenu::setIndex(char* indiriz)
{
	for (unsigned int idx=0;idx<elencoBanner.count();idx++)
	{
		if (!strcmp(elencoBanner.at(idx)->getAddress(),indiriz))
		{
			qDebug("setindex trovato %s",indiriz);
			elencoBanner.at(idx)->mostra(banner::BUT2);
			indice=idx;
		}
		else
			elencoBanner.at(idx)->nascondi(banner::BUT2);
	}
}


void sottoMenu::mostra_all(char but)
{
	for (unsigned int idx=0;idx<elencoBanner.count();idx++)
	{
		elencoBanner.at(idx)->mostra(but);
	}
}

void sottoMenu::setNumRig(uchar n)
{
	numRighe=n;
}
void sottoMenu::setHeight(int h)
{
	height=h;
}
uchar sottoMenu::getNumRig()
{
	return(numRighe);
}
int sottoMenu::getHeight()
{
	return(height);
}

void sottoMenu::mouseReleaseEvent ( QMouseEvent *  e)	
{
	qDebug("Released");
	if (freez)
	{
		freez=FALSE;
		emit(freeze(freez));
	}
	QWidget::mouseReleaseEvent ( e );
}

void sottoMenu::freezed(bool f)
{
	freez=f;

	qDebug("%s freezed %d",name(),freez);

	if (freez)
	{	   
		for(uchar idx=0;idx<elencoBanner.count();idx++)
			elencoBanner.at(idx)->setEnabled(FALSE);
		if(hasNavBar)
			bannNavigazione->setEnabled(FALSE);
	}
	else
	{
		for(uchar idx=0;idx<elencoBanner.count();idx++)
			elencoBanner.at(idx)->setEnabled(TRUE);
		if(hasNavBar)
			bannNavigazione->setEnabled(TRUE);
	}
	emit (frez(f));
}

void sottoMenu::setGeometry(int x, int y, int w, int h)
{
	//purtroppo in QTE se da un figlio faccio height() o width() mi da le dimensioni del padre...
	qDebug("sottoMenu::setGeometry(%d, %d, %d, %d)", x, y, w, h);
	height=h; 
	width=w;
	QWidget::setGeometry(x, y, w, h);

}

void  sottoMenu::killBanner(banner* b)
{
	int icx=elencoBanner.findRef( b );
	//    int icx=elencoBanner.find( b );

	if ( icx  != -1 )
	{
		elencoBanner.at(icx)->hide();
		elencoBanner.remove(icx);
		//qDebug("ti scrivo icx %d", icx);    
		indice=0;
		indicold=100;
		//	qDebug("bannerCount= %d", elencoBanner.count());
		draw();
		if ((elencoBanner.count()==0)&&(parentWidget()) )
		{
			emit(Closed());//hide();
			parentWidget()->showFullScreen();
		}
	}
}

void sottoMenu::hide(bool index)
{
	qDebug("[TERMO] sottoMenu::hide() (%s)", name());
	QWidget::hide();
	emit(hideChildren());
	if(index)
	{
		indice=0;
		forceDraw();
	}
}

void sottoMenu::svuota()
{
	elencoBanner.clear();
	draw();
}

uint sottoMenu::getCount()
{
	return(elencoBanner.count());
}



void  sottoMenu::setIndice(char c)
{
	if (c<=elencoBanner.count())
		indice=c;
}

void sottoMenu::show()
{
	qDebug("sottoMenu::show() (%s)", name());

	if(strcmp(name(), "ILLUMINO")==0)
		init_dimmer();
	forceDraw();
	QWidget::show();
}

void sottoMenu::reparent ( QWidget * parent, WFlags f, const QPoint & p, 
		bool showIt)
{
	qDebug("sottoMenu::reparent()");
	emit(parentChanged(parent));
	//disconnect(QObject::parent(), SIGNAL(sendFrame(char *)), 0, 0);
	QWidget::reparent(parent, f, p, showIt);
}


void sottoMenu::addAmb(char *a)
{
	qDebug("sottoMenu::addAmb(%s)", a);
	for (int idx=elencoBanner.count()-1;idx>=0;idx--)
		elencoBanner.at(idx)->addAmb(a);
}

void sottoMenu::initBanner(banner *bann, QDomNode conf)
{
	bann->setBGColor(paletteBackgroundColor());
	bann->setFGColor(paletteForegroundColor());

	QDomNode n = findNamedNode(conf, "descr");
	bann->SetTextU(n.toElement().text());

	n = findNamedNode(conf, "id");
	bann->setId(n.toElement().text().toInt());

	bann->setAnimationParams(0, 0);
	// note: we are ignoring the serial number...
}

QDomNode sottoMenu::findNamedNode(QDomNode root, QString name)
{
	QValueList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		if (item.isNull())
		{
			QDomNodeList list = n.childNodes();
			for (unsigned i = 0; i < list.length(); ++i)
				nodes.append(list.item(i));
			nodes.pop_front();
		}
		else
		{
			return item;
		}
	}
	QDomNode null;
	return null;
}

#ifdef IPHONE_MODE
	void sottoMenu::mouseMoveEvent( QMouseEvent *e )
	{
		static int x=0,y=0,Xpos=0,Ypos=0;
		static QTime lastEvent = QTime::currentTime(Qt::LocalTime);
		static QTime lastManEvent = QTime::currentTime(Qt::LocalTime);
		static int down = 0, gapT=100, gapY=10;

		QTime s = QTime::currentTime(Qt::LocalTime);

		printf("sottoMenu::mouseMoveEvent %d - %d\n",y,s.msec());
		if (s>lastEvent.addMSecs(200))
		{
			gapY=10;
			gapT=50;
			y=e->y();
		} 

		if ((abs(y-e->y())>gapY))
		{
			QTime s1 = lastManEvent.addMSecs(gapT);
			if (e->y()>y) 
			{ 
				if ( ((s>s1) && (!down)) || (e->y()-y>20) )
				{
					if (bannNavigazione) emit(goUP());
					lastManEvent = QTime::currentTime(Qt::LocalTime); 
					printf("up %d\n",y);
					if (e->y()-y>35)  emit(goUP());
					if (gapT >30) gapT=30;
					if (gapY > 5) gapY=5;
				}
				else if (down)
				{
					down = 0;
					gapT = 50;
					gapY = 10;
				}
			}
			else
			{
				if ( ((s>s1) && (down)) || (y-e->y()>20) )
				{
					if (bannNavigazione) emit(goDO());
					lastManEvent = QTime::currentTime(Qt::LocalTime); 
					printf("down %d\n",y);
					if (y-e->y()>35)  emit(goUP());
					if (gapT >30) gapT=30;
					if (gapY > 5) gapY=5;
				}
				else if (!down)
				{
					down = 1;
					gapT = 50;
					gapY = 10;
				}
			}
			x=e->x(); 
			y=e->y();
		}
		lastEvent = QTime::currentTime(Qt::LocalTime);     
	}
#endif

// Specialized submenus function definition
//
ProgramMenu::ProgramMenu(QWidget *parent, const char *name, QDomNode conf)
	: sottoMenu(parent, name)
{
	conf_root = conf;
}

void ProgramMenu::status_changed(QPtrList<device_status> list)
{
	bool update = false;
	for (QPtrListIterator<device_status> it(list); device_status *ds = it.current(); ++it)
	{
		if (ds->get_type() == device_status::THERMAL_REGULATOR_4Z || ds->get_type() == device_status::THERMAL_REGULATOR_99Z)
		{
			stat_var curr_season(stat_var::SEASON);
			ds->read(device_status_thermal_regulator::SEASON_INDEX, curr_season);
			switch (curr_season.get_val())
			{
				case thermal_regulator::SUMMER:
					if (season == thermal_regulator::WINTER)
					{
						season = thermal_regulator::SUMMER;
						createSummerBanners();
						update = true;
					}
					break;
				case thermal_regulator::WINTER:
					if (season == thermal_regulator::SUMMER)
					{
						season = thermal_regulator::WINTER;
						createWinterBanners();
						update = true;
					}
					break;

			}
		}
	}
	if (update)
	{
		setAllBGColor(paletteBackgroundColor());
		setAllFGColor(paletteForegroundColor());
		forceDraw();
	}
}

WeeklyMenu::WeeklyMenu(QWidget *parent, const char *name, QDomNode conf)
	: ProgramMenu(parent, name, conf)
{
	season = thermal_regulator::SUMMER;
	createSummerBanners();
}

void WeeklyMenu::createSummerBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_estivo.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu: wrong node in config file");
	}

	QDomNode summer = conf_root.namedItem("summer");
	if (!summer.isNull())
	{
		QDomNode programs = summer.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

void WeeklyMenu::createWinterBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("programma_invernale.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu: wrong node in config file");
	}

	QDomNode winter = conf_root.namedItem("winter");
	if (!winter.isNull())
	{
		QDomNode programs = winter.namedItem("prog");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

ScenarioMenu::ScenarioMenu(QWidget *parent, const char *name, QDomNode conf)
	: ProgramMenu(parent, name, conf)
{
	season = thermal_regulator::SUMMER;
	createSummerBanners();
}

void ScenarioMenu::createSummerBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("scenario_estivo.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] ScenarioMenu: wrong node in config file");
	}

	QDomNode summer = conf_root.namedItem("summer");
	if (!summer.isNull())
	{
		QDomNode programs = summer.namedItem("scen");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

void ScenarioMenu::createWinterBanners()
{
	elencoBanner.clear();
	const QString i_ok = QString("%1%2").arg(IMG_PATH).arg("btnok.png");
	const QString i_central = QString("%1%2").arg(IMG_PATH).arg("scenario_invernale.png");

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] ScenarioMenu: wrong node in config file");
	}

	QDomNode winter = conf_root.namedItem("winter");
	if (!winter.isNull())
	{
		QDomNode programs = winter.namedItem("scen");
		QDomNode p;
		if (!programs.isNull())
			p = programs.firstChild();
		while (!p.isNull())
		{
			BannWeekly *bp = new BannWeekly(this);
			bp->SetIcons(i_ok.ascii(), 0, i_central.ascii());
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
			// set Text taken from conf.xml
			if (p.isElement())
			{
				bp->SetTextU(p.toElement().text());
				QRegExp re("(\\d)");
				int index = re.search(p.nodeName());
				if (index != -1)
					bp->setProgram(re.cap(1).toInt());
			}
			elencoBanner.append(bp);
			p = p.nextSibling();
		}
	}
}

TimeEditMenu::TimeEditMenu(QWidget *parent, const char *name)
	: sottoMenu(parent, name, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	time_edit = new FSBannTime(this);
	elencoBanner.append(time_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void TimeEditMenu::performAction()
{
	emit(timeSelected(time()));
}

QTime TimeEditMenu::time()
{
	return time_edit->time();
}

DateEditMenu::DateEditMenu(QWidget *parent, const char *name)
	: sottoMenu(parent, name, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	date_edit = new FSBannDate(this);
	elencoBanner.append(date_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void DateEditMenu::performAction()
{
	emit(dateSelected(date()));
}
QDate DateEditMenu::date()
{
	return date_edit->date();
}
