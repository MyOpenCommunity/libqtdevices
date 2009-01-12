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

#include "page.h"
#include "main.h"

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
class tastiera : public Page
{
Q_OBJECT
public:
	tastiera(int line=LINE);
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

	/// Return the text inserted in the virtual Keyboard
	QString getText();

	/// A function to reset the text inserted by virtual Keyboard.
	void resetText();

public slots:
/*!
  \brief Executed when the \a ok \a button is clicked. Hides the object and closes.
*/
	void ok();

/*!
  \brief Executed when the \a canc \a button is clicked. Hides the object and closes with NULL result.
*/
	void canc();

protected:
	QLabel *digitLabel, *scrittaLabel;
	virtual void showEvent(QShowEvent *event);

private:
	BtButton *zeroBut, *unoBut, *dueBut, *treBut, *quatBut, *cinBut, *seiBut, *setBut, *ottBut, *novBut, *cancBut, *okBut;
	QString text;
	tastiType mode;
	QButtonGroup *buttons_group;

private slots:
	void buttonClicked(int number);
};


//! Normal keyboard with a line containing 8 disabled buttons representing 8 status bits
class tastiera_con_stati : public tastiera
{
Q_OBJECT
public:
	tastiera_con_stati(int s[8]);
protected:
	virtual void paintEvent(QPaintEvent *event);  //! Invert fg/bg colors for active "stati"
private:
	BtButton *stati[8];
	bool st[8];
};


#endif // TASTIERA_H
