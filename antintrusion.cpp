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


#include "antintrusion.h"
#include "keypad.h"
#include "bann_antintrusion.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "alarmpage.h"
#include "btmain.h" // bt_global::btmain
#include "bannercontent.h"
#include "navigation_bar.h"
#include "btbutton.h"
#include "main.h"
#include "skinmanager.h"
#include "icondispatcher.h"

#include <openmsg.h>

#include <QDateTime>
#include <QDebug>
#include <QDomNode>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalMapper>


enum
{
	ITEM_ANTINTRUSION_ZONE = 13001,
};

Antintrusion::Antintrusion(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	skin_cid = bt_global::skin->getCidState();

	tasti = NULL;
	impianto = 0;
	forward_button = 0;

	alarmTexts[0] = tr("technical");
	alarmTexts[1] = tr("intrusion");
	alarmTexts[2] = tr("tamper");
	alarmTexts[3] = tr("anti-panic");

	top_widget = new QWidget;

	alarms = new AlarmList;
	connect(alarms, SIGNAL(Closed()), SLOT(showPage()));

#ifdef LAYOUT_BTOUCH
	// TODO: we introduce a double dependency to customize the image of the forward
	// button and to obtain a reference of it (to show/hide the button).
	// We can do better!

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("partial"));
	buildPage(new BannerContent, nav_bar, QString(), top_widget);
	forward_button = nav_bar->forward_button;
#else
	buildPage(getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT, top_widget);
#endif

	connect(this, SIGNAL(abilitaParz(bool)), SLOT(IsParz(bool)));
	connect(this, SIGNAL(forwardClick()), SLOT(Parzializza()));

	curr_alarm = -1;
	loadItems(config_node);
	Q_ASSERT_X(impianto, "Antintrusion::Antintrusion", "Impianto not found on the configuration file!");
	connect(this, SIGNAL(Closed()), SLOT(requestZoneStatus()));
	connect(bt_global::btmain, SIGNAL(startscreensaver(Page*)),
			SLOT(requestStatusIfCurrentWidget(Page*)));
	subscribe_monitor(5);

	checkAlarmCount();
}

int Antintrusion::sectionId()
{
	return ANTIINTRUSIONE;
}

void Antintrusion::createImpianto(const QString &descr)
{
	// We have to use a layout for the top_widget, in order to define an appropriate
	// sizeHint (needed by the main layout added to the Page)
	// An alternative is to define a custom widget that rimplement the sizeHint method.
	QHBoxLayout *l = new QHBoxLayout(top_widget);
	l->setSpacing(10);
	int mleft, mright;
	page_content->layout()->getContentsMargins(&mleft, NULL, &mright, NULL);
	l->setContentsMargins(mleft, 0, mright, 10);

	impianto = new impAnti(top_widget,
			       bt_global::skin->getImage("on"),
			       bt_global::skin->getImage("off"),
			       bt_global::skin->getImage("info"),
			       bt_global::skin->getImage("alarm_state"));
	impianto->setText(descr);
	impianto->Draw();
#ifdef CONFIG_BTOUCH
	impianto->setId(IMPIANTINTRUS); // can probably be removed
#endif
	l->addWidget(impianto);

	connect(impianto, SIGNAL(impiantoInserito()), SLOT(plantInserted()));
	connect(impianto, SIGNAL(abilitaParz(bool)), SIGNAL(abilitaParz(bool)));
	connect(impianto, SIGNAL(clearChanged()), SIGNAL(clearChanged()));
	connect(impianto, SIGNAL(pageClosed()), SLOT(showPage()));
	connect(impianto, SIGNAL(sxClick()), SLOT(showAlarms()));

	connect(this, SIGNAL(partChanged(AntintrusionZone*)), impianto, SLOT(partChanged(AntintrusionZone*)));

#ifdef LAYOUT_TOUCHX
	forward_button = new BtButton;
	forward_button->setImage(bt_global::skin->getImage("partial"));
	// remove the alignment after rewriting impAnti as a new style banner
	l->addWidget(forward_button, 0, Qt::AlignTop);
	connect(forward_button, SIGNAL(clicked()), SIGNAL(forwardClick()));
#endif
}

