#include "singlechoicepage.h"
#include "navigation_bar.h"
#include "singlechoicecontent.h"
#include "btbutton.h"

#include <QDebug>


SingleChoicePage::SingleChoicePage()
{
#ifdef LAYOUT_BTOUCH
	buildPage(new SingleChoiceContent, new NavigationBar("ok"));
#else
	buildPage(new SingleChoiceContent, new NavigationBar);
#endif

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
	confirmSelection();
	emit Closed();
}

void SingleChoicePage::confirmSelection()
{
	bannerSelected(page_content->checkedId()); // update the current id and do custom actions
	setCheckedId(getCurrentId());
}
