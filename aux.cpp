#include "aux.h"
#include "btbutton.h"
#include "main.h" // ICON_FFWD
#include "fontmanager.h" // bt_global::font

#include <QVBoxLayout>
#include <QLabel>


Aux::Aux(const QString &name, const QString &amb)
{
	main_layout->addSpacing(30);
	aux_name = new QLabel;
	aux_name->setFont(bt_global::font->get(FontManager::SUBTITLE));
	aux_name->setText(name);
	main_layout->addWidget(aux_name, 0, Qt::AlignHCenter);

	main_layout->addSpacing(30);
	amb_descr = new QLabel;
	amb_descr->setFont(bt_global::font->get(FontManager::SUBTITLE));
	amb_descr->setText(amb);
	main_layout->addWidget(amb_descr, 0, Qt::AlignHCenter);

	main_layout->addSpacing(20);
	fwd_but = new BtButton;
	fwd_but->setImage(ICON_FFWD);
	main_layout->addWidget(fwd_but, 0, Qt::AlignHCenter);

	addBackButton();
	connect(fwd_but, SIGNAL(clicked()), SIGNAL(Btnfwd()));
}

void Aux::setAmbDescr(const QString &d)
{
	amb_descr->setText(d);
}

