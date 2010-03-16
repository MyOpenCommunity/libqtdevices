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

#include <QList>

class QString;
class QLabel;


class MessagePage : public Page
{
Q_OBJECT
public:
	MessagePage();
	void setData(const QString &date, const QString &text, bool read);

signals:
	void nextMessage();
	void prevMessage();

private:
	QLabel *date_label, *message_label, *new_message_label;
	bool already_read;
};


/**
 * This class show a list of scs messages, read from an xml file.
 */
class MessagesListPage : public Page
{
Q_OBJECT
public:
	MessagesListPage();
	typedef ItemList ContentType;
	virtual int sectionId();

public slots:
	virtual void showPage();

private:
	MessagePage *message_page;
	int current_index;
	bool need_update;
	void loadMessages(const QString &filename);

private slots:
	void showMessage(int index);
	void deleteAll();
	void showPrevMessage();
	void showNextMessage();
};


#endif

