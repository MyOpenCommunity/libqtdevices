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

#ifndef MESSAGES_H
#define MESSAGES_H

#include "page.h"
#include "itemlist.h"
#include "skinmanager.h" // SkinManager::CidState
#include "state_button.h" // StateButton::Status

#include <QList>

class QString;
class QLabel;
class QDomNode;


class MessageList : public ItemList
{
public:
	MessageList(QWidget *parent, int rows_per_page);

protected:
	virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);
};


class DeleteMessagesPage : public Page
{
Q_OBJECT
public:
	DeleteMessagesPage();
signals:
	void deleteAll();
};


class MessagePage : public Page
{
Q_OBJECT
public:
	MessagePage();
	void setData(const QString &date, const QString &text, bool read);

signals:
	void nextMessage();
	void prevMessage();
	void deleteMessage();

private:
	QLabel *date_label, *message_label, *new_message_label;
	bool already_read;
};


class AlertMessagePage : public Page
{
Q_OBJECT
public:
	AlertMessagePage(const QString &date, const QString &text);
	virtual int sectionId() const;
	QString date() const { return _date; }
	QString text() const { return _text; }

public slots:
	virtual void cleanUp() { deleteLater(); }

signals:
	void goHome();
	void goMessagesList();
	void deleteMessage();

private:
	QString _date, _text;
};


class MessagesListPage : public Page
{
Q_OBJECT
public:
	MessagesListPage(const QDomNode &config_node);
	typedef MessageList ContentType;
	virtual int sectionId() const;

public slots:
	virtual void showPage();

signals:
	void changeIconState(StateButton::Status);

private:
	MessagePage *message_page;
	DeleteMessagesPage *delete_page;
	PageTitleWidget *title;
	QList<AlertMessagePage *> alert_pages;

	bool unread_messages; // true if there is at least one message unread
	int current_index;
	bool need_update;
	SkinManager::CidState skin_cid; // required to save/restore the cid, when a new message arrives.

	void loadMessages(const QString &filename);
	void saveMessages();
	void setMessageAsRead(int index, bool read = true);

private slots:
	void newMessage(const DeviceValues &values_list);
	void showMessage(int index);

	// Delete all the messages
	void deleteAll();
	// Delete a single message
	void deleteMessage();
	void deleteAlertMessage();

	void showPrevMessage();
	void showNextMessage();

	// Slots for the AlarmMessagePage
	void goHome();
	void goMessagesList();

	void checkForUnread();
};


#endif

