#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin
#include "hardware_functions.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>

static const int buttons_dim = 60;


NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	if (hardwareType() == TOUCH_X)
	{
		main_layout = new QVBoxLayout(this);
		main_layout->addStretch(1);
		up_button = createButton(up_icon, SIGNAL(upClick()));
		down_button = createButton(down_icon, SIGNAL(downClick()));
		back_button = createButton(back_icon, SIGNAL(backClick()));
	}
	else
	{
		main_layout = new QHBoxLayout(this);
		back_button = createButton(back_icon, SIGNAL(backClick()));
		down_button = createButton(down_icon, SIGNAL(downClick()));
		up_button = createButton(up_icon, SIGNAL(upClick()));
		forward_button = createButton(forward_icon, SIGNAL(forwardClick()));
		main_layout->addStretch(1);
	}

	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
}

BtButton *NavigationBar::createButton(QString icon, const char *signal)
{
	if (!icon.isNull())
	{
		BtButton *b = new BtButton(this);
		connect(b, SIGNAL(clicked()), signal);
		if (QFileInfo(icon).exists())
			b->setImage(icon); // for retrocompatibility
		else
			b->setImage(bt_global::skin->getImage(icon));
		main_layout->addWidget(b);
		return b;
	}
	return 0;
}

QSize NavigationBar::sizeHint() const
{
	if (hardwareType() == TOUCH_X)
		return QSize(75, 355);
	else
		return QSize(buttons_dim * 4, buttons_dim);
}

void NavigationBar::displayScrollButtons(bool display)
{
	down_button->setVisible(display);
	up_button->setVisible(display);
}