void Antintrusion::loadItems(const QDomNode &config_node)
{
#ifndef CONFIG_BTOUCH
	createImpianto("");
#endif

	int zone_count = 0;
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");

		banner *b;

#ifdef CONFIG_BTOUCH
		if (id == IMPIANTINTRUS)
			createImpianto(descr);
		else
		if (id == ZONANTINTRUS)
#endif
		if (id == ITEM_ANTINTRUSION_ZONE)
		{
			zones[zone_count] = descr;
			b = new AntintrusionZone(descr, getTextChild(item, "where"));
#if 0
			b = new zonaAnti(this, descr, getTextChild(item, "where"),
					 bt_global::skin->getImage("zone"),
					 bt_global::skin->getImage("alarm_off"),
					 bt_global::skin->getImage("alarm_on"));
			b->setText(descr);
			b->setId(id);
			b->Draw();
#endif
			page_content->appendBanner(b);
			connect(this, SIGNAL(abilitaParz(bool)), b, SLOT(abilitaParz(bool)));
			connect(this, SIGNAL(clearChanged()), b, SLOT(clearChanged()));
			connect(b, SIGNAL(partChanged(AntintrusionZone*)), SIGNAL(partChanged(AntintrusionZone*)));
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
			// We assume that the antintrusion impianto came before all the zones
			Q_ASSERT_X(impianto, "Antintrusion::loadItems", "Found a zone before the impianto!");
			impianto->setZona((AntintrusionZone *)b);

			++zone_count;
		}
		else
			Q_ASSERT_X(false, "Antintrusion::loadItems", qPrintable(QString("Type of item %1 not handled!").arg(id)));
	}
}

void Antintrusion::showPage()
{
	checkAlarmCount();
	Page::showPage();
}

void Antintrusion::plantInserted()
{
	clearAlarms();
}

Antintrusion::~Antintrusion()
{
	clearAlarms();
}

void Antintrusion::IsParz(bool ab)
{
	qDebug("antintrusione::IsParz(%d)", ab);

	if (ab)
		forward_button->show();
	else
		forward_button->hide();
}

void Antintrusion::Parzializza()
{
	qDebug("antintrusione::Parzializza()");
	int s[MAX_ZONE];
	for (int i = 0; i < MAX_ZONE; i++)
		s[i] = impianto->getIsActive(i);

	if (tasti)
		delete tasti;
	tasti = new KeypadWithState(s);
	connect(tasti, SIGNAL(Closed()), SLOT(Parz()));
	connect(tasti, SIGNAL(Closed()), SLOT(showPage()));
	tasti->setMode(Keypad::HIDDEN);
	tasti->showPage();
}

void Antintrusion::Parz()
{
	qDebug("antintrusione::Parz()");
	QString pwd = tasti->getText();
	if (!pwd.isEmpty())
	{
		QString f = "*5*50#" + pwd + "#";
		for (int i = 0; i < MAX_ZONE; i++)
			f += impianto->getIsActive(i) ? "0" : "1";
		f += "*0##";
		qDebug() << "sending part frame" << f;
		sendFrame(f);
		impianto->ToSendParz(false);
	}
	connect(&request_timer, SIGNAL(timeout()), SLOT(request()));
	request_timer.start(5000);
}

void Antintrusion::checkAlarmCount()
{
	qDebug("checkAlarmCount %d %d", allarmi.size(), alarms->alarmCount());
	// the first condition is for BTouch, the second for TouchX
	if (!allarmi.isEmpty() || alarms->alarmCount() != 0)
		impianto->mostra(BannerOld::BUT1);
	else
		impianto->nascondi(BannerOld::BUT1);
}

void Antintrusion::inizializza()
{
	BannerPage::inizializza();

	impianto->inizializza();
	connect(impianto, SIGNAL(clearAlarms()), this, SLOT(doClearAlarms()));
}

void Antintrusion::manageFrame(OpenMsg &msg)
{
	int what = msg.what();

	if (what == 12 || what == 15 || what == 16 || what == 17)
	{
		AlarmPage::altype t = AlarmPage::TECNICO;

		if (what == 12)
			t = AlarmPage::TECNICO;
		else if (what == 15)
			t = AlarmPage::INTRUSIONE;
		else if (what == 16)
			t = AlarmPage::MANOMISSIONE;
		else if (what == 17)
			t = AlarmPage::PANIC;

		int zona = QString(msg.Extract_dove()).mid(1).toInt();

		addAlarm(alarmTexts[t], t, zona);
	}
}

