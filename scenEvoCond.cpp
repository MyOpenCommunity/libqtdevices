//#include "btbutton.h"
#include <qfont.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qprocess.h>

#include <qfile.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "openclient.h"
#include "sottomenu.h"
#include "scenevocond.h"
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"

/*****************************************************************
** Advanced scenario management generic condition
****************************************************************/	

scenEvo_cond::scenEvo_cond(QWidget *parent, char *name)  : 
  QFrame(parent, name)
{
    val = -1;
    for(int i = 0; i < MAX_EVO_COND_IMG; i++)
	img[i] = new QString("");
}

const char *scenEvo_cond::getImg(int index)
{
    if(index >= MAX_EVO_COND_IMG) return "";
    return img[index]->ascii() ;
}

void scenEvo_cond::setImg(int index, const char *s)
{
    qDebug("scenEvo_cond: setting image %d to %s", index, s);
    if(index >= MAX_EVO_COND_IMG) return ;
    *img[index]  = s;
}

int scenEvo_cond::getVal(void)
{
    return val;
}

void scenEvo_cond::setVal(int v)
{
    val = v ;
}

const char *scenEvo_cond::getDescription(void)
{
  return "Generic scenEvo condition";
}

void scenEvo_cond::mostra()
{
    // Does nothing by default
    qDebug("scenEvo_cond::mostra()");
}

void scenEvo_cond::setBGColor(int r, int g, int b)
{
    setBGColor( QColor :: QColor(r,g,b)); 
}

void scenEvo_cond::setFGColor(int r, int g, int b)
{
    setFGColor( QColor :: QColor(r,g,b)); 
}

void scenEvo_cond::setBGColor(QColor c)
{	
    qDebug("scenEvo_cond::setBGColor(QColor c)");
}

void scenEvo_cond::setFGColor(QColor c)
{
    qDebug("scenEvo_cond::setFGColor(QColor c)");
}

void scenEvo_cond::SetIcons()
{
    // Does nothing by default
    qDebug("scenEvo_cond::SetIcons()");
}

void scenEvo_cond::Next()
{
    qDebug("scenEvo_cond::Next()");
    emit(SwitchToNext());
}

void scenEvo_cond::Prev()
{
    qDebug("scenEvo_cond::Prev()");
    //emit(SwitchToPrev());
    reset();
    emit(resetAll());
    emit(SwitchToFirst());
}

void scenEvo_cond::OK()
{
    qDebug("scenEvo_cond::OK()");
    save();
    emit(SwitchToFirst());
}

void scenEvo_cond::Apply()
{
    qDebug("scenEvo_cond::Apply()");
}

void scenEvo_cond::setEnabled(bool e)
{
    qDebug("scenEvo_cond::setEnabled(%d)", e);
}

void scenEvo_cond::save()
{
    qDebug("scenEvo_cond::save()");
}

void scenEvo_cond::reset()
{
    qDebug("scenEvo_cond::reset()");
}

int scenEvo_cond::get_serial_number()
{
    return serial_number;
}

void scenEvo_cond::set_serial_number(int n)
{
    serial_number = n;
}

void scenEvo_cond::inizializza(void)
{
}

void scenEvo_cond::handle_frame(char *s)
{
    qDebug("scenEvo_cond::handle_frame()");
    emit(frame_available(s));
}

bool scenEvo_cond::isTrue(void)
{
    return false;
}

/*****************************************************************
 ** Advanced scenario management, time condition
****************************************************************/	

scenEvo_cond_h::scenEvo_cond_h(QWidget *parent, char *name) :
  scenEvo_cond(parent, name)
{
    qDebug("***** scenEvo_cond_h::scenEvo_cond_h");
    h = new QString("");
    m = new QString("");
    s = new QString("");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif  
    ora=NULL;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(scaduta()));
    cond_time = new QDateTime(QDateTime::currentDateTime());
    ora = new timeScript(this, "condizione scen evo h", 2 , cond_time);
}

void scenEvo_cond_h::set_h(const char *_h)
{
    *h = _h;
    qDebug("scenEvo_cond_h::set_h : %s", h->ascii());
#if 0
    cond_time->setTime(QTime(h->toInt(), m->toInt(), s->toInt()));
    setupTimer();
#endif
}

void scenEvo_cond_h::set_m(const char *_m)
{
    *m = _m;
    qDebug("scenEvo_cond_h::set_m : %s", m->ascii());
    QTime t(h->toInt(), m->toInt(), 0);
    cond_time->setTime(t);
    ora->setDataOra(QDateTime(QDate::currentDate(), t));
    ora->showTime();
    setupTimer();
}

void scenEvo_cond_h::set_s(const char *_s)
{
    *s = _s;
    qDebug("scenEvo_cond_h::set_s : %s", s->ascii());
#if 0
    cond_time->setTime(QTime(h->toInt(), m->toInt(), s->toInt()));
    setupTimer();
#endif
}

const char *scenEvo_cond_h::getDescription(void)
{
  return "scenEvo hour condition";
}

