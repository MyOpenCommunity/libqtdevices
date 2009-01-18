#include "singlechoicepage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bannondx.h"
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage()
{
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
{;
	bannerSelected(buttons.checkedId());
	emit Closed();
}

void SingleChoicePage::showEvent(QShowEvent *event)
{
	sottoMenu::showEvent(event);
	foreach (QAbstractButton *btn, buttons.buttons())
		if (buttons.id(btn) == getCurrentId())
			btn->setChecked(true);
}

