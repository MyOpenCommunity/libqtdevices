/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannonoff.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANNONOFF
#define BANNONOFF

#include "banner.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class bannOnOff : public banner
{
    Q_OBJECT
public:
     bannOnOff( QWidget *, const char * );
/*    void SetText(const char *;
    void SetIcons(const char *onIcon=0, const char *offIcon=0);
    void Draw();*/
     void setIcons(const char *onIcon=0, const char *offIcon=0);
public slots:
 /*void Accendi();
  void Spegni();*/
private:
  signals:
/*  void sxClick();
  void dxClick();*/
  
/*  QLabel * BannerIcon;
  QLabel * BannerText;
  QPixmap *  Icon1;
  QPixmap * Icon2; 
  char testo[20];
  char spento;*/
};


#define BUTONOFF_ICON_DIM_X 	120
#define BUTONOFF_ICON_DIM_Y		60	
#define BANONOFF_BUT_DIM		60

#endif //BANNONOFF
