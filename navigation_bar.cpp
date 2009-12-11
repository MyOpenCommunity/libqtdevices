#include "navigation_bar.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin

#include <QFileInfo>
#include <QGridLayout>
#include <QVBoxLayout>

static const int buttons_dim = 60;


NavigationBar::NavigationBar(QString forward_icon, QString down_icon, QString up_icon, QString back_icon, QWidget *parent)
{
	QGridLayout *l = new QGridLayout(this);
	if ((back_button = createButton(back_icon, SIGNAL(backClick()))))
		l->addWidget(back_button, 0, 0);
	if ((down_button = createButton(down_icon, SIGNAL(downClick()))))
		l->addWidget(down_button, 0, 1);
	if ((up_button = createButton(up_icon, SIGNAL(upClick()))))
		l->addWidget(up_button, 0, 2);
	if ((forward_button = createButton(forward_icon, SIGNAL(forwardClick()))))
		l->addWidget(forward_button, 0, 3);
	l->setColumnStretch(1, 1);
	l->setColumnStretch(2, 1);
	l->setColumnStretch(3, 1);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	main_layout = l;

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

