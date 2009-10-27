#include "singlechoicecontent.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bann1_button.h" // bannOnSx
#include "btbutton.h"

SingleChoiceContent::SingleChoiceContent(QWidget *parent)
		: ContentWidget(parent)
{
	buttons.setExclusive(true);

	connect(&buttons, SIGNAL(buttonClicked(int)),
		this, SIGNAL(bannerSelected(int)));
}

void SingleChoiceContent::addBanner(const QString &text, int id)
{
	bannOnSx *bann = new bannOnSx(this, ICON_VUOTO);
	BtButton *btn = bann->getButton();
	btn->setCheckable(true);
	bann->setText(text);
	bann->Draw();
	buttons.addButton(btn, id);
	appendBanner(bann);
}

int SingleChoiceContent::checkedId() const
{
	return buttons.checkedId();
}

void SingleChoiceContent::setCheckedId(int id)
{
	buttons.button(id)->setChecked(true);
}
