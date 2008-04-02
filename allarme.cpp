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
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"
#include "banner.h"
#include "allarme.h"
#include "bannfrecce.h"
#include "btlabel.h"

/*****************************************************************
** Generic alarm
****************************************************************/	

allarme::allarme(QWidget *parent, const QString & name, char *indirizzo, char *IconaDx,
		 altype t): 
    QFrame(parent, name.ascii())
{
    qDebug("allarme::allarme()");
    qDebug("indirizzo = %s, IconaDx = %s, tipo = %d", indirizzo, IconaDx, t);
    type = t;
    SetIcons(IconaDx);
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif  
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT); 
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    descr->setText(name);
    connect(bnav, SIGNAL(backClick()), this, SIGNAL(Back()));
    connect(bnav, SIGNAL(upClick()), this, SIGNAL(Prev()));
    connect(bnav, SIGNAL(downClick()), this, SIGNAL(Next()));
    connect(bnav, SIGNAL(forwardClick()), this, SIGNAL(Delete()));
    // ??
    show() ;
}

void allarme::SetIcons(char *icon)
{
    qDebug("allarme::SetIcons()");
    QPixmap *Icon1 = new QPixmap();
    char *icon1;
    switch(type) {
    case allarme::TECNICO:
	icon1 = IMG_PATH "imgalltec.png";
	break;
    default:
	icon1 = IMG_PATH "imgallintr.png";
	break;
    }
    Icon1->load(icon1);
    Immagine = new BtLabel(this, "icona_alarm");
    Immagine->setPixmap(*Icon1);
    Immagine->setGeometry(MAX_WIDTH/2 - ICON_DIM/2, MAX_HEIGHT/(4*NUM_RIGHE), 
			  ICON_DIM, MAX_HEIGHT/NUM_RIGHE);
    delete(Icon1);
    descr = new BtLabel(this, "descr_alarm");
    descr->setFont( QFont( DEFAULT_FONT, 20, QFont::Bold ) );
    descr->setAlignment(AlignHCenter|AlignVCenter);
    descr->setGeometry(0, MAX_HEIGHT/2 - (MAX_HEIGHT/NUM_RIGHE)/2, 
		       MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

    bnav = new bannFrecce(this, "banner_nav", 4, icon);
    //bnav->setGeometry(0, MAX_HEIGHT-NAV_DIM-NAV_DIM/8, MAX_WIDTH, NAV_DIM);
    bnav->setGeometry(0 , MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE,
		      MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);
}


void allarme::setBGColor(QColor c)
{
    qDebug("allarme::setBGColor()");
    bnav->setBGColor(c);
    setPaletteBackgroundColor(c);    
    Immagine->setPaletteBackgroundColor(c);
    descr->setPaletteBackgroundColor(c);
}

void allarme::setFGColor(QColor c)
{
    qDebug("allarme::setFGColor()");
    bnav->setFGColor(c);
    setPaletteForegroundColor(c);  
    Immagine->setPaletteForegroundColor(c);
    descr->setPaletteForegroundColor(c);
}

void allarme::setBGColor(int r, int g, int b)
{
    return allarme::setBGColor(QColor(r, g, b));
}

void allarme::setFGColor(int r, int g, int b)
{
    return allarme::setFGColor(QColor(r, g, b));
}

void allarme::draw()
{
    qDebug("allarme::draw()");
    //Immagine->draw();
    //descr->draw();
    //bnav->draw();
}

void allarme::show()
{
    qDebug("allarme::show()");
    QFrame::show();
    bnav->show();
    Immagine->show();
    descr->show();
}

void allarme::freezed(bool f)
{
    qDebug("allarme::freezed(%d)", f);
    bool enabled = !f;
    bnav->setEnabled(enabled);
}