void scenEvo_cond_h::SetIcons() 
{
    QPixmap* Icon1 = new QPixmap();
    QPixmap* Icon2 = NULL;
    char iconName[MAX_PATH];
    qDebug("scenEvo_cond_h::SetIcons()");
    for(int i=0; i<6; i++)
	qDebug("icon[%d] = %s", i, getImg(i));
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT); 
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    memset(iconName, '\000', sizeof(iconName));
    strcpy(iconName, ICON_FRECCIA_SU);
    // Pulsanti su
    Icon1->load(iconName);
    getPressName((char*)ICON_FRECCIA_SU, iconName,sizeof(iconName));
    if (QFile::exists(iconName)) {  	 
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    for (uchar idx=0;idx<2;idx++) {
	but[idx] = new BtButton(this, "freccia"/*+QString::number(idx)*/);
	but[idx] -> setGeometry(idx*80+50, 80, 60, 60);
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2) {
	    but[idx] -> setPressedPixmap(*Icon2);
	}
    }
    delete (Icon1);
    if(Icon2)
	delete (Icon2);
    // Pulsanti giu`
    Icon2=NULL;
    Icon1 = new QPixmap();
    Icon1->load(ICON_FRECCIA_GIU);	
    getPressName((char*)ICON_FRECCIA_GIU, &iconName[0],sizeof(iconName));
    if (QFile::exists(iconName)) {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    for (uchar idx=2;idx<4;idx++) {
	but[idx] = new BtButton(this,"freccia"+QString::number(idx));
	but[idx] -> setGeometry((idx-2)*80+50,190,60,60);	
	but[idx] -> setAutoRepeat(true);
	but[idx] -> setPixmap(*Icon1);
	if (Icon2)
	    but[idx] -> setPressedPixmap(*Icon2);
    }	
    delete(Icon1);
    if(Icon2)
	delete Icon2;
    // Orologio
    Icon1 = new QPixmap();
    Icon2 = NULL;
    Icon1->load(getImg(0));    
    Immagine = new BtLabel(this, "orologio");
    if (Icon1)
	Immagine -> setPixmap(*Icon1); 
    Immagine->setGeometry(90,0,60,60);
    delete(Icon1);
    // Pulsante in basso a sinistra, area 6 (SE C'E` L'ICONA)
    if(getImg(A6_ICON_INDEX)[0]) {
	Icon1 = new QPixmap();
	Icon1->load(getImg(A6_ICON_INDEX));
	qDebug("Area 6: loaded icon %s", getImg(A6_ICON_INDEX));
	getPressName((char *)getImg(A6_ICON_INDEX), iconName,sizeof(iconName));
	if (QFile::exists(iconName)) {  
	    Icon2 = new QPixmap();
	    Icon2->load(iconName);
	}
	but[A6_BUTTON_INDEX] = new BtButton(this, "ok");
	but[A6_BUTTON_INDEX]->setGeometry(0, MAX_HEIGHT - 60, 60, 60);
	but[A6_BUTTON_INDEX]->setPixmap(*Icon1);
	if (Icon2)
	    but[A6_BUTTON_INDEX]->setPressedPixmap(*Icon2);
	delete Icon1;
	if(Icon2)
	    delete Icon2;
    } else
	but[A6_BUTTON_INDEX] = NULL;
    // Pulsante in basso al centro, area 7
    if(getImg(A7_ICON_INDEX)[0]) {
	Icon1 = new QPixmap();
	Icon2 = NULL;
	Icon1->load(getImg(A7_ICON_INDEX));
	qDebug("Area 7: loaded icon %s", getImg(A7_ICON_INDEX));
	getPressName((char *)getImg(A7_ICON_INDEX),iconName, sizeof(iconName));
	if (QFile::exists(iconName)) {  
	    Icon2 = new QPixmap();
	    Icon2->load(iconName);
	} 
	but[A7_BUTTON_INDEX] = new BtButton(this, "np1");
	but[A7_BUTTON_INDEX]->setGeometry(MAX_WIDTH/2 - 30, MAX_HEIGHT - 60, 
					  60, 60);
	but[A7_BUTTON_INDEX]->setPixmap(*Icon1);
	if (Icon2)
	    but[A7_BUTTON_INDEX]->setPressedPixmap(*Icon2);
	delete Icon1;
	if(Icon2)
	    delete Icon2;
    } else
	but[A7_BUTTON_INDEX] = NULL;
    // Pulsante in basso a destra, area 8
    if(getImg(A8_ICON_INDEX)[0]) {
	Icon1 = new QPixmap();
	Icon2 = NULL;
	Icon1->load(getImg(A8_ICON_INDEX));
	qDebug("Area 8: loaded icon %s", getImg(A8_ICON_INDEX));
	getPressName((char *)getImg(A8_ICON_INDEX),iconName, sizeof(iconName));
	if (QFile::exists(iconName)) {  
	    Icon2 = new QPixmap();
	    Icon2->load(iconName);
	}
	but[A8_BUTTON_INDEX] = new BtButton(this, "np2");
	but[A8_BUTTON_INDEX]->setGeometry(MAX_WIDTH - 60, MAX_HEIGHT - 60, 60, 60);
	but[A8_BUTTON_INDEX]->setPixmap(*Icon1);
	if (Icon2)
	    but[A8_BUTTON_INDEX]->setPressedPixmap(*Icon2);
	delete Icon1;
	if(Icon2)
	    delete Icon2;
    } else
	but[A8_BUTTON_INDEX] = NULL;
#if 0
    cond_time = new QDateTime(QDateTime::currentDateTime());
    cond_time->setTime(QTime(h->toInt(), m->toInt(), s->toInt()));
    ora = new timeScript(this, "condizione scen evo h", 2 , cond_time);
#endif
    ora->setGeometry(40, 140, 160, 50);
    ora->setFrameStyle( QFrame::Plain );
    ora->setLineWidth(0);    
    qDebug("scenEvo_cond_h::SetIcons(), fine");
}

void scenEvo_cond_h::mostra()
{
    qDebug("scenEvo_cond_h::mostra()");
    for (uchar idx=0; idx < 8; idx++)
	if(but[idx])
	    but[idx]->show();   
    ora->show();
    Immagine->show();
    
    //bannNavigazione->show();
    show();     
    disconnect( but[0] ,SIGNAL(clicked()), ora, SLOT(aumOra()));
    disconnect( but[1] ,SIGNAL(clicked()), ora, SLOT(aumMin()));
    disconnect( but[2] ,SIGNAL(clicked()), ora, SLOT(diminOra()));
    disconnect( but[3] ,SIGNAL(clicked()), ora, SLOT(diminMin()));
    connect( but[0] , SIGNAL(clicked()), ora, SLOT(aumOra()));
    connect( but[1] , SIGNAL(clicked()), ora, SLOT(aumMin()));
    connect( but[2] ,SIGNAL(clicked()), ora, SLOT(diminOra()));
    connect( but[3] ,SIGNAL(clicked()), ora, SLOT(diminMin()));
    //connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(okTime()));
    //disconnect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(okTipo()));
    //disconnect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(Closed()));
    //disconnect(bannNavigazione, SIGNAL(backClick()), this, SLOT(Closed()));
    //connect(bannNavigazione, SIGNAL(backClick()), this, SLOT(Closed()));
    if(but[A6_BUTTON_INDEX]) {
        disconnect( but[A6_BUTTON_INDEX], SIGNAL(released()), this, SLOT(OK()));
	connect( but[A6_BUTTON_INDEX], SIGNAL(released()), this, SLOT(OK()));
    }
    if(getImg(3)[0] == 0) {
	// cimg4 is empty
	if(but[A8_BUTTON_INDEX]) {
	    qDebug("connecting A8 to Prev");
	    disconnect( but[A8_BUTTON_INDEX], SIGNAL(released()), 
			this, SLOT(Prev()));
	    connect( but[A8_BUTTON_INDEX], SIGNAL(released()), 
		     this, SLOT(Prev()));
	}
    } else {
	if(but[A7_BUTTON_INDEX]) {
	    qDebug("connecting A7 to Prev");
	    disconnect( but[A7_BUTTON_INDEX], SIGNAL(released()), 
		     this, SLOT(Prev()));
	    connect( but[A7_BUTTON_INDEX], SIGNAL(released()), 
		     this, SLOT(Prev()));
	}
	if(but[A8_BUTTON_INDEX]) {
	    qDebug("connecting A8 to Next");
	    disconnect( but[A8_BUTTON_INDEX], SIGNAL(released()), 
			this, SLOT(Next()));
	    connect( but[A8_BUTTON_INDEX], SIGNAL(released()), 
		     this, SLOT(Next()));
	}
    }
}

void scenEvo_cond_h::setBGColor(QColor c)
{	
    setPaletteBackgroundColor(c);    
    if (ora)
	ora->setPaletteBackgroundColor(c);
    for (uchar idx=0;idx<7;idx++) {
	    if (but[idx])
		but[idx]->setPaletteBackgroundColor(c);
    }
    //if ( bannNavigazione)
    //bannNavigazione->setBGColor(c);
}

void scenEvo_cond_h::setFGColor(QColor c)
{
    setPaletteForegroundColor(c);
    if (ora)
        ora->setPaletteForegroundColor(c);
    for (uchar idx=0;idx<7;idx++) {
	if (but[idx])   
	    but[idx]->setPaletteForegroundColor(c);
    }
    //if ( bannNavigazione)
    //bannNavigazione->setFGColor(c);
}

