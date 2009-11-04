#include "singlechoicepage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "bann1_button.h" // bannOnSx
#include "navigation_bar.h"
#include "singlechoicecontent.h"
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage()
{
	buildPage(new SingleChoiceContent, new NavigationBar(ICON_OK));

	connect(this, SIGNAL(forwardClick()), SLOT(okPressed()));
}

void SingleChoicePage::addBanner(const QString &text, int id)
{
	page_content->addBanner(text, id);

	if (id == getCurrentId())
		setCheckedId(id);
}

void SingleChoicePage::setCheckedId(int id)
{
	page_content->setCheckedId(id);
}

void SingleChoicePage::okPressed()
{
	bannerSelected(page_content->checkedId()); // update the current id and do custom actions
	setCheckedId(getCurrentId());

	emit Closed();
}