void Antintrusion::addAlarm(QString descr, int t, int zona)
{
	bt_global::skin->setCidState(skin_cid);

	QString alarm_description = descr;
	QString zone_description;
	if (t == AlarmPage::TECNICO)
		zone_description = QString("AUX:%1").arg(zona);
	else
		zone_description = zones[zona - 1];

	QDateTime now = QDateTime::currentDateTime();

	allarmi.append(new AlarmPage(static_cast<AlarmPage::altype>(t), alarm_description, zone_description, now));
	// The current alarm is the last alarm inserted
	curr_alarm = allarmi.size() - 1;

	AlarmPage *curr = allarmi.at(curr_alarm);
	connect(curr, SIGNAL(Next()), SLOT(nextAlarm()));
	connect(curr, SIGNAL(Prev()), SLOT(prevAlarm()));
	connect(curr, SIGNAL(Delete()), SLOT(deleteAlarm()));
	connect(curr, SIGNAL(showHomePage()), SLOT(showHomePage()));
	connect(curr, SIGNAL(showAlarmList()), SLOT(showAlarms()));
	connect(curr, SIGNAL(destroyed(QObject*)), SLOT(cleanupAlarmPage(QObject*)));

	alarms->addAlarm(t, alarm_description, zone_description, now);

	curr->showPage();
	checkAlarmCount();
}

void Antintrusion::showHomePage()
{
	doClearAlarms();
	bt_global::btmain->showHomePage();
}

void Antintrusion::nextAlarm()
{
	qDebug("antiintrusione::nextAlarm()");
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::nextAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));

	if (++curr_alarm >= allarmi.size())
		curr_alarm = 0;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::prevAlarm()
{
	qDebug("antiintrusione::prevAlarm()");
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::prevAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));
	if (--curr_alarm < 0)
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::deleteAlarm()
{
	Q_ASSERT_X(curr_alarm >= 0 && curr_alarm < allarmi.size(), "Antintrusion::deleteAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(curr_alarm).arg(allarmi.size())));
	allarmi.takeAt(curr_alarm)->deleteLater();

	if (allarmi.isEmpty())
	{
		curr_alarm = -1;
		// TODO the delay is probably not needed anymore
		QTimer::singleShot(150, this, SLOT(checkAlarmCount()));
		return;
	}
	else if (curr_alarm >= allarmi.size())
		curr_alarm = allarmi.size() - 1;

	allarmi.at(curr_alarm)->showPage();
}

void Antintrusion::cleanupAlarmPage(QObject *page)
{
	allarmi.removeOne(static_cast<AlarmPage*>(page));
}

#ifdef LAYOUT_BTOUCH

void Antintrusion::showAlarms()
{
	qDebug("antiintrusione::showAlarms()");
	if (allarmi.isEmpty())
		return;

	curr_alarm = allarmi.size() - 1;
	allarmi.at(curr_alarm)->showPage();
}

#else

void Antintrusion::showAlarms()
{
	doClearAlarms();
	alarms->showPage();
}

#endif

void Antintrusion::doClearAlarms()
{
	clearAlarms();
}

void Antintrusion::clearAlarms()
{
	qDebug("antiintrusione::clearAlarms()");
	while (!allarmi.isEmpty())
	{
		AlarmPage *a = allarmi.takeFirst();
		a->deleteLater();
	}
}

void Antintrusion::requestZoneStatus()
{
	qDebug("Request zone status");
	for (int i = 1; i <= 8; ++i)
		sendFrame(QString("*#5*#%1##").arg(i));
}

void Antintrusion::request()
{
	request_timer.stop();
	disconnect(&request_timer, SIGNAL(timeout()), this, SLOT(request()));
	sendInit("*#5*0##");
}

void Antintrusion::requestStatusIfCurrentWidget(Page *curr)
{
	// We can't use currentPage() because at the time when startscreensaver
	// is emitted the currentPage is the screensaver page.
	if (curr == this)
		requestZoneStatus();
}


// keep the same order as the altype enum in alarmpage.h
static const char *alarm_icons[] = { "technic_alarm", "intrusion_alarm", "tamper_alarm", "panic_alarm" };

AlarmItems::AlarmItems()
{
	for (int i = 0; i < 4; ++i)
		icons.append(bt_global::skin->getImage(alarm_icons[i]));

	connect(&mapper, SIGNAL(mapped(QWidget *)), SLOT(removeAlarm(QWidget *)));
}

void AlarmItems::addAlarm(int type, const QString &description, const QString &zone, const QDateTime &date)
{
	QWidget *alarm = new QWidget(this);
	QHBoxLayout *l = new QHBoxLayout(alarm);

	// alarm type icon
	QLabel *icon = new QLabel;
	icon->setPixmap(*bt_global::icons_cache.getIcon(icons[type]));

	// alarm description
	QLabel *s = new QLabel(description);
	s->setAlignment(Qt::AlignTop|Qt::AlignLeft);

	// alarm zone
	QLabel *z = new QLabel(zone);
	z->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

	QLabel *d = new QLabel(date.toString("dd/MM/yyyy\nhh:mm:ss"));
	d->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

	// delete button
	BtButton *trash = new BtButton;
	trash->setImage(bt_global::skin->getImage("alarm_del"));

	l->addWidget(icon);
	l->addWidget(s, 1);
	l->addWidget(z, 1);
	l->addWidget(d, 1);
	l->addWidget(trash);

	alarms.append(alarm);
	mapper.setMapping(trash, alarm);
	connect(trash, SIGNAL(clicked()), &mapper, SLOT(map()));

	need_update = true;
	if (isVisible())
	{
		prepareLayout();
		updateLayout(alarms);
	}
}

void AlarmItems::removeAlarm(QWidget *item)
{
	// TODO after removing the alarm, needs to:
	// - close the alarm list page if it was the last alarm
	// - hide the link to the alarm list page in the parent page
	int i = 0;
	foreach (QWidget *a, alarms)
	{
		if (a == item)
		{
			removeAlarm(i);
			break;
		}

		++i;
	}
}

void AlarmItems::removeAlarm(int index)
{
	alarms[index]->hide();
	alarms[index]->deleteLater();
	alarms.removeAt(index);

	need_update = true;
	if (isVisible())
		prepareLayout();
	if (current_page >= pageCount())
		current_page = pageCount() - 1;

	if (isVisible())
		updateLayout(alarms);
}

void AlarmItems::prepareLayout()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());
	QLayoutItem *child;

	while ((child = l->takeAt(0)) != 0)
		if (QWidget *w = child->widget())
			w->hide();
	pages.clear();

	GridContent::prepareLayout(alarms, 1);

	// add alarms to the layout
	for (int i = 0; i < pages.size() - 1; ++i)
	{
		int base = pages[i];
		for (int j = 0; base + j < pages[i + 1]; ++j)
			l->addWidget(alarms.at(base + j), j, 0);
	}

	l->setRowStretch(l->rowCount(), 1);
}

