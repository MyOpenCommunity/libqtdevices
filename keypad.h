/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** keypad.h
**
** A numeric keypad
**
****************************************************************/

#ifndef KEYPAD_H
#define KEYPAD_H

#include "page.h"
#include "window.h"

class QLabel;
class QVBoxLayout;

/*!
  \class Keypad
  \brief This class is the implementation of a keypad intended for password inserting.

  \author Davide
  \date lug 2005
*/
class Keypad : public Page
{
Q_OBJECT
public:
	Keypad(bool back_button = true);

/*! \enum Type
  differentiate between encripted and clean mode
*/
	enum Type
	{
		HIDDEN = 0,  /*!< When the code is composed the only a \a * is shown for each digit on the display */
		CLEAN  /*!< When the code is composed the digits are shown on the display*/
	};

/*!
  \brief Selects the mode of operation (encripted or not).
*/
	void setMode(Type t);

	/// Return the text inserted in the virtual Keyboard
	QString getText();

	/// A function to reset the text inserted by virtual Keyboard.
	void resetText();

protected:
	// used by KeypadWithState to add the row with the zone state
	void insertLayout(QLayout *l);

private:
	void updateText();

private:
	QVBoxLayout *topLayout;
	QLabel *digitLabel;
	QString text;
	Type mode;

private slots:
	// remove a digit from the value; close the page if the value is empty
	void deleteChar();

	// add a digit to the value
	void buttonClicked(int number);
};


//! Normal keyboard with a line containing 8 disabled buttons representing 8 status bits
class KeypadWithState : public Keypad
{
Q_OBJECT
public:
	KeypadWithState(int s[8]);
};


class KeypadWindow : public Window
{
Q_OBJECT
public:
	KeypadWindow(Keypad::Type type);

	// merthods forwarded to the corresponding Keypad methods
	QString getText();
	void resetText();

private:
	Keypad *keypad;
};

#endif // KEYPAD_H