void scenEvo_cond_h::setupTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    int secsto = now.secsTo(*cond_time);
    while(secsto <= 0)
      // Do it tomorrow 
      secsto += 24 * 60 * 60;
    // According to QT doc, if timer is running, it is stopped and restarted
    // with new interval. Otherwise it is just started.
    qDebug("(re)starting timer with interval = %d", secsto * 1000);
#if 0
    timer->changeInterval(secsto * 1000);
#else
    timer->stop();
    timer->start(secsto * 1000, true);
#endif
}

void scenEvo_cond_h::Apply()
{
    *cond_time = ora->getDataOra();
    setupTimer();
}

void scenEvo_cond_h::OK()
{
    qDebug("scenEvo_cond_h::OK()");
    Apply();
    scenEvo_cond::OK();
}

void scenEvo_cond_h::scaduta()
{
    qDebug("scenEvo_cond_h::scaduta()");
    emit(verificata());
    setupTimer();
}

void scenEvo_cond_h::setEnabled(bool e)
{
    qDebug("scenEvo_cond_h::setEnabled(%d)", e);
    for(int i=0; i<8; i++)
	if(but[i])
	    but[i]->setEnabled(e);
}

void scenEvo_cond_h::save()
{
    qDebug("scenEvo_cond_h::save()");
    copyFile("cfg/conf.xml","cfg/conf1.lmx");
    setCfgValue("cfg/conf1.lmx", SCENARIO_EVOLUTO, "hour", 
		cond_time->time().toString("hh").ascii(), get_serial_number());
    setCfgValue("cfg/conf1.lmx", SCENARIO_EVOLUTO, "minute", 
		cond_time->time().toString("mm").ascii(), get_serial_number());
    QDir::current().rename("cfg/conf1.lmx","cfg/conf.xml",FALSE);
}

void scenEvo_cond_h::reset()
{
    qDebug("scenEvo_cond_h::reset()");
    //cond_time->setTime(QTime(h->toInt(), m->toInt(), s->toInt()));
    ora->setDataOra(*cond_time);
    ora->showTime();
}

bool scenEvo_cond_h::isTrue(void)
{
    return 
	((cond_time->time().hour() == 
	  QDateTime::currentDateTime().time().hour()) &&
	 (cond_time->time().minute() == 
	  QDateTime::currentDateTime().time().minute()));
}

/*****************************************************************
 ** Advanced scenario management, device condition
****************************************************************/

scenEvo_cond_d::scenEvo_cond_d(QWidget *parent, char *name) :
  scenEvo_cond(parent, name)
{
    qDebug("scenEvo_cond_d::scenEvo_cond_d()");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif  
    descr = new QString("");
    where = new QString("");
    trigger = new QString("");
    memset(but, 0, sizeof(but));
    qDebug("scenEvo_cond_d::scenEvo_cond_d(), end");
}

void scenEvo_cond_d::set_descr(const char *d)
{
    qDebug("scenEvo_cond_d::set_descr(%s)", d);
    *descr = d;
}

void scenEvo_cond_d::set_where(const char *w)
{
    qDebug("scenEvo_cond_d::set_where(%s)", w);
    *where = w;
}

void scenEvo_cond_d::get_where(QString& out)
{
    out = *where;
}

void scenEvo_cond_d::set_trigger(const char *t)
{
    qDebug("scenEvo_cond_d::set_trigger(%s)", t);
    *trigger = t;
}

const char *scenEvo_cond_d::getDescription(void)
{
  return "scenEvo device condition";
}

void scenEvo_cond_d::mostra()
{
    qDebug("scenEvo_cond_d::mostra()");
    char tmp[100];
    for (uchar idx=0; idx < 8; idx++)
	if(but[idx])
	    but[idx]->show();   
    area1_ptr->show();
    sprintf(tmp, "%s", descr->ascii());
    area2_ptr->setText(tmp);
    area2_ptr->show();
    if(actual_condition)
	actual_condition->show();
    show();
}

void scenEvo_cond_d::setBGColor(QColor c)
{	
    qDebug("scenEvo_cond_d::setBGColor()");
    setPaletteBackgroundColor(c);    
    for (uchar idx=0;idx<7;idx++) {
	if (but[idx])
	    but[idx]->setPaletteBackgroundColor(c);
    }
    area1_ptr->setPaletteBackgroundColor(c);
    area2_ptr->setPaletteBackgroundColor(c);
    if(actual_condition)
	actual_condition->setBGColor(c);
}

void scenEvo_cond_d::setFGColor(QColor c)
{
    qDebug("scenEvo_cond_d::setFGColor()");
    setPaletteForegroundColor(c);
    for (uchar idx=0;idx<7;idx++) {
	if (but[idx])   
	    but[idx]->setPaletteForegroundColor(c);
    }
    area1_ptr->setPaletteForegroundColor(c);
    area2_ptr->setPaletteForegroundColor(c);
    if(actual_condition)
	actual_condition->setFGColor(c);
}

void scenEvo_cond_d::setEnabled(bool e)
{
    qDebug("scenEvo_cond_d::setEnabled(%d)", e);
    for(int i=0; i<7; i++)
	if(but[i])
	    but[i]->setEnabled(e);
}


void scenEvo_cond_d::SetButtonIcon(int icon_index, int button_index)
{
    QPixmap* Icon1;
    QPixmap* Icon2;
    char iconName[MAX_PATH];
    if(!getImg(icon_index)[0]) {
	but[button_index] = NULL;
	return;
    }
    Icon1 = new QPixmap();
    getPressName((char *)getImg(icon_index), iconName, sizeof(iconName));
    if(QFile::exists(getImg(icon_index))) {
	Icon1->load(getImg(icon_index));
	if(but[button_index])
	    but[button_index]->setPixmap(*Icon1);
    }
    if(QFile::exists(iconName)) {
	Icon2 = new QPixmap();
	Icon2->load(iconName);
	if(but[button_index])
	    but[button_index]->setPressedPixmap(*Icon2);
    }
    delete Icon1;
    if(Icon2) 
	delete Icon2;
}

