#ifndef  GENERIC_CLASSES_H
#define GENERIC_CLASSES_H

#include "qlabel.h"
#include "openclient.h"
#include "btlabel.h"

#include <qfont.h>
#include <stdlib.h>
#include <string.h>

class versio : public BtLabel
{
    Q_OBJECT
public:
     versio( QWidget *parent= NULL, const char *name = NULL, unsigned int f =  0);     
     void   inizializza();
     void   setPaletteForegroundColor( const QColor & );
     void   setPaletteBackgroundColor( const QColor & );

public slots:
     void gestFrame(char*);
signals:
     void sendFrame(char*);	
private:
     unsigned char vers;
     unsigned char release;
     unsigned char build;
     unsigned char pic_version;
     unsigned char pic_release;
     unsigned char pic_build;
     unsigned char hw;
     BtLabel*datiGen, *bticino;
};

#endif //GENERIC_CLASSES_H
