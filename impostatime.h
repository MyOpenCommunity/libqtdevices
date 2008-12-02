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

#include "btwidget.h"

class timeScript;
class BtButton;
class QLabel;


/*!
  \class impostaTime
  \brief This is the class used to set time and date.

  \author Davide
  \date lug 2005
*/

class  impostaTime : public BTWidget
{
Q_OBJECT
public:
	impostaTime(QWidget *parent=0);
	BtButton *but[7];

protected:
	virtual void showEvent(QShowEvent *event);

private slots:
	void OKTime();
	void OKDate();

private:
	timeScript *dataOra;
	QLabel *Immagine;
	void disconnectAllButton();
};


#endif // IMPOSTA_TIME_H
