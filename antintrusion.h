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


#ifndef ANTINTRUSION_H
#define ANTINTRUSION_H

#include "frame_receiver.h"
#include "page.h"
#include "gridcontent.h"
#include "skinmanager.h" // SkinManager::CidState

#include <QString>
#include <QTimer>
#include <QList>
#include <QSignalMapper>

class impAnti;
class AntintrusionZone;
class BtButton;
class Keypad;
class AlarmPage;
class QDomNode;
class QWidget;
class AlarmList;
class QDateTime;


/*!
  \class Antintrusion
  \brief This is a class that contains the alarm-plant, the zone's list and the alarm queue.
  The plant is always kept on the top, the zones in the remaining lines. Alarm queue is active
  only when there's some alarm pending (and it's possible from \a antintrusione object to access
to \a alarm \a queue one though a button). When there's an alarm the alarm queue becomes automatically visible
*/
class Antintrusion : public BannerPage, FrameReceiver
{
Q_OBJECT
public:
	Antintrusion(const QDomNode &config_node);
	~Antintrusion();
	virtual void inizializza();
	virtual void manageFrame(OpenMsg &msg);

	virtual int sectionId();

	virtual void showPage();

public slots:
	void Parzializza();
	void Parz();
	void IsParz(bool);

/*!
  \brief if there are no allarms in the queue the button in the plant area which give the possibility to see the queue is hidden
*/
	void checkAlarmCount();
/*!
  \brief Invoked when next alarm must be displayed
*/
	void nextAlarm();
/*!
  \brief Invoked when prev alarm must be displayed
*/
	void prevAlarm();
/*!
  \brief Invoked when current alarm must be deleted
*/
	void deleteAlarm();
/*!
  \brief Invoked when alarm list must be shown
*/
	void showAlarms();
/*!
  \brief clear alarm list
*/
	void doClearAlarms();
	void request();

signals:
/*!
  \brief enable/disable area partialization enable events
*/
	void abilitaParz(bool ab);
/*!
  \brief part. changed events
*/
	void partChanged(AntintrusionZone *);
/*!
  \brief clear changed flags
*/
	void clearChanged();

private:
	void createImpianto(const QString &description);

private:
/*!
  \param <impianto> alarm plant
*/
	impAnti* impianto;
	QWidget* top_widget;
/*!
  \param <allarmi> alarm's queue
*/
	QList<AlarmPage*> allarmi;
	AlarmList *alarms;
	int curr_alarm;
/*!
  \param <alarmTexts[altype]> text for a given alarm
*/
	QString alarmTexts[4];
	QString zones[8];
	Keypad *tasti;
	static const int MAX_ZONE = 8;
	QTimer request_timer;
	BtButton *forward_button; // the forward button of the navigation bar
	SkinManager::CidState skin_cid;

	void loadItems(const QDomNode &config_node);

	void clearAlarms();
	void addAlarm(QString descr, int t, int zona);

private slots:
	void showHomePage();
	void requestZoneStatus();
	void requestStatusIfCurrentWidget(Page *curr);
	void plantInserted();
	void cleanupAlarmPage(QObject *page);
};


class AlarmItems : public GridContent
{
Q_OBJECT
public:
	AlarmItems();

	void addAlarm(int type, const QString &description, const QString &zone, const QDateTime &date);
	void removeAlarm(int index);
	int alarmCount();

	void drawContent();
	void prepareLayout();

private slots:
	void removeAlarm(QWidget *);

private:
	QList<QWidget*> alarms;
	QStringList icons;
	QSignalMapper mapper;
};


class AlarmList : public Page
{
Q_OBJECT
public:
	typedef AlarmItems ContentType;

	AlarmList();

	void addAlarm(int type, const QString &description, const QString &zone, const QDateTime &date);
	int alarmCount();

	virtual void activateLayout();
	virtual int sectionId();

private:
	AlarmItems *alarms;
};

#endif // ANTINTRUSION_H
