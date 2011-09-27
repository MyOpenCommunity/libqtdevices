/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "alarmpage.h"
#include "navigation_bar.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "btbutton.h"
#include "skinmanager.h"
#include "pagestack.h"
#include "main.h" // ANTIINTRUSION
#include "labels.h" // ImageLabel
#include "generic_functions.h" // DateConversions::formatDateConfigShort

#include <QPixmap>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QDateTime>


#ifdef LAYOUT_TS_3_5

AlarmPage::AlarmPage(const QString &icon, const QString &d, const QString &zone, int id)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("alarm_del"));
	QWidget *content = new QWidget;
	buildPage(content, nav_bar);

	QString descr = d;

	QString hhmm = QTime::currentTime().toString("hh:mm");
	QString ddMM = DateConversions::formatDateConfigShort(QDate::currentDate(), '.');

	descr += QString("\n%1   %2    %3").arg(hhmm).arg(ddMM).arg(zone);

	QVBoxLayout *l = new QVBoxLayout(content);
	l->setContentsMargins(0, 20, 0, 30);
	l->setSpacing(10);
	image = new ImageLabel(icon);
	image->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	l->addWidget(image);

	l->addSpacing(35);
	description = new QLabel;
	description->setFont(bt_global::font->get(FontManager::TEXT));
	description->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	description->setText(descr);
	l->addWidget(description);

	l->addStretch();

	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), this, SIGNAL(nextAlarm()));
	connect(nav_bar, SIGNAL(downClick()), this, SIGNAL(prevAlarm()));
	connect(nav_bar, SIGNAL(forwardClick()), this, SIGNAL(deleteAlarm()));
	alarm_id = id;
}

#else

class AlarmPageData : public QWidget
{
public:
	AlarmPageData(QStringList captions, QStringList values);

private:
	QWidget *createPart(const QString &background, const QString &caption, const QString &value);
};

AlarmPageData::AlarmPageData(QStringList captions, QStringList values)
{
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	QLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addWidget(createPart("alarm_info_hour", captions[0], values[0]));
	l->addWidget(createPart("alarm_info_date", captions[1], values[1]));
	l->addWidget(createPart("alarm_info_zone", captions[2], values[2]));
}

QWidget *AlarmPageData::createPart(const QString &background, const QString &caption, const QString &value)
{
	QLabel *back = new QLabel;

	QVBoxLayout *l = new QVBoxLayout(back);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	back->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage(background)));

	QLabel *c = new QLabel(caption);
	c->setAlignment(Qt::AlignCenter);
	c->setFont(bt_global::font->get(FontManager::TEXT));

	QLabel *v = new QLabel(value);
	v->setAlignment(Qt::AlignCenter);
	v->setFont(bt_global::font->get(FontManager::TEXT));

	l->addWidget(c);
	l->addWidget(v);

	return back;
}


AlarmPage::AlarmPage(const QString &icon, const QString &description, const QString &zone, int id)
{
	QLabel *title = new QLabel(description);
	title->setAlignment(Qt::AlignHCenter);
	title->setFont(bt_global::font->get(FontManager::TITLE));

	QLabel *i = new QLabel;
	i->setPixmap(*bt_global::icons_cache.getIcon(icon));

	QDateTime time = QDateTime::currentDateTime();
	QWidget *d = new AlarmPageData(QStringList() << tr("Hour") << tr("Date") << tr("Zone"),
			QStringList() << time.toString("hh:mm") << DateConversions::formatDateConfigShort(QDate::currentDate())
			<< zone);

	BtButton *home = new BtButton(bt_global::skin->getImage("go_home"));
	BtButton *list = new BtButton(bt_global::skin->getImage("info"));
	BtButton *trash = new BtButton(bt_global::skin->getImage("alarm_del"));

	QGridLayout *l = new QGridLayout(this);
	l->setAlignment(Qt::AlignCenter);

	l->addWidget(i, 1, 0, 2, 1);
	l->addWidget(title, 0, 1, 1, 3);
	l->addWidget(d, 1, 1, 1, 3);

	l->addWidget(home, 2, 1, Qt::AlignHCenter);
	l->addWidget(list, 2, 2, Qt::AlignHCenter);
	l->addWidget(trash, 2, 3, Qt::AlignHCenter);

	connect(home, SIGNAL(clicked()), SIGNAL(showHomePage()));
	connect(list, SIGNAL(clicked()), SIGNAL(showAlarmList()));
	connect(trash, SIGNAL(clicked()), SIGNAL(deleteAlarm()));
	alarm_id = id;
}

#endif

int AlarmPage::alarmId() const
{
	return alarm_id;
}

void AlarmPage::showPage()
{
	bt_global::page_stack.showUserPage(this);
	Page::showPage();
}

void AlarmPage::cleanUp()
{
	// On ts10, we want to delete the alarm popup when we exit from the page
	// (because the alarm itself remains on the AlarmListPage). On ts3, we do not
	// to want this because the AlarmListPage is never shown.
#ifdef LAYOUT_TS_10
	deleteLater();
#endif
}

int AlarmPage::sectionId() const
{
	return ANTIINTRUSION;
}

