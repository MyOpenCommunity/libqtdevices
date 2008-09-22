/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** MultimediaSource.cpp
 **
 ** finestra di dati sulla sorgente MultimediaSource
 **
 ****************************************************************/

#include "buttons_bar.h"

#include <qlayout.h>
#include <qstring.h>


/// ***********************************************************************************************************************
/// Methods for ButtonsBar
/// ***********************************************************************************************************************

ButtonsBar::ButtonsBar(QWidget *parent, unsigned int number_of_buttons, Orientation orientation) :
	QWidget(parent, 0, WStyle_NoBorder | WStyle_Customize)
{
	/// Create ButtonGroup, this can handle QButton objects
	buttons_group = new QButtonGroup(number_of_buttons, orientation, this);
	// Permettono la corretta disposizione dei pulsanti
	buttons_group->setInsideMargin(0);
	buttons_group->setInsideSpacing(0);
	// Elimina il bordo bianco
	buttons_group->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	/// Init Vector to store BtButton pointers
	buttons_list.clear();
	buttons_list.resize(number_of_buttons);

	for (uint i = 0; i < number_of_buttons; i++)
	{
		// the BtButton is created and inserted in ButtonGroup automatically because it is
		// its parent. We also need to store the pointer in the button_list vector because
		// when we call find method of buttons group we obtain a QButton pointer and no a
		// BtButton pointer!
		buttons_list.insert(i, new BtButton(buttons_group, QString("Button%1").arg(i)));
	}

	// Senza il seguente Layout la barra non viene visualizzata in modo corretto
	if (orientation == Horizontal)
	{
		QHBoxLayout *main_layout = new QHBoxLayout(this);
		main_layout->addWidget(buttons_group);
	}
	else if (orientation == Vertical)
	{
		QVBoxLayout *main_layout = new QVBoxLayout(this);
		main_layout->addWidget(buttons_group);
		main_layout->addStretch();
	}

	isToggleBar = false;

	/// Connect Signal
	connect(buttons_group, SIGNAL(clicked(int)), this, SIGNAL(clicked(int)));
}

bool ButtonsBar::setButtonIcons(unsigned int button_number, const QPixmap &icon, const QPixmap &pressed_icon)
{
	if (!buttons_list.at(button_number))
		return false;

	buttons_list.at(button_number)->setPixmap(icon);
	buttons_list.at(button_number)->setPressedPixmap(pressed_icon);
	// Non è necessario il setGeometry o il resize dei pulsanti,
	// ma è molto importante nel costruttore del ButtonGroup indicare il numero di pulsanti
	return true;
}

void ButtonsBar::setToggleButtons(bool enable)
{
	for (uint i = 0; i < buttons_list.count(); i++)
		buttons_list.at(i)->setToggleButton(enable);
	
	isToggleBar = enable;
}

void ButtonsBar::setToggleStatus(unsigned int button_up_index)
{
	if (isToggleBar && button_up_index < buttons_list.count())
	{
		for (uint i = 0; i < buttons_list.count(); i++)
			buttons_list.at(i)->setOn(false);
		buttons_list.at(button_up_index)->setOn(true);
	}
}

void ButtonsBar::showButton(int idx)
{
	buttons_list[idx]->show();
}

void ButtonsBar::hideButton(int idx)
{
	buttons_list[idx]->hide();
}

void ButtonsBar::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	setBackgroundColor(c);
	buttons_group->setPaletteBackgroundColor(c);

	for (uint i = 0; i < buttons_list.size(); i++)
		buttons_list[i]->setPaletteBackgroundColor(c);

}
void ButtonsBar::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_group->setPaletteForegroundColor(c);

	for (uint i = 0; i < buttons_list.size(); i++)
		buttons_list[i]->setPaletteForegroundColor(c);
}


