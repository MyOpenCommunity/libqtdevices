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

#include <QList>

class QString;
class QLabel;
class QDomNode;

/**
 * The message list
 */
class MessageList : public ItemList
{
public:
	MessageList(QWidget *parent, int rows_per_page);

protected:
	virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);
};

/**
 * The page used to ask if delete all the messages or not.
 */
class DeleteMessagesPage : public Page
{
Q_OBJECT
public:
	DeleteMessagesPage();
signals:
	void deleteAll();
};


/**
 * The page used to show a message.
 */
class MessagePage : public Page
{
Q_OBJECT
public:
	MessagePage();
	void setData(const QString &date, const QString &text, bool read);

signals:
	// Request to show the next message
	void nextMessage();
	// Request to show the previous message
	void prevMessage();
	// Request to delete the current message
	void deleteMessage();

private:
	QLabel *date_label, *message_label, *new_message_label;
	bool already_read;
};


/**
 * The page used to show an alarm message.
 */
class AlertMessagePage : public Page
{
Q_OBJECT
public:
	AlertMessagePage(const QString &date, const QString &text);
	virtual int sectionId() const;

public slots:
	virtual void cleanUp() { deleteLater(); }

signals:
	void goHome();
	void goMessagesList();
	void deleteMessage();
};


/**
 * This class show a list of scs messages, read from an xml file.
 */
class MessagesListPage : public Page
{
Q_OBJECT
public:
	MessagesListPage(const QDomNode &config_node);
	typedef MessageList ContentType;
	virtual int sectionId() const;

public slots:
	virtual void showPage();

private:
	MessagePage *message_page;
	DeleteMessagesPage *delete_page;
	PageTitleWidget *title;
	QList<AlertMessagePage *> alert_pages;

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
};


#endif

