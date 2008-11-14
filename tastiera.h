/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** tastiera.h
**
**definizioni della pagina di tastiera numerica
**
****************************************************************/

#ifndef TASTIERA_H
#define TASTIERA_H

#include "main.h"

#include <QWidget>

class BtButton;
class QLabel;
class QButtonGroup;

#define LINE MAX_HEIGHT/5

/*!
  \class tastiera
  \brief This class is the implementation of a keypad intended for password inserting.

  \author Davide
  \date lug 2005
*/

class tastiera : public QWidget
{
Q_OBJECT
public:
    tastiera(QWidget *parent=0, const char *name=0, int line=LINE);
/*!
  \brief Draws the page
*/
	void draw();

/*! \enum tastiType
  differentiate between encripted and clean mode
*/
	enum tastiType
	{
		HIDDEN = 0,  /*!< When the code is composed the only a \a * is shown for each digit on the display */
		CLEAN  /*!< When the code is composed the digits are shown on the display*/
	};

/*!
  \brief Selects the mode of operation (encripted or not).
*/
	void setMode(tastiType t);
public slots:
/*!
  \brief Executed when the \a ok \a button is clicked. Hides the object and closes.
*/
	void ok();

/*!
  \brief Executed when the \a canc \a button is clicked. Hides the object and closes with NULL result.
*/
	void canc();

	void showTastiera();

protected:
	QLabel *digitLabel, *scrittaLabel;

private:
	BtButton *zeroBut, *unoBut, *dueBut, *treBut, *quatBut, *cinBut, *seiBut, *setBut, *ottBut, *novBut, *cancBut, *okBut;
	QString text;
	tastiType mode;
	QButtonGroup *buttons_group;

private slots:
	void buttonClicked(int number);

signals:
/*!
  \brief Emitted when the object is closed. The argument represent the code composed by the user.
*/
	void Closed(char*);
};


//! Normal keyboard with a line containing 8 disabled buttons representing 8 status bits
class tastiera_con_stati : public tastiera
{
Q_OBJECT
public:
	//! Constructor
	tastiera_con_stati(int s[8], QWidget *parent=0, const char *name=0);
public slots:
	//! Invert fg/bg colors for active "stati"
	void show();
private:
	BtButton *stati[8];
	bool st[8];
};


#endif // TASTIERA_H
