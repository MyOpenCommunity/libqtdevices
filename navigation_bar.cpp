#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin

#include <QFileInfo>

static const int buttons_dim = 60;


NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	back_button = createButton(back_icon, SIGNAL(backClick()), 0);
	down_button = createButton(down_icon, SIGNAL(downClick()), 1);
	up_button = createButton(up_icon, SIGNAL(upClick()), 2);
	forward_button = createButton(forward_icon, SIGNAL(forwardClick()), 3);
}

BtButton *NavigationBar::createButton(QString icon, const char *signal, int pos)
{
	if (!icon.isNull())
	{
		BtButton *b = new BtButton(this);
		connect(b, SIGNAL(clicked()), signal);
		if (QFileInfo(icon).exists())
			b->setImage(icon); // for retrocompatibility
		else
			b->setImage(bt_global::skin->getImage(icon));
		b->setGeometry(buttons_dim * pos, 0, buttons_dim, buttons_dim);
		return b;
	}
	return 0;
}

QSize NavigationBar::sizeHint() const
{
	return QSize(buttons_dim * 4, buttons_dim);
}

void NavigationBar::displayScrollButtons(bool display)
{
	down_button->setVisible(display);
	up_button->setVisible(display);
}

