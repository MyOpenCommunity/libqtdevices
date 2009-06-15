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
