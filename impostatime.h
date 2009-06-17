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

#include "page.h"

class timeScript;
class BtButton;
class QLabel;


/*!
  \class impostaTime
  \brief This is the class used to set time and date.

  \author Davide
  \date lug 2005
*/

class  impostaTime : public Page
{
Q_OBJECT
public:
	impostaTime();
	BtButton *but[7];

private slots:
	void OKTime();
	void OKDate();

private:
	timeScript *dataOra;
	QLabel *Immagine;
	void setDatePage();

private slots:
	void setTimePage();

};


#endif // IMPOSTA_TIME_H
