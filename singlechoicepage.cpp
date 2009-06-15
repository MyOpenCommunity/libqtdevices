#include "singlechoicepage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bann1_button.h" // bannOnSx
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage(bool paging)
{
	if (paging)
		setNavBarMode(4, ICON_OK);
	else
		setNavBarMode(10, ICON_OK);

	buttons.setExclusive(true);

	connect(this, SIGNAL(goDx()), SLOT(okPressed()));
}

void SingleChoicePage::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	BtButton *btn = bann->getButton();
	btn->setCheckable(true);
	bann->setText(text);
	buttons.addButton(btn, id);
	elencoBanner.append(bann);

	if (id == getCurrentId())
		btn->setChecked(true);
}

void SingleChoicePage::okPressed()
{
	bannerSelected(buttons.checkedId()); // update the current id and do custom actions
	foreach (QAbstractButton *btn, buttons.buttons())
		if (buttons.id(btn) == getCurrentId())
			btn->setChecked(true);

	emit Closed();
}

