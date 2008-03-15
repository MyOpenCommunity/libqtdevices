/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements generic Buttons Bar
 *
 * it is used to manage a group of BtButtons and to arange it Horizontally or 
 * Vertically
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \date December 2007
 */

#ifndef BUTTONS_BAR_H
#define BUTTONS_BAR_H

#include <qframe.h>
#include <qlcdnumber.h>
#include <qmap.h>
#include <qptrvector.h>
#include <qbuttongroup.h>

#include <qlayout.h>
#include <qpainter.h>

#include "btbutton.h"
#include "main.h"
#include "btlabel.h"
#include "bannfrecce.h"
#include "bann3but.h"


/** \class ButtonsBar
 *  this class implements a buttons bar
 */
class ButtonsBar : public QWidget
{
Q_OBJECT
public:
	ButtonsBar(QWidget *parent = 0, int number_of_buttons = 1, Orientation orientation = Horizontal);
	bool setButtonIcons(int button_number, const QPixmap &icon, const QPixmap &pressed_icon);
	
	/// Set toggle state on all buttons
	void setToggleButtons(bool enable);
	/// If Toggle Status has been enabled, set the given button on;
	void setToggleStatus(int button_up_index);
			;
	void showButton(int idx);
	void hideButton(int idx);
	void setBGColor(QColor c);
	void setFGColor(QColor c);
private:
	bool                  isToggleBar;
	QPtrVector<BtButton>  buttons_list;
	QButtonGroup          *buttons_group;
signals:
	/// The signal pressed(int ) of QButton group is connected to the following by the constructor
	void clicked(int button_number);
};

#endif // BUTTONS_BAR_H