void scenEvo_cond_d::SetIcons() 
{
    qDebug("scenEvo_cond_d::SetIcons()");
    QPixmap* Icon1 = new QPixmap();
    QPixmap* Icon2 = NULL;
    char iconName[MAX_PATH];
    for(int i=0; i<6; i++)
	qDebug("icon[%d] = %s", i, getImg(i));
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT); 
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    area1_ptr = new BtLabel(this, "Area1");
    area1_ptr->setGeometry(0, 0, BUTTON_DIM, BUTTON_DIM);
    area2_ptr = new BtLabel(this, "Area2");
    area2_ptr->setGeometry(BUTTON_DIM, BUTTON_DIM/2 - TEXT_Y_DIM/2, 
			   TEXT_X_DIM, TEXT_Y_DIM);
    area2_ptr->setFont( QFont( "helvetica", 20, QFont::Bold ) );
    area2_ptr->setAlignment(AlignHCenter|AlignVCenter);
    BtButton *b = new BtButton(this, "Up button");
    but[A3_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 80, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Up()));
    b = new BtButton(this, "Down button");
    but[A4_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 190, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Down()));
    b = new BtButton(this, "OK button");
    but[A5_BUTTON_INDEX] = b;
    b->setGeometry(0, MAX_HEIGHT - BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(OK()));
    b = new BtButton(this, "Prev button");
    but[A6_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH - BUTTON_DIM, MAX_HEIGHT - BUTTON_DIM, 
		   BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Prev()));
    // area #1
    if (QFile::exists(getImg(A1_ICON_INDEX)))
      Icon1->load(getImg(A1_ICON_INDEX));
    area1_ptr->setPixmap(*Icon1);
    delete Icon1;
    // area #3
    SetButtonIcon(A3_ICON_INDEX, A3_BUTTON_INDEX);
    // area #4
    SetButtonIcon(A4_ICON_INDEX, A4_BUTTON_INDEX);
    // area #5
    SetButtonIcon(A5_ICON_INDEX, A5_BUTTON_INDEX);
    // area #8
    SetButtonIcon(A6_ICON_INDEX, A6_BUTTON_INDEX);
    // Create actual device condition
    device_condition *dc ;
    qDebug("#### Condition type = %d", getVal());
    switch(getVal()) {
    case 1:
	dc = new device_condition_light_status(this, "light_status", trigger);
	break;
    case 2:
	dc = new device_condition_dimming(this, "dimming_val", trigger);
	break;
    case 3:
	dc = new device_condition_temp(this, "temp_val", trigger);
        but[A3_BUTTON_INDEX]->setAutoRepeat(true);
        but[A4_BUTTON_INDEX]->setAutoRepeat(true);
	break;
    case 4:
	dc = new device_condition_volume(this, "audio_val", trigger);
	break;
    case 6:
	dc = new device_condition_dimming_100(this, "dimming_100_val", trigger);
	break;
    default:
	qDebug("Unknown device condition");
	dc = NULL;
    }
    if(dc) {
	dc->setGeometry(40,140,160,50);
	connect(dc, SIGNAL(richStato(char *)), this, 
		SIGNAL(richStato(char *)));
#if 0
	// Non mandiamo + segnali quando la condizione sul device e` verificata
	// E` solo la condizione oraria che manda un segnale. Questa 
	// condizione dice solo se e` verificata o meno con isTrue()
	connect(dc, SIGNAL(verificata()), this, SIGNAL(verificata()));
#endif
	connect(this, SIGNAL(frame_available(char *)),
		dc, SLOT(handle_frame(char *)));
	dc->set_where(*where);
    }
    actual_condition = dc;
    qDebug("scenEvo_cond_d::SetIcons(), end");
}

void scenEvo_cond_d::Up(void)
{
    qDebug("scenEvo_cond_d::Up()");
    actual_condition->Up();
}

void scenEvo_cond_d::Down(void)
{
    qDebug("scenEvo_cond_d::Down()");
    actual_condition->Down();
}

void scenEvo_cond_d::Apply(void)
{
    actual_condition->OK();
}

void scenEvo_cond_d::OK(void)
{
    qDebug("scenEvo_cond_d::OK()");
    // Save ALL conditions here (not just this one)
    //Apply();
    //scenEvo_cond::OK();
    emit(okAll());
    emit(SwitchToFirst());
}

void scenEvo_cond_d::save()
{
    qDebug("scenEvo_cond_d::save()");
    QString s; actual_condition->get_condition_value(s);
    copyFile("cfg/conf.xml","cfg/conf1.lmx");
    setCfgValue("cfg/conf1.lmx", SCENARIO_EVOLUTO, "trigger", 
		s.ascii(), get_serial_number());
    QDir::current().rename("cfg/conf1.lmx","cfg/conf.xml",FALSE);
    reset();
    inizializza();
}

void scenEvo_cond_d::reset()
{
    qDebug("scenEvo_cond_d::reset()");
    actual_condition->reset();
}

void scenEvo_cond_d::inizializza(void)
{
    if(actual_condition)
	actual_condition->inizializza();
}

bool scenEvo_cond_d::isTrue(void)
{
    return actual_condition ? actual_condition->isTrue() : false ;
}

/*****************************************************************
 ** Actual generic device condition
****************************************************************/
device_condition::device_condition(QWidget *p, QString *s)
{
    qDebug("device_condtion::device_condition(%s)", s->ascii());
    parent = p;
    satisfied = false;
}

int device_condition::get_min()
{
    return 0;
}

int device_condition::get_max()
{
    return 0;
}

int device_condition::get_step()
{
    return 1;
}

int device_condition::get_divisor()
{
    return 1;
}

bool device_condition::show_OFF_on_zero()
{
    return false;
}

int device_condition::get_condition_value(void)
{
    return cond_value;
}

int device_condition::set_condition_value(int v)
{
    if(v > get_max())
	v = get_max();
    if(v < get_min())
	v = get_min();
    cond_value = v;
}

int device_condition::set_condition_value(QString s)
{
    qDebug("device_condition::set_condition_value (%s)", s.ascii());
    set_condition_value(s.toInt());
}

void device_condition::get_condition_value(QString& out)
{
    qDebug("device_condition::get_condition_value(QString&)");
    char tmp[100];
    sprintf(tmp, "%d", get_condition_value());
    out = tmp;
}

void device_condition::show()
{
    qDebug("device_condition::show()");
    frame->show();
}

void device_condition::Draw()
{
    char tmp[50];
    QString u;
    get_unit(u);
    int v = get_current_value();
    if(show_OFF_on_zero() && !v)
	sprintf(tmp, "OFF");
    else
	sprintf(tmp, "%d%s", v, u.ascii());
    ((QLabel *)frame)->setText(tmp);
}

void device_condition::setGeometry(int x, int y, int sx, int sy)
{
    frame->setGeometry(x, y, sx, sy);
}

void device_condition::Up()
{
    qDebug("device_condition::Up()");
    int val = get_current_value();
    val += get_step();
    qDebug("val = %d", val);
    set_current_value(val);
    Draw();
    show();
}

void device_condition::Down()
{
    qDebug("device_condition::Down()");
    int val = get_current_value();
    val -= get_step();
    set_current_value(val);
    Draw();
    show();
}

void device_condition::OK()
{
    qDebug("device_condition::OK()");
    set_condition_value(get_current_value());
}

int device_condition::get_current_value()
{
    return current_value;
}

int device_condition::set_current_value(int v)
{
    if(v > get_max())
	v = get_max();
    if(v < get_min())
	v = get_min();
    current_value = v;
    return current_value;
}

scenEvo_cond_d *device_condition::get_parent()
{
    return (scenEvo_cond_d *)parent;
}

void device_condition::get_unit(QString& out)
{
    out = QString("");
}

void device_condition::setFGColor(QColor c)
{
    qDebug("device_condition::setFGColor (%d, %d, %d)", c.red(), c.green(),
	   c.blue());
    frame->setPaletteForegroundColor(c);
}

void device_condition::setBGColor(QColor c)
{
    qDebug("device_condition::setBGColor", c.red(), c.green(),
	   c.blue());
    frame->setPaletteBackgroundColor(c);
}

