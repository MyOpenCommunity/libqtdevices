#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin

#include <QBoxLayout>
#include <QFileInfo>


NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	main_layout = new QHBoxLayout(this);
	addButton(back_icon, SIGNAL(backClick()));
	addButton(down_icon, SIGNAL(downClick()));
	addButton(up_icon, SIGNAL(upClick()));
	addButton(forward_icon, SIGNAL(forwardClick()));
	main_layout->addStretch();
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
}

void NavigationBar::addButton(QString icon, const char *signal)
{
	if (!icon.isNull())
	{
		BtButton *b = new BtButton;
		connect(b, SIGNAL(clicked()), signal);
		if (QFileInfo(icon).exists())
			b->setImage(icon); // for retrocompatibility
		else
			b->setImage(bt_global::skin->getImage(icon));
		main_layout->addWidget(b);
	}
}

