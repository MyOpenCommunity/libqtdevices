#include "singlechoicecontent.h"
#include "bann1_button.h" // BannLeft
#include "btbutton.h"
#include "skinmanager.h"


class CheckableBanner : public BannLeft
{
public:
	CheckableBanner(const QString &text);

	BtButton *getButton();
};

CheckableBanner::CheckableBanner(const QString &text)
	: BannLeft(0)
{
	initBanner("", text);
	left_button->setCheckable(true);
	// TODO touch 10 uses 4 icons for togglable BtButton: normal,
	//      pressed normal, toggled, pressed toggled
	left_button->setImage(bt_global::skin->getImage("unchecked"));
	left_button->setPressedImage(bt_global::skin->getImage("checked"));
}

BtButton *CheckableBanner::getButton()
{
	return left_button;
}


SingleChoiceContent::SingleChoiceContent(QWidget *parent)
		: BannerContent(parent)
{
	buttons.setExclusive(true);

	connect(&buttons, SIGNAL(buttonClicked(int)),
		this, SIGNAL(bannerSelected(int)));
}

void SingleChoiceContent::addBanner(const QString &text, int id)
{
	CheckableBanner *bann = new CheckableBanner(text);
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
