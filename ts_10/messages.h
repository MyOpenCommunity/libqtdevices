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
class QScrollArea;


/*!
	\ingroup Messages
	\def MESSAGES_MAX
	The maximum number of messages saved.
*/
#define MESSAGES_MAX 10

/*!
	\ingroup Messages
	\def MESSAGES_FILENAME
	The file where the messages are stored.
*/
#define MESSAGES_FILENAME "cfg/extra/4/messages.xml"

/*!
	\defgroup Messages Messages

	The section allows the user to read and delete messages coming from
	the Guard Unit.
	A popup is displayed using the AlertMessagePage class when a new message
	arrives. The message is alos appended to the list of messages and saved in the
	MESSAGES_FILENAME file, up to MESSAGES_MAX.

	The MessagesListPage is used to display the list of messages while the
	MessagePage displays a single message and the DeleteMessagesPage can be
	used to delete all the messages.
	The icons for the section, displayed in the homepage and in the toolbar box,
	will change to signal when there are unread messages.
*/


/*!
	\ingroup Messages
	\brief The content of the page that shows a message list.
*/
class MessageList : public ItemList
{
public:
	MessageList(QWidget *parent, int rows_per_page);

protected:
	virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);
};


/*!
	\ingroup Messages
	\brief The page used to ask if delete all the messages or not.
*/
class DeleteMessagesPage : public Page
{
Q_OBJECT
public:
	DeleteMessagesPage();
signals:
	/*!
		\brief Request to delete all the messages.
	*/
	void deleteAll();
};


/*!
	\ingroup Messages
	\brief The content of a MessagePage / AlertMessagePage.

	Contains the logic to scroll up/down the content, return to the home page
	or to the message list, delete a single message and change the message itself.
*/
class MessageContent : public QWidget
{
Q_OBJECT
public:
	MessageContent();
	/*!
		\brief Show or hide the button to return to the home page.
		\sa goHome()
	*/
	void showHomeButton(bool show);

	/*!
		\brief Show or hide the button to return to the message list.
		\sa goMessagesList()
	*/
	void showMessageListButton(bool show);

	/*!
		\brief Show or hide the label that notifies that the message is new.
	*/
	void showNewMessageLabel(bool show);

	/*!
		\brief Show or hide the button to go to the next message.
	*/
	void showNextButton(bool show);

	/*!
		\brief Show or hide the button to go to the previous message.
	*/
	void showPrevButton(bool show);

	/*!
		\brief Set the date of the message displayed.
	*/
	void setDate(const QString &date);

	/*!
		\brief Set the text of the message displayed.
	*/
	void setMessage(const QString &message);

public slots:
	/*!
		\brief Scroll up the content of the message, if the message is too long.
	*/
	void scrollUp();

	/*!
		\brief Scroll down the content of the message, if the message is too long.
	*/
	void scrollDown();

signals:
	/*!
		\brief Emitted when the user clicks the home button.
		\sa showHomeButton()
	*/
	void goHome();

	/*!
		\brief Emitted when the user clicks the message list button.
		\sa showMessageListButton()
	*/
	void goMessagesList();

	/*!
		\brief Emitted when the user clicks the delete button.
	*/
	void deleteMessage();

	/*!
		\brief Emitted when the user clicks the previous button.
		\sa showPrevButton()
	*/
	void prevMessage();

	/*!
		\brief Emitted when the user clicks the next button.
		\sa showNextButton()
	*/
	void nextMessage();

private:
	BtButton *go_home_button, *go_message_list_button, *prev_button, *next_button;
	QLabel *date_label, *message_label, *new_message_label;
	QScrollArea *text_area;
};



/*!
	\ingroup Messages
	\brief Shows an scs message.
*/
class MessagePage : public Page
{
Q_OBJECT
public:

	typedef MessageContent ContentType;

	MessagePage();

	/*!
		\brief Set the content of message displayed from the page.
	*/
	void setData(const QString &date, const QString &text, bool read);

signals:
	/*!
		\brief Request to show the next message.
	*/
	void nextMessage();

	/*!
		\brief Request to show the previous message.
	*/
	void prevMessage();

	/*!
		\brief Request to delete the current message.
	*/
	void deleteMessage();

private:
	QLabel *date_label, *message_label, *new_message_label;
	bool already_read;
};


/*!
	\ingroup Messages
	\brief The popup page for an scs message.
*/
class AlertMessagePage : public Page
{
Q_OBJECT
public:
	typedef MessageContent ContentType;

	AlertMessagePage(const QString &date, const QString &text);
	virtual int sectionId() const;

	/*!
		\brief Return the message date.
	*/
	QString date() const { return _date; }

	/*!
		\brief Return the message text.
	*/
	QString text() const { return _text; }

public slots:
	virtual void cleanUp() { deleteLater(); }

signals:
	/*!
		\brief Request to display the homepage.
	*/
	void goHome();

	/*!
		\brief Request to display the messages list.
	*/
	void goMessagesList();

	/*!
		\brief Request to delete the message.
	*/
	void deleteMessage();

private:
	QString _date, _text;
};


/*!
	\ingroup Messages
	\brief Shows a list of scs messages, read from an xml file.

	This class loads the list of messages and handles the requests performed
	by the other \ref Messages classes. It also change the state of the icon displayed
	in the homepage and in the toolbar box.
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
	void cleanUpAlert();

	void checkForUnread();

};


#endif