void device_condition::inizializza()
{
    qDebug("device_condition::inizializza()");
    dev->init();
}

void device_condition::reset()
{
    qDebug("device_condition::reset()");
    set_current_value(get_condition_value());
    Draw();
}

bool device_condition::isTrue()
{
    return satisfied;
}

void device_condition::set_where(QString s)
{
    qDebug("device_condition::set_where(%s)\n", s.ascii());
    dev->set_where(s);
    // Aggiunge il nodo alla cache
    dev = btouch_device_cache.add_device(dev) ;
#if 0
    // Pass frames on to device for analysis
    connect(this, SIGNAL(frame_available(char *)), 
	    dev, SLOT(frame_rx_handler(char *)));
#endif
    // Get status changed events back
    connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
	    this, SLOT(status_changed(QPtrList<device_status>)));
}

void device_condition::set_pul(bool p)
{
    dev->set_pul(p);
}

void device_condition::set_group(int g)
{
    dev->set_group(g);
}

void device_condition::status_changed(QPtrList<device_status>)
{
    qDebug("device_condition::status_changed()");
}

void device_condition::handle_frame(char *s)
{
    qDebug("device_condition::handle_frame()");
    emit(frame_available(s));
}

/*****************************************************************
 ** Actual light status device condition
****************************************************************/
device_condition_light_status::
device_condition_light_status(QWidget *parent, char *name, QString *c) : 
    device_condition(parent, c)
{
    QLabel *l = new QLabel(parent, name);
    l->setAlignment(AlignHCenter|AlignVCenter);
    l->setFont( QFont( "helvetica", 20, QFont::Bold ) );
    frame = l;
    set_condition_value(*c);
    set_current_value(device_condition::get_condition_value());
    dev = new light(QString(""));
    Draw();
}

void device_condition_light_status::get_string(QString& out)
{
    out = get_current_value() ? "ON" : "OFF" ;
}

void device_condition_light_status::Draw()
{
    QString s; get_string(s);
    ((QLabel *)frame)->setText(s.ascii());
}

void device_condition_light_status::status_changed(QPtrList<device_status> sl)
{
    int trig_v = device_condition::get_condition_value();
    stat_var curr_status(stat_var::ON_OFF);
    qDebug("device_condition_light_status::status_changed()");
     QPtrListIterator<device_status> *dsi = 
      new QPtrListIterator<device_status>(sl);
    dsi->toFirst();
    device_status *ds;
    while( ( ds = dsi->current() ) != 0) {
	switch (ds->get_type()) {
	case device_status::LIGHTS:
	    ds->read(device_status_light::ON_OFF_INDEX, curr_status);
	    qDebug("Light status variation");
	    qDebug("trigger value = %d, current value = %d\n", trig_v, 
		   curr_status.get_val());
	    if(trig_v == curr_status.get_val()) {
		qDebug("light condition (%d) satisfied", trig_v);
		satisfied = true;
	    } else {
		qDebug("light condition (%d) NOT satisfied", trig_v);
		satisfied = false;
	    }
	    break;
	case device_status::DIMMER:
	    qDebug("dimmer status variation, ignored");
	    break;
	case device_status::DIMMER100:
	    qDebug("new dimmer status variation, ignored");
	    break;
	case device_status::NEWTIMED:
	    qDebug("new timed device status variation, ignored");
	    break;
	default:
	    qDebug("device status of unknown type (%d)", ds->get_type());
	    break;
	}
	++(*dsi);
    }
    delete dsi;
}

int device_condition_light_status::get_max()
{
    return 1;
}

int device_condition_light_status::set_condition_value(QString s)
{
    qDebug("device_condition_light_status::set_condition_value");
    int v = 0;
    if(s == "1")
	v = 1;
    else if(s == "0")
	v = 0;
    else 
	qDebug("Unknown condition value %s for device_condition_light_status");
    return device_condition::set_condition_value(v);
}

void device_condition_light_status::get_condition_value(QString& out)
{
    out = device_condition::get_condition_value() ? "1" : "0" ;
}


/*****************************************************************
 ** Actual dimming value device condition
****************************************************************/
device_condition_dimming::device_condition_dimming(QWidget *parent, 
						   char *name, QString *c) :
    device_condition(parent, c)
{
    qDebug("device_condition_dimming::device_condition_dimming(%s)",
	   c->ascii());
    QLabel *l = new QLabel(parent, name);
    l->setAlignment(AlignHCenter|AlignVCenter);
    l->setFont( QFont( "helvetica", 20, QFont::Bold ) );
    frame = l;
    if(strcmp(c->ascii(), "0") == 0)
    {
      set_condition_value_min(0);
      set_condition_value_max(0);
    }
    else
    {
      set_condition_value_min((QString) c->at(0));
      set_condition_value_max(c->ascii()+2);
    }
    //set_condition_value(*c);
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    // A dimmer is actually a light
    dev = new dimm(QString(""));
    Draw();
}

QString device_condition_dimming::get_current_value()
{
  char val[50];
  int val_m = get_condition_value_min();
  //if(val_m == 0)
  sprintf(val, "%d", val_m);
    return val;
  //else
    
}

int device_condition_dimming::get_min() 
{ 
    return 0; 
} 

int device_condition_dimming::get_max()
{
    return 100;
}

int device_condition_dimming::get_step()
{
    return 10;
}


void device_condition_dimming::Up()
{
    qDebug("device_condition_dimming::Up()");
    
    int val = get_current_value_min();
    switch(val)
    {
      case 0:
        set_current_value_min(2);
        set_current_value_max(4);
        break;
      case 2:
        set_current_value_min(5);
        set_current_value_max(7);
        break;
      case 5:
        set_current_value_min(8);
        set_current_value_max(10);
        break;
      default:
        break;
    }
    Draw();
    show();
}

void device_condition_dimming::Down()
{
    qDebug("device_condition_dimming::Down()");
    int val = get_current_value_min();
    switch(val)
    {
      case 2:
        set_current_value_min(0);
        set_current_value_max(0);
        break;
      case 5:
        set_current_value_min(2);
        set_current_value_max(4);
        break;
      case 8:
        set_current_value_min(5);
        set_current_value_max(7);
        break;
      default:
        break;
    }
    Draw();
    show();
}

