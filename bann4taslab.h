/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4taslab.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANN4TASLAB_H
#define BANN4TASLAB_H

#include "banner.h"
#include "main.h"


class bann4tasLab : public banner
{
    Q_OBJECT
public:
     bann4tasLab( QWidget * parent=NULL, const char *name=NULL);     
signals:
/*  void  backClick(); 
  void  upClick(); 
  void  downClick(); 
  void  forwardClick(); */
private:
};

#define BUT4TL_DIM 		60
#define ICO4TL_DIM		120	


#endif //BANN4TASLAB_H
