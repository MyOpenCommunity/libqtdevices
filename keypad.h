/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef KEYPAD_H
#define KEYPAD_H

#include "page.h"
#include "window.h"

class QLabel;
class QHBoxLayout;
class QGridLayout;

/*!
	\ingroup Core
	\ingroup Settings
	\brief A numeric keypad intended for password inserting.
*/
class Keypad : public Page
{
Q_OBJECT
public:
	Keypad(bool back_button = true);

	/*!
		\brief Differentiate between encrypted and clean mode.
	*/
	enum Type
	{
		HIDDEN = 0,  /*!< Do not show the digits inserted. */
		CLEAN        /*!< Show the digits inserted. */
	};

	/*!
		\brief Selects the mode of operation (encrypted or not).
	*/
	void setMode(Type t);

	/*!
		\brief Return the text inserted in the virtual keypad.
	*/
	QString getText();

	/*!
		\brief Reset the text inserted by virtual keypad.
	*/
	void resetText();

	void showWrongPassword(bool is_visible);
	void setMessage(const QString &text);

protected:
	// used by KeypadWithState to add the row with the zone state
	void insertLayout(QLayout *l);

private:
	void updateText();

private:
	QGridLayout *top_layout;
	QLabel *digit_label;
	QLabel *warning_label, *msg_label;
	QString text;
	Type mode;

private slots:
	// remove a digit from the value; close the page if the value is empty
	void deleteChar();

	// add a digit to the value
	void buttonClicked(int number);

signals:
	// User confirms the selection
	void accept();
};


// Normal keyboard with a line containing 8 disabled buttons representing 8 status bits
class KeypadWithState : public Keypad
{
Q_OBJECT
public:
	KeypadWithState(const QList<int> &s);

	void setStates(const QList<int> &s);

private:
	QList<QLabel*> states;
	QHBoxLayout *states_layout;

	void drawStates(const QList<int> &s);
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
