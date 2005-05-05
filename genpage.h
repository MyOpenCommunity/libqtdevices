/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** genpage.h
**
**definizioni della pagine di visualizzazione versioni
**
****************************************************************/
#ifndef  GENERIC_PAGES_H
#define GENERIC_PAGES_H


#include "btlabel.h"

#include <qfont.h>
#include <stdlib.h>
#include <string.h>

class genPage : public BtLabel
{
    Q_OBJECT
public:
     genPage( QWidget *parent= NULL, const char *name = NULL, unsigned char tipo=0, const char *img = NULL,unsigned int f =  0);    
 /*    void   setPaletteForegroundColor( const QColor & );
     void   setPaletteBackgroundColor( const QColor & );
*/
public slots:
signals:
private:
};


enum {
    RED=0,
    GREEN=1,
    BLUE=2,
    IMAGE=3,
    NONE=4
};

#endif //GENERIC_PAGES_H