void device_condition_dimming::Draw()
{
    char tmp[50];
    QString u;
    qDebug("device_condition_dimming::Down()");
    get_unit(u);
    int v = get_current_value_min();
    if(!v)
      sprintf(tmp, "OFF");
    else
    {
      int M = get_current_value_max();
      sprintf(tmp, "%d%s - %d%s", v*10, u.ascii(), M*10, u.ascii());
    }
    ((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming::OK()
{
    qDebug("device_condition_dimming::OK()");
    set_condition_value_min(get_current_value_min());
    set_condition_value_max(get_current_value_max());
}

void device_condition_dimming::reset()
{
    qDebug("device_condition_dimming::reset()");
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    Draw();
}

void device_condition_dimming::get_unit(QString& out)
{
    out = "%";
}

bool device_condition_dimming::show_OFF_on_zero()
{
    return true;
}

void device_condition_dimming::set_condition_value_min(int s)
{
    qDebug("device_condition_dimming::set_condition_value_min(%d)", s);
    min_val = s;
}

void device_condition_dimming::set_condition_value_min(QString s)
{
    qDebug("device_condition_dimming::set_condition_value_min(%s)", s.ascii());
    min_val = s.toInt();
}

int device_condition_dimming::get_condition_value_min()
{
    return min_val;
}

void device_condition_dimming::set_condition_value_max(int s)
{
    qDebug("device_condition_dimming::set_condition_value_max(%d)", s);
    max_val = s;
}

void device_condition_dimming::set_condition_value_max(QString s)
{
    qDebug("device_condition_dimming::set_condition_value_max(%s)", s.ascii());
    max_val = s.toInt();
}

int device_condition_dimming::get_condition_value_max()
{
    return max_val;
}

int device_condition_dimming::get_current_value_min()
{
    return current_value_min;
}

void device_condition_dimming::set_current_value_min(int min)
{
    current_value_min = min;
}

int device_condition_dimming::get_current_value_max()
{
    return current_value_max;
}

void device_condition_dimming::set_current_value_max(int max)
{
    current_value_max = max;
}

#if 0
void device_condition_dimming::Draw()
{
    QLCDNumber *l = (QLCDNumber *)frame;
    l->display(get_current_value());
    show();
}
#endif

int device_condition_dimming::set_condition_value(QString s)
{
    return device_condition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming::get_condition_value(QString& out)
{
    char tmp[100];
    qDebug("device_condition_dimming::get_condition_value()");
    if(get_condition_value_min() == 0)
      sprintf(tmp, "0");
    else
      sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
    out =  tmp ;
}

void device_condition_dimming::status_changed(QPtrList<device_status> sl)
{
    //device_status::type t = ds->get_type();
    int trig_v_min = get_condition_value_min();
    int trig_v_max = get_condition_value_max();
    int trig_v = -1;
    stat_var curr_lev(stat_var::LEV);
    stat_var curr_speed(stat_var::SPEED);
    stat_var curr_status(stat_var::ON_OFF);
    int val10;
    QPtrListIterator<device_status> *dsi = new QPtrListIterator<device_status>(sl);
    dsi->toFirst();
    device_status *ds;
    qDebug("device_condition_dimming::status_changed()");
    while( ( ds = dsi->current() ) != 0) {
    //qDebug("trigger value is %d", trig_v);
      switch (ds->get_type()) {
        case device_status::LIGHTS:
          qDebug("Light status variation, ignored");
          break;
        case device_status::DIMMER:
          ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
          qDebug("dimmer status variation");
          qDebug("level = %d", curr_lev.get_val()/10);
          qDebug("trigger value min is %d - max is %d, val10 = %d", trig_v_min, trig_v_max, curr_lev.get_val()/10);
          if((curr_lev.get_val()/10 >= trig_v_min) && (curr_lev.get_val()/10 <= trig_v_max)){
            qDebug("Condition triggered");
            satisfied = true;
          } else
           satisfied = false;
          break;
        case device_status::DIMMER100:
          qDebug("dimmer 100 status variation, ignored");
          break;
        case device_status::NEWTIMED:
          qDebug("new timed device status variation, ignored");
          break;
        default:
          qDebug("device status of unknown type (%d)", ds->get_type());
          break;
      }
      ++(*dsi);
    }
    delete dsi;
}

/*****************************************************************
 ** Actual dimming 100 value device condition
****************************************************************/
device_condition_dimming_100::device_condition_dimming_100(QWidget *parent, char *name, QString *c) :
device_condition(parent, c)
{
    char sup[10];
    qDebug("device_condition_dimming_100::device_condition_dimming_100(%s)", c->ascii());
    QLabel *l = new QLabel(parent, name);
    l->setAlignment(AlignHCenter|AlignVCenter);
    l->setFont( QFont( "helvetica", 20, QFont::Bold ) );
    frame = l;
    if(strcmp(c->ascii(), "0") == 0)
    {
      set_condition_value_min(0);
      set_condition_value_max(0);
    }
    else
    {
      sprintf(sup, "%s", c->ascii());
      strtok(sup, "-");
      set_condition_value_min(sup);
      sprintf(sup, "%s", strchr(c->ascii(), '-')+1);
      set_condition_value_max(sup);
    }
    //set_condition_value(*c);
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    // A dimmer is actually a light
    dev = new dimm(QString(""));
    Draw();
}

QString device_condition_dimming_100::get_current_value()
{
  char val[50];
  int val_m = get_condition_value_min();
  //if(val_m == 0)
  sprintf(val, "%d", val_m);
    return val;
  //else
    
}

int device_condition_dimming_100::get_min() 
{ 
    return 0; 
} 

int device_condition_dimming_100::get_max()
{
    return 100;
}

int device_condition_dimming_100::get_step()
{
    return 10;
}


void device_condition_dimming_100::Up()
{
    qDebug("device_condition_dimming_100::Up()");
    
    int val = get_current_value_min();
    switch(val)
    {
      case 0:
        set_current_value_min(1);
        set_current_value_max(20);
        break;
      case 1:
        set_current_value_min(21);
        set_current_value_max(40);
        break;
      case 21:
        set_current_value_min(41);
        set_current_value_max(70);
        break;
      case 41:
        set_current_value_min(71);
        set_current_value_max(100);
        break;
      default:
        break;
    }
    Draw();
    show();
}

void device_condition_dimming_100::Down()
{
    qDebug("device_condition_dimming_100::Down()");
    int val = get_current_value_min();
    switch(val)
    {
      case 1:
        set_current_value_min(0);
        set_current_value_max(0);
        break;
      case 21:
        set_current_value_min(1);
        set_current_value_max(20);
        break;
      case 41:
        set_current_value_min(21);
        set_current_value_max(40);
        break;
      case 71:
        set_current_value_min(41);
        set_current_value_max(70);
        break;
      default:
        break;
    }
    Draw();
    show();
}

void device_condition_dimming_100::Draw()
{
    char tmp[50];
    QString u;
    qDebug("device_condition_dimming_100::Down()");
    get_unit(u);
    int v = get_current_value_min();
    if(!v)
      sprintf(tmp, "OFF");
    else
    {
      int M = get_current_value_max();
      sprintf(tmp, "%d%s - %d%s", v, u.ascii(), M, u.ascii());
    }
    ((QLabel *)frame)->setText(tmp);
}

void device_condition_dimming_100::OK()
{
    qDebug("device_condition_dimming_100::OK()");
    set_condition_value_min(get_current_value_min());
    set_condition_value_max(get_current_value_max());
}

void device_condition_dimming_100::reset()
{
    qDebug("device_condition_dimming_100::reset()");
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    Draw();
}

void device_condition_dimming_100::get_unit(QString& out)
{
    out = "%";
}

bool device_condition_dimming_100::show_OFF_on_zero()
{
    return true;
}

void device_condition_dimming_100::set_condition_value_min(int s)
{
    qDebug("device_condition_dimming_100::set_condition_value_min(%d)", s);
    min_val = s;
}

void device_condition_dimming_100::set_condition_value_min(QString s)
{
    qDebug("device_condition_dimming_100::set_condition_value_min(%s)", s.ascii());
    min_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_min()
{
    return min_val;
}

void device_condition_dimming_100::set_condition_value_max(int s)
{
    qDebug("device_condition_dimming_100::set_condition_value_max(%d)", s);
    max_val = s;
}

void device_condition_dimming_100::set_condition_value_max(QString s)
{
    qDebug("device_condition_dimming_100::set_condition_value_max(%s)", s.ascii());
    max_val = s.toInt();
}

int device_condition_dimming_100::get_condition_value_max()
{
    return max_val;
}

int device_condition_dimming_100::get_current_value_min()
{
    return current_value_min;
}

void device_condition_dimming_100::set_current_value_min(int min)
{
    current_value_min = min;
}

int device_condition_dimming_100::get_current_value_max()
{
    return current_value_max;
}

void device_condition_dimming_100::set_current_value_max(int max)
{
    current_value_max = max;
}

int device_condition_dimming_100::set_condition_value(QString s)
{
    return device_condition::set_condition_value(s.toInt() * 10);
}

void device_condition_dimming_100::get_condition_value(QString& out)
{
    char tmp[100];
    qDebug("device_condition_dimming_100::get_condition_value()");
    if(get_condition_value_min() == 0)
      sprintf(tmp, "0");
    else
      sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
    out =  tmp ;
}

void device_condition_dimming_100::status_changed(QPtrList<device_status> sl)
{
    //device_status::type t = ds->get_type();
    int trig_v_min = get_condition_value_min();
    int trig_v_max = get_condition_value_max();
    stat_var curr_lev(stat_var::LEV);
    stat_var curr_speed(stat_var::SPEED);
    stat_var curr_status(stat_var::ON_OFF);
    int val10;
    QPtrListIterator<device_status> *dsi = new QPtrListIterator<device_status>(sl);
    dsi->toFirst();
    device_status *ds;
    qDebug("device_condition_dimming_100::status_changed()");
    while( ( ds = dsi->current() ) != 0) {
    //qDebug("trigger value is %d", trig_v);
      switch (ds->get_type()) {
        case device_status::LIGHTS:
          qDebug("Light status variation, ignored");
          break;
        case device_status::DIMMER:
          ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
          qDebug("dimmer status variation, ignored");
          break;
        case device_status::DIMMER100:
          qDebug("dimmer 100 status variation");
          ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
          ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
          qDebug("level = %d, speed = %d", curr_lev.get_val(), 
          curr_speed.get_val());
          val10 = curr_lev.get_val();
          qDebug("trigger value min is %d - max is %d, val = %d", trig_v_min, trig_v_max, val10);
          if((val10 >= trig_v_min) && (val10 <= trig_v_max)){
            qDebug("Condition triggered");
            satisfied = true;
          } else
           satisfied = false;
          break;
        case device_status::NEWTIMED:
          qDebug("new timed device status variation, ignored");
          break;
        default:
          qDebug("device status of unknown type (%d)", ds->get_type());
          break;
      }
      ++(*dsi);
    }
    delete dsi;
}

/*****************************************************************
 ** Actual volume device condition
****************************************************************/
device_condition_volume::device_condition_volume(QWidget *parent, 
						   char *name, QString *c) :
    device_condition(parent, c)
{
    char sup[10];
    QLabel *l = new QLabel(parent, name);
    l->setAlignment(AlignHCenter|AlignVCenter);
    l->setFont( QFont( "helvetica", 20, QFont::Bold ) );
    frame = l;
    if(strcmp(c->ascii(), "-1") == 0)
    {
      set_condition_value_min(-1);
      set_condition_value_max(-1);
    }
    else
    {
      sprintf(sup, "%s", c->ascii());
      strtok(sup, "-");
      set_condition_value_min(sup);
      sprintf(sup, "%s", strchr(c->ascii(), '-')+1);
      set_condition_value_max(sup);
    }
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    Draw();
    dev = new sound_device(QString(""));
}

void device_condition_volume::set_condition_value_min(int s)
{
    qDebug("device_condition_volume::set_condition_value_min(%d)", s);
    min_val = s;
}

void device_condition_volume::set_condition_value_min(QString s)
{
    qDebug("device_condition_volume::set_condition_value_min(%s)", s.ascii());
    min_val = s.toInt();
}

int device_condition_volume::get_condition_value_min()
{
    return min_val;
}

void device_condition_volume::set_condition_value_max(int s)
{
    qDebug("device_condition_volume::set_condition_value_max(%d)", s);
    max_val = s;
}

void device_condition_volume::set_condition_value_max(QString s)
{
    qDebug("device_condition_volume::set_condition_value_max(%s)", s.ascii());
    max_val = s.toInt();
}

int device_condition_volume::get_condition_value_max()
{
    return max_val;
}

int device_condition_volume::get_current_value_min()
{
    return current_value_min;
}

void device_condition_volume::set_current_value_min(int min)
{
    current_value_min = min;
}

int device_condition_volume::get_current_value_max()
{
    return current_value_max;
}

void device_condition_volume::set_current_value_max(int max)
{
    current_value_max = max;
}

int device_condition_volume::get_min() 
{ 
    return 0; 
} 

int device_condition_volume::get_max()
{
    return 31;
}

#if 0
int device_condition_volume::get_step()
{
    return 10;
}
#endif

int device_condition_volume::set_condition_value(QString s)
{
#if 0
    // Mail di agresta del 29/05/2006
    static const int trans_table[] = 
    { 0, 3, 6, 9, 12, 15, 19, 22, 25, 28, 31 };
    int v = s.toInt()/get_step() - 1;
    qDebug("device_condition_volume::set_condition_value()");
    if(v < 0)
	v = 0;
    if(v > 10)
	v = sizeof(trans_table) / sizeof(v);
#endif
    // FIXME: USE device_condition::set_condition_value
    int v = s.toInt();
    qDebug("setting condition value to %d", v);
    return device_condition::set_condition_value(v);
}

void device_condition_volume::get_condition_value(QString& out)
{
    char tmp[100];
    qDebug("device_condition_volume::get_condition_value()");
    if(get_condition_value_min() == -1)
      sprintf(tmp, "-1");
    else
      sprintf(tmp, "%d-%d", get_condition_value_min(), get_condition_value_max());
    out =  tmp ;
}

void device_condition_volume::Up()
{
    qDebug("device_condition_volume::Up()");
    int v_m = get_current_value_min();
    int v_M = get_current_value_max();
    qDebug("v_m = %d - v_M = %d", v_m, v_M);
    if(v_m == -1)
    {
      v_m = 0;
      v_M = 31;
    }
    else if(v_m == 0)
    {
      if(v_M == 31)
        v_M = 6;
      else
      {
        v_m = 7;
        v_M = 12;
      }
    }
    else if(v_m == 7)
    {
      v_m = 13;
      v_M = 22;
    }
    else
    {
      v_m = 23;
      v_M = 31;
    }
    qDebug("new value m = %d - M = %d", v_m, v_M);
    set_current_value_min(v_m);
    set_current_value_max(v_M);
    Draw();
    show();
}

void device_condition_volume::Down()
{
    qDebug("device_condition_volume::Down()");
    int v_m = get_current_value_min();
    int v_M = get_current_value_max();
    qDebug("v_m = %d - v_M = %d", v_m, v_M);
    if(v_m == 23)
    {
      v_m = 13;
      v_M = 22;
    }
    else if(v_m == 13)
    {
      v_m = 7;
      v_M = 12;
    }
    else if(v_m == 7)
    {
      v_m = 0;
      v_M = 6;
    }
    else if(v_m == 0)
    {
      if(v_M == 6)
        v_M = 31;
      else
      {
        v_m = -1;
        v_M = -1;
      }
    }
    qDebug("new value m = %d - M = %d", v_m, v_M);
    set_current_value_min(v_m);
    set_current_value_max(v_M);
    Draw();
    show();
}

void device_condition_volume::OK()
{
    qDebug("device_condition_volume::OK()");
    set_condition_value_min(get_current_value_min());
    set_condition_value_max(get_current_value_max());
}

void device_condition_volume::Draw()
{
    char tmp[50];
    QString u;
    get_unit(u);
    int val_min = get_current_value_min();
    int val_max = get_current_value_max();
    qDebug("device_condition_volume::Draw(), val_min = %d - val_max = %d", val_min, val_max);
    if(val_min == -1)
      sprintf(tmp, "OFF");
    else if((val_min == 0) && (val_max == 31))
      sprintf(tmp, "ON");
    else
      sprintf(tmp, "%d%s - %d%s", (val_min == 0 ? 0 : (10 * (val_min <= 15 ? val_min/3 : (val_min-1)/3) + 1)), u.ascii(), 10 * (val_max <= 15 ? val_max/3 : (val_max-1)/3), u.ascii());
    ((QLabel *)frame)->setText(tmp);
}

void device_condition_volume::status_changed(QPtrList<device_status> sl)
{
  //device_status::type t = ds->get_type();
  int trig_v_min = get_condition_value_min();
  int trig_v_max = get_condition_value_max();
  stat_var curr_volume(stat_var::AUDIO_LEVEL);
  stat_var curr_stato(stat_var::ON_OFF);
  qDebug("device_condition_volume::status_changed()");
  QPtrListIterator<device_status> *dsi = 
  new QPtrListIterator<device_status>(sl);
  dsi->toFirst();
  device_status *ds;
  while( ( ds = dsi->current() ) != 0) {
    switch (ds->get_type()) {
      case device_status::AMPLIFIER:
        qDebug("Amplifier status change");
        qDebug("Confition value_min = %d - value_max = %d", trig_v_min, trig_v_max);
        ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_volume);
        ds->read(device_status_amplifier::ON_OFF_INDEX, curr_stato);
        qDebug("volume = %d - stato = %d", curr_volume.get_val(), curr_stato.get_val());
        if((trig_v_min == -1) && (curr_stato.get_val() == 0))
        {
          qDebug("Condition triggered");
          satisfied = true;
        }
        else if((curr_stato.get_val() == 1) && (curr_volume.get_val() >= trig_v_min) && (curr_volume.get_val() <= trig_v_max))
        {
          qDebug("Condition triggered");
          satisfied = true;
        }
        else
          satisfied = false;
        break;
      default:
        qDebug("device status of unknown type (%d)", ds->get_type());
        break;
    }
    ++(*dsi);
  }
  delete dsi;
}

void device_condition_volume::get_unit(QString& out)
{
    out = "%";
}

void device_condition_volume::reset()
{
    qDebug("device_condition_volume::reset()");
    set_current_value_min(get_condition_value_min());
    set_current_value_max(get_condition_value_max());
    Draw();
}

/*****************************************************************
 ** Actual temperature device condition
****************************************************************/
device_condition_temp::device_condition_temp(QWidget *parent, 
					     char *name, QString *c) :
    device_condition(parent, c)
{
#if 0
    QLCDNumber *l = new QLCDNumber(parent, name);
    l->setFrameStyle( QFrame::Plain );
    l->setLineWidth(0);
    l->setNumDigits(3);
    l->setSegmentStyle(QLCDNumber::Flat);    
#else
    QLabel *l = new QLabel(parent, name);
    l->setAlignment(AlignHCenter|AlignVCenter);
    l->setFont( QFont( "helvetica", 20, QFont::Bold ) );
#endif
    frame = l;
    set_condition_value(*c);
    set_current_value(device_condition::get_condition_value());
    Draw();
    dev = new temperature_probe(QString(""));
}

int device_condition_temp::get_min()
{
    return -50;
}

int device_condition_temp::get_max()
{
    return 500;
}

int device_condition_temp::get_step()
{
    return 5;
}

int device_condition_temp::get_divisor()
{
    return 10;
}

void device_condition_temp::get_unit(QString& out)
{
    out = "°C ±1°C" ;
}

#if 0
void device_condition_temp::Draw()
{
    QLCDNumber *l = (QLCDNumber *)frame;
    int val = get_current_value();
    char tmp[100] ;
    sprintf(tmp, "%d.%d", val/10, val%10);
    l->display(tmp);
}
#else
void device_condition_temp::Draw()
{
    char tmp[50];
    QString u;
    get_unit(u);
    int val = get_current_value();
    qDebug("device_condition_temp::Draw(), val = %d", val);
    if(val == -5)
      sprintf(tmp, "-0.5%s ", u.ascii());
    else
      sprintf(tmp, "%d.%d%s", val/10, val >= 0 ? val%10 : -val%10, u.ascii());
    ((QLabel *)frame)->setText(tmp);
}
#endif

int device_condition_temp::set_condition_value(QString s)
{
    bool neg = s[0] == '1';
    int val = (s.right(3)).toInt();
    return device_condition::set_condition_value(neg ? -val : val);
}

void device_condition_temp::get_condition_value(QString& out)
{
    char tmp[100];
    int val = device_condition::get_condition_value();
    sprintf(tmp, "%c%3d", (val < 0 ? '1' : '0') , abs(val));
    out = tmp;
}

void device_condition_temp::status_changed(QPtrList<device_status> sl)
{
    //device_status::type t = ds->get_type();
    int trig_v = device_condition::get_condition_value();
    stat_var curr_temp(stat_var::TEMPERATURE);
    qDebug("device_condition_temp::status_changed()");
    qDebug("trig_v = %d", trig_v);
     QPtrListIterator<device_status> *dsi = 
      new QPtrListIterator<device_status>(sl);
    dsi->toFirst();
    device_status *ds;
    while( ( ds = dsi->current() ) != 0) {
	switch (ds->get_type()) {
	case device_status::TEMPERATURE_PROBE:
	    qDebug("Temperature changed");
	    ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, 
		     curr_temp);
	    qDebug("Current temperature %d", curr_temp.get_val());
	    if((curr_temp.get_val() >= (trig_v-10)) &&  (curr_temp.get_val() <= (trig_v+10))){
		qDebug("Condition triggered");
		satisfied = true;
	    } else
		satisfied = false;
	    break;
	default:
	    qDebug("device status of unknown type (%d)", ds->get_type());
	    break;
	}
	++(*dsi);
    }
    delete dsi;
}
