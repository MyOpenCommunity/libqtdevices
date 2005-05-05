/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** genpage.cpp
**
**pagine di visualizzazione generiche
**
****************************************************************/

#include "genpage.h"
#include "main.h"
#include <qwidget.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qfile.h>

genPage::genPage( QWidget *parent,const char *name , unsigned char tipo,const char *img, unsigned int f)
        : BtLabel( parent, name,  f)
{ 

     setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
     setCursor (QCursor (blankCursor));
#endif
     switch (tipo){
     case RED:setPaletteBackgroundColor(QColor(255,0,0));break;
     case BLUE:setPaletteBackgroundColor(QColor(0,0,255));break;
     case GREEN:setPaletteBackgroundColor(QColor(0,255,0));break;
     case IMAGE:if (QFile::exists(img)) setPixmap(QPixmap(img));break;
     }
}

/*void   versio::setPaletteForegroundColor( const QColor & c )
{
         datiGen->setPaletteBackgroundColor(c);
         bticino->setPaletteBackgroundColor(c);
        BtLabel::setPaletteForegroundColor(c);
}
void   versio::setPaletteBackgroundColor( const QColor &  c)
{
      datiGen->setPaletteForegroundColor(c);
       bticino->setPaletteForegroundColor(c);
      BtLabel::setPaletteBackgroundColor(c);      
}*/

