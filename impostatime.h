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

class BtDateEdit;
class BtTimeEdit;
class ChangeDate;


/*!
  \class ChangeTime
  \brief This is the class used to set time.

  \author Davide
  \date lug 2005
*/
class  ChangeTime : public Page
{
Q_OBJECT
public:
	ChangeTime();

public slots:
	virtual void showPage();

protected:
	void timerEvent(QTimerEvent* event);

private slots:
	void acceptTime();

private:
	BtTimeEdit *edit;
	ChangeDate *date; // next page
	int timer_id;
};


/*!
  \class ChangeDate
  \brief This is the class used to set time.

  \author Davide
  \date lug 2005
*/
class  ChangeDate : public Page
{
Q_OBJECT
public:
	ChangeDate();

private slots:
	void acceptDate();

private:
	BtDateEdit *edit;
};


#endif // IMPOSTA_TIME_H
