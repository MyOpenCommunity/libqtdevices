#include "singlechoicepage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bann1_button.h" // bannOnSx
#include "navigation_bar.h"
#include "content_widget.h"
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage()
{
	buildPage(new ContentWidget, new NavigationBar(ICON_OK));
	buttons.setExclusive(true);

	connect(this, SIGNAL(forwardClick()), SLOT(okPressed()));
}

void SingleChoicePage::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	BtButton *btn = bann->getButton();
	btn->setCheckable(true);
	bann->setText(text);
	bann->Draw();
	buttons.addButton(btn, id);
	content_widget->appendBanner(bann);

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

