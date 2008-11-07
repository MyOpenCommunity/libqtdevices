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

#include "btbutton.h"

#include <QButtonGroup>
#include <QPixmap>
#include <QVector>
#include <QWidget>



/** \class ButtonsBar
 *  this class implements a buttons bar
 */
class ButtonsBar : public QWidget
{
Q_OBJECT
public:
	ButtonsBar(QWidget *parent = 0, unsigned int number_of_buttons = 1, Qt::Orientation orientation = Qt::Horizontal);
	~ButtonsBar();
	bool setButtonIcons(unsigned int button_number, const QPixmap &icon, const QPixmap &pressed_icon);
	
	void showButton(int idx);
	void hideButton(int idx);
private:
	QVector<BtButton*>  buttons_list;
	QButtonGroup *buttons_group;
signals:
	/// The signal pressed(int ) of QButton group is connected to the following by the constructor
	void clicked(int button_number);
};

#endif // BUTTONS_BAR_H
