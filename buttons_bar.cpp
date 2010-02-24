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


#include "buttons_bar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QString>


/// ***********************************************************************************************************************
/// Methods for ButtonsBar
/// ***********************************************************************************************************************

ButtonsBar::ButtonsBar(QWidget *parent, unsigned int number_of_buttons, Qt::Orientation orientation) :
	QWidget(parent, Qt::FramelessWindowHint)
{
	// TODO: capire come mai non si puo' usare l'operatore ternario!!
	QBoxLayout *box;
	if (orientation == Qt::Horizontal)
		box = new QHBoxLayout(this);
	else
		box = new QVBoxLayout(this);

	/// Create ButtonGroup, this can handle QButton objects
	buttons_group = new QButtonGroup(this);

	// TODO: capire come farlo con qt4!
	// Permettono la corretta disposizione dei pulsanti
	//buttons_group->setInsideMargin(0);
	//buttons_group->setInsideSpacing(0);

	// Elimina il bordo bianco
	//buttons_group->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	/// Init Vector to store BtButton pointers
	buttons_list.reserve(number_of_buttons);

	for (uint i = 0; i < number_of_buttons; i++)
	{
		BtButton* btn = new BtButton(this);
		buttons_group->addButton(btn, i);
		box->addWidget(btn);
		buttons_list.append(btn);
	}

	/// Connect Signal
	connect(buttons_group, SIGNAL(buttonClicked(int)), this, SIGNAL(clicked(int)));
}

ButtonsBar::~ButtonsBar()
{
}

bool ButtonsBar::setButtonIcon(unsigned int button_number, const QString &icon_path)
{
	if ((int)button_number >= buttons_list.size())
		return false;

	buttons_list.at(button_number)->setImage(icon_path);
	// Non è necessario il setGeometry o il resize dei pulsanti,
	// ma è molto importante nel costruttore del ButtonGroup indicare il numero di pulsanti
	return true;
}

void ButtonsBar::showButton(int idx)
{
	buttons_list[idx]->show();
}

void ButtonsBar::hideButton(int idx)
{
	buttons_list[idx]->hide();
}
