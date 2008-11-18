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

#include "btbutton.h"
#include "main.h"
#include "timescript.h"

#include <QFrame>

class QLabel;


/*!
  \class impostaTime
  \brief This is the class used to set time and date.

  \author Davide
  \date lug 2005
*/

class  impostaTime : public QFrame
{
Q_OBJECT
public:
	impostaTime(QWidget *parent=0, const char *name=0);
	BtButton *but[7];

signals:
/*!
  \brief Emitted when the object is closed.
*/
	void Closed();

public slots:
/*!
  \brief Draws the object.
*/
	void mostra();

protected:
	void hideEvent(QHideEvent *event);

private slots:
	void OKTime();
	void OKDate();

private:
	timeScript *dataOra;
	QLabel *Immagine;
};


#endif // IMPOSTA_TIME_H
