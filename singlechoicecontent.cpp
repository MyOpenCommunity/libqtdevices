#include "singlechoicecontent.h"
#include "btbutton.h"
#include "skinmanager.h"


CheckableBanner *SingleChoice::createBanner(const QString &text, const QString &right_icon)
{
	return new CheckableBanner(text, right_icon);
}

CheckableBanner::CheckableBanner(const QString &text, const QString &right_icon)
	: Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("unchecked"), right_icon, text);
	left_button->setCheckable(true);
	// TODO touch 10 uses 4 icons for togglable BtButton: normal,
	//      pressed normal, toggled, pressed toggled
	left_button->setPressedImage(bt_global::skin->getImage("checked"));
}

BtButton *CheckableBanner::getButton()
{
	return left_button;
}


SingleChoiceContent::SingleChoiceContent()
{
	buttons.setExclusive(true);

	connect(&buttons, SIGNAL(buttonClicked(int)),
		this, SIGNAL(bannerSelected(int)));
}

void SingleChoiceContent::addBanner(CheckableBanner *bann, int id)
{
	buttons.addButton(bann->getButton(), id);
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

QList<QAbstractButton*> SingleChoiceContent::getButtons()
{
	return buttons.buttons();
}
