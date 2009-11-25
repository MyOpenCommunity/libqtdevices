#include "alarmpage.h"
#include "navigation_bar.h"
#include "main.h" // IMG_PATH
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache

#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>


AlarmPage::AlarmPage(const QString &name, char *indirizzo, QString icon, altype t)
{
	NavigationBar *nav_bar = new NavigationBar(icon);
	QWidget *content = new QWidget;
	buildPage(content, nav_bar);

	type = t;

	QString icon_name;
	switch (type)
	{
	case AlarmPage::TECNICO:
		icon_name = IMG_PATH "imgalltec.png";
		break;
    default:
		icon_name = IMG_PATH "imgallintr.png";
		break;
	}

	QVBoxLayout *l = new QVBoxLayout(content);
	l->setContentsMargins(0, 20, 0, 30);
	l->setSpacing(10);
	image = new QLabel;
	image->setPixmap(*bt_global::icons_cache.getIcon(icon_name));
	image->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	l->addWidget(image);

	description = new QLabel;
	description->setFont(bt_global::font->get(FontManager::TEXT));
	description->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	description->setText(name);
	l->addWidget(description);

	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), this, SIGNAL(Prev()));
	connect(nav_bar, SIGNAL(downClick()), this, SIGNAL(Next()));
	connect(nav_bar, SIGNAL(forwardClick()), this, SIGNAL(Delete()));
}