void AlarmItems::drawContent()
{
	if (need_update)
		prepareLayout();
	updateLayout(alarms);
}

int AlarmItems::alarmCount()
{
	return alarms.count();
}


AlarmList::AlarmList()
{
	QWidget *header = new QWidget;
	QHBoxLayout *l = new QHBoxLayout(header);

	QLabel *t = new QLabel(tr("Alarm type"));
	t->setAlignment(Qt::AlignHCenter);
	l->addWidget(t, 1);

	QLabel *z = new QLabel(tr("Zone"));
	z->setAlignment(Qt::AlignHCenter);
	l->addWidget(z, 1);

	QLabel *d = new QLabel(tr("Date & Hour"));
	d->setAlignment(Qt::AlignLeft);
	l->addWidget(d, 1);

	alarms = new AlarmItems;

	PageTitleWidget *title_widget = 0;
#ifdef LAYOUT_TOUCHX
	title_widget = new PageTitleWidget(tr("Alarms"), SMALL_TITLE_HEIGHT);

	connect(alarms, SIGNAL(contentScrolled(int, int)),
		title_widget, SLOT(setCurrentPage(int, int)));
#endif
	NavigationBar *nav_bar = new NavigationBar;
	buildPage(alarms, nav_bar, header, title_widget);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), alarms, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(upClick()), alarms, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), alarms, SLOT(pgDown()));
	connect(alarms, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

int AlarmList::sectionId()
{
	return ANTIINTRUSIONE;
}

void AlarmList::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	Page::activateLayout();

	if (page_content)
		page_content->drawContent();
}

void AlarmList::addAlarm(int type, const QString &description, const QString &zone, const QDateTime &date)
{
	alarms->addAlarm(type, description, zone, date);
}

int AlarmList::alarmCount()
{
	return alarms->alarmCount();
}
