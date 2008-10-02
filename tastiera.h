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

#include <qwidget.h>

class BtLabel;
class BtButton;

#define LINE MAX_HEIGHT/5

/*!
  \class tastiera
  \brief This class is the implementation of a keypad intended for password inserting.

  \author Davide
  \date lug 2005
*/

class  tastiera : public QWidget
{
   Q_OBJECT
public:
    tastiera(QWidget *parent=0, const char *name=0, int line=LINE);

/*!
  \brief Sets the background color for the banner.

  The arguments are RGB components for the color.
*/
	void setBGColor(int, int , int);

/*!
  \brief Sets the foreground color for the banner.

  The arguments are RGB components for the color.
*/
	void setFGColor(int , int , int);

/*!
  \brief Sets the background color for the banner.

  The argument is the QColor description of the color.
*/
	virtual void setBGColor(QColor);

/*!
  \brief Sets the foreground color for the banner.

  The argument is the QColor description of the color.
*/
	virtual void setFGColor(QColor);

	BtButton *zeroBut, *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*seiBut, *setBut, *ottBut, *novBut, *cancBut, *okBut;
	BtLabel* digitLabel, *scrittaLabel;
/*!
  \brief Draws the page
*/
	void draw();

/*!
  \brief Selects the mode of operation (encripted or not).
*/
	void setMode(char);

/*! \enum    tastiType
  differentiate between encripted and clean mode
*/
	enum tastiType
	{
		HIDDEN,  /*!< When the code is composed the only a \a * is shown for each digit on the display */
		CLEAN  /*!< When the code is composed the digits are shown on the display*/
	};

signals:
/*!
  \brief Emitted when the object is closed. The argument represent the code composed by the user.
*/
	void Closed(char*);

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
/*!
  \brief Executed when the \a zero \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press0();

/*!
  \brief Executed when the \a one \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press1();

/*!
  \brief Executed when the \a two \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press2();

/*!
  \brief Executed when the \a three \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press3();

/*!
  \brief Executed when the \a four \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press4();

/*!
  \brief Executed when the \a five \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press5();

/*!
  \brief Executed when the \a six \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press6();

/*!
  \brief Executed when the \a seven \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press7();

/*!
  \brief Executed when the \a eight \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press8();

/*!
  \brief Executed when the \a nine \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/
	void press9();

private:
	char pwd[6];
	char mode;
	void setPaletteBackgroundColor(const QColor &c);
	void setPaletteForegroundColor(const QColor &c);
	void setPaletteBackgroundPixmap(const QPixmap &pixmap);
};


//! Normal keyboard with a line containing 8 disabled buttons representing 8 status bits
class tastiera_con_stati : public tastiera
{
Q_OBJECT
public:
	//! Constructor
	tastiera_con_stati(int s[8], QWidget *parent=0, const char *name=0);
	void setBGColor(QColor);
	void setFGColor(QColor);
public slots:
	//! Invert fg/bg colors for active "stati"
	void show();
private:
	BtButton *stati[8];
	bool st[8];
};


#endif // TASTIERA_H
