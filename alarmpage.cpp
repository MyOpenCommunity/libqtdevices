#include "alarmpage.h"
#include "navigation_bar.h"
#include "main.h" // IMG_PATH
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "btbutton.h"

#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>


#ifdef LAYOUT_BTOUCH

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

#else

class AlarmPageData : public QLabel
{
public:
	AlarmPageData(QStringList captions, QStringList values);

protected:
	virtual void paintEvent(QPaintEvent *e);

private:
	QStringList captions, values;
};

AlarmPageData::AlarmPageData(QStringList _captions, QStringList _values)
{
	captions = _captions;
	values = _values;

	setPixmap(*bt_global::icons_cache.getIcon(IMG_PATH "home/alarmpopupinfo.png"));
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void AlarmPageData::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);

	QPainter p(this);

	int w = width() / 3, h = height() / 2;

	for (int i = 0; i< 3; ++i)
		p.drawText(QRect(w * i, 0, w, h), Qt::AlignCenter, captions[i]);

	for (int i = 0; i< 3; ++i)
		p.drawText(QRect(w * i, h, w, h), Qt::AlignCenter, values[i]);
}

AlarmPage::AlarmPage(const QString &name, char *indirizzo, QString icon, altype t)
{
	QLabel *title = new QLabel(tr("Technical"));
	title->setAlignment(Qt::AlignHCenter);
	title->setFont(bt_global::font->get(FontManager::TITLE));

	QLabel *i = new QLabel;
	i->setPixmap(*bt_global::icons_cache.getIcon(IMG_PATH "home/imgallarmeantintrusione.png"));

	QLabel *d = new AlarmPageData(QStringList() << "Hour" << "Date" << "Zone",
				      QStringList() << "15:47" << "18/01" << "Z 5");

	BtButton *home = new BtButton;
	home->setImage(IMG_PATH "home/home.png");

	BtButton *list = new BtButton;
	list->setImage(IMG_PATH "antintrusione/storicoallarmi.png");

	BtButton *trash = new BtButton;
	trash->setImage(IMG_PATH "antintrusione/elimina.png");

	QGridLayout *l = new QGridLayout(this);
	l->setAlignment(Qt::AlignCenter);

	l->addWidget(i, 1, 0, 2, 1);
	l->addWidget(title, 0, 1, 1, 3);
	l->addWidget(d, 1, 1, 1, 3);

	l->addWidget(home, 2, 1);
	l->addWidget(list, 2, 2);
	l->addWidget(trash, 2, 3);
}

#endif
