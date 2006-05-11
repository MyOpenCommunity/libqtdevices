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
    emit(SwitchToPrev());
}

void scenEvo_cond::OK()
{
    qDebug("scenEvo_cond::OK()");
}

void scenEvo_cond::setEnabled(bool e)
{
    qDebug("scenEvo_cond::setEnabled(%d)", e);
}

/*****************************************************************
 ** Advanced scenario management, time condition
****************************************************************/	

scenEvo_cond_h::scenEvo_cond_h(QWidget *parent, char *name) :
  scenEvo_cond(parent, name)
{
    h = new QString("");
    m = new QString("");
    s = new QString("");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif  
    ora=NULL;
    timer = NULL;
}

void scenEvo_cond_h::set_h(const char *_h)
{
    *h = _h;
    qDebug("scenEvo_cond_h::set_h : %s", h->ascii());
}

void scenEvo_cond_h::set_m(const char *_m)
{
    *m = _m;
    qDebug("scenEvo_cond_h::set_m : %s", m->ascii());
}

void scenEvo_cond_h::set_s(const char *_s)
{
    *s = _s;
    qDebug("scenEvo_cond_h::set_s : %s", s->ascii());
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
    // Pulsante in basso a sinistra, area 6
    Icon1 = new QPixmap();
    Icon1->load(getImg(A6_ICON_INDEX));
    qDebug("Area 6: loaded icon %s", getImg(A6_ICON_INDEX));
    getPressName((char *)getImg(A6_ICON_INDEX), iconName, sizeof(iconName));
    if (QFile::exists(iconName)) {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    but[A6_BUTTON_INDEX] = new BtButton(this, "prev");
    but[A6_BUTTON_INDEX]->setGeometry(0, MAX_HEIGHT - 60, 60, 60);
    but[A6_BUTTON_INDEX]->setPixmap(*Icon1);
    if (Icon2)
	but[A6_BUTTON_INDEX]->setPressedPixmap(*Icon2);
    delete Icon1;
    if(Icon2)
	delete Icon2;
    // Pulsante in basso al centro, area 7
    Icon1 = new QPixmap();
    Icon2 = NULL;
    Icon1->load(getImg(A7_ICON_INDEX));
    getPressName((char *)getImg(A7_ICON_INDEX), iconName, sizeof(iconName));
    if (QFile::exists(iconName)) {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    but[A7_BUTTON_INDEX] = new BtButton(this, "ok");
    but[A7_BUTTON_INDEX]->setGeometry(MAX_WIDTH/2 - 30, MAX_HEIGHT - 60, 
				      60, 60);
    but[A7_BUTTON_INDEX]->setPixmap(*Icon1);
    if (Icon2)
	but[A7_BUTTON_INDEX]->setPressedPixmap(*Icon2);
    delete Icon1;
    if(Icon2)
	delete Icon2;
    // Pulsante in basso a destra, area 8
    Icon1 = new QPixmap();
    Icon2 = NULL;
    Icon1->load(getImg(A8_ICON_INDEX));
    getPressName((char *)getImg(A8_ICON_INDEX), iconName, sizeof(iconName));
    if (QFile::exists(iconName)) {  
	Icon2 = new QPixmap();
	Icon2->load(iconName);
    }
    but[A8_BUTTON_INDEX] = new BtButton(this, "next");
    but[A8_BUTTON_INDEX]->setGeometry(MAX_WIDTH - 60, MAX_HEIGHT - 60, 60, 60);
    but[A8_BUTTON_INDEX]->setPixmap(*Icon1);
    if (Icon2)
	but[A8_BUTTON_INDEX]->setPressedPixmap(*Icon2);
    delete Icon1;
    if(Icon2)
	delete Icon2;
    cond_time = new QDateTime(QDateTime::currentDateTime());
    cond_time->setTime(QTime(h->toInt(), m->toInt(), s->toInt()));
    ora = new timeScript(this, "condizione scen evo h", 2 , cond_time);
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
    connect( but[A6_BUTTON_INDEX], SIGNAL(released()), this, SLOT(Prev()));
    connect( but[A7_BUTTON_INDEX], SIGNAL(released()), this, SLOT(OK()));
    connect( but[A8_BUTTON_INDEX], SIGNAL(released()), this, SLOT(Next()));
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

void scenEvo_cond_h::OK()
{
    qDebug("scenEvo_cond_h::OK()");
    *cond_time = ora->getDataOra();
    QDateTime now = QDateTime::currentDateTime();
    int secsto = now.secsTo(*cond_time);
    while(secsto <= 0)
      // Do it tomorrow 
      secsto += 24 * 60 * 60;
#if 0
    if(!timer) {
	qDebug("scenEvo_cond_h::OK(), creating timer");
	timer = new QTimer(this, "scenEvo_cond_h timer");
    }
    connect(timer, SIGNAL(timeout()), this, SLOT(scaduta()));
    if(!timer->isActive()) {
      qDebug("starting timer (%d)", secsto * 1000);
      timer->start(secsto * 1000, true);
    } else {
      qDebug("timer is already active, changing period to %d", secsto * 1000);
      timer->changeInterval(secsto * 1000);
    }
#else
    qDebug("scheduling scaduta() after %d ms", secsto*1000);
    QTimer::singleShot(secsto*1000, this, SLOT(scaduta()) );
#endif
    
}

void scenEvo_cond_h::scaduta()
{
    qDebug("scenEvo_cond_h::scaduta()");
    emit(verificata());
    QDateTime now = QDateTime::currentDateTime();
    int secsto = now.secsTo(*cond_time);
    while(secsto <= 0)
	// Do it tomorrow 
	secsto += 24 * 60 * 60;
#if 0
    qDebug("restarting timer (%d)", secsto * 1000);
    timer->start(secsto * 1000, true);
#else
    qDebug("scheduling scaduta() after %d ms", secsto*1000);
    QTimer::singleShot(secsto*1000, this, SLOT(scaduta()) );
#endif
}

void scenEvo_cond_h::setEnabled(bool e)
{
    qDebug("scenEvo_cond_h::setEnabled(%d)", e);
    for(int i=0; i<8; i++)
	but[i]->setEnabled(e);
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
    *descr = d;
}

void scenEvo_cond_d::set_where(const char *w)
{
    *where = w;
}

void scenEvo_cond_d::set_trigger(const char *t)
{
    *trigger = t;
}

const char *scenEvo_cond_d::getDescription(void)
{
  return "scenEvo device condition";
}

void scenEvo_cond_d::mostra()
{
    qDebug("scenEvo_cond_d::mostra()");
    for (uchar idx=0; idx < 8; idx++)
	if(but[idx])
	    but[idx]->show();   
    area1_ptr->show();
    area2_ptr->show();
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
}

void scenEvo_cond_d::SetButtonIcon(int icon_index, int button_index)
{
    QPixmap* Icon1;
    QPixmap* Icon2;
    char iconName[MAX_PATH];
    Icon1 = new QPixmap();
    getPressName((char *)getImg(icon_index), iconName, sizeof(iconName));
    if(QFile::exists(getImg(icon_index))) {
	Icon1->load(getImg(icon_index));
	but[button_index]->setPixmap(*Icon1);
    }
    if(QFile::exists(iconName)) {
	Icon2 = new QPixmap();
	Icon2->load(iconName);
	but[button_index]->setPressedPixmap(*Icon2);
    }
    delete Icon1;
    if(Icon2) 
	delete Icon2;
}

void scenEvo_cond_d::SetIcons() 
{
    qDebug("scenEvo_cond_h::SetIcons()");
    QPixmap* Icon1 = new QPixmap();
    QPixmap* Icon2 = NULL;
    char iconName[MAX_PATH];
    for(int i=0; i<6; i++)
	qDebug("icon[%d] = %s", i, getImg(i));
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT); 
    area1_ptr = new BtLabel(this, "Area1");
    area1_ptr->setGeometry(0, 0, BUTTON_DIM, BUTTON_DIM);
    area2_ptr = new BtLabel(this, "Area2");
    area2_ptr->setGeometry(BUTTON_DIM, BUTTON_DIM/2 - TEXT_Y_DIM/2, 
			   TEXT_X_DIM, TEXT_Y_DIM);
    BtButton *b = new BtButton(this, "Up button");
    but[A3_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 80, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Up()));
    b = new BtButton(this, "Down button");
    but[A4_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, 190, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Down()));
    b = new BtButton(this, "Prev button");
    but[A5_BUTTON_INDEX] = b;
    b->setGeometry(0, MAX_HEIGHT - BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(Prev()));
#if 0
    b = new BtButton(this, "Next button");
    but[A6_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH/2 - BUTTON_DIM/2, MAX_HEIGHT - BUTTON_DIM, 
		   BUTTON_DIM, BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(OK()));
#else
    but[A6_BUTTON_INDEX] = NULL;
#endif
    b = new BtButton(this, "OK button");
    but[A7_BUTTON_INDEX] = b;
    b->setGeometry(MAX_WIDTH - BUTTON_DIM, MAX_HEIGHT - BUTTON_DIM, BUTTON_DIM, 
		   BUTTON_DIM);
    connect(b, SIGNAL(clicked()), this, SLOT(OK()));
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
    // area #7
    SetButtonIcon(A7_ICON_INDEX, A7_BUTTON_INDEX);
}

void scenEvo_cond_d::Up(void)
{
    qDebug("scenEvo_cond_d::Up()");
}

void scenEvo_cond_d::Down(void)
{
    qDebug("scenEvo_cond_d::Down()");
}

void scenEvo_cond_d::OK(void)
{
    qDebug("scenEvo_cond_d::OK()");
}
