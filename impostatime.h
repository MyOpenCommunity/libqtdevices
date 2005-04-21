/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** impostaTime.h
**
**definizioni della pagine di impostazione data/ora
**
****************************************************************/

#ifndef IMPOSTA_TIME_H
#define IMPOSTA_TIME_H

#include <qframe.h>
#include <qlabel.h>
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include "timescript.h"
//#include <qobject.h>

    
class  impostaTime : public QFrame
{
   Q_OBJECT
public:
    impostaTime( QWidget *parent=0, const char *name=0 );
   void 	setBGColor(QColor);
   void 	setFGColor(QColor);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   int 	setBGPixmap(char* );
   BtButton*  but[7];
    QLabel* Immagine;
    
signals:
    void 	Closed();
    void 	gestFrame(char*);
    void 	sendFrame(char*);

public slots:
    void	OKTime();
    void    OKDate();
    void 	mostra();
private:
   timeScript* dataOra;
//   QProcess* dateChange;
};


#endif // IMPOSTA_TIME_H
