#include "messages.h"
#include "itemlist.h"
#include "navigation_bar.h"
#include "main.h"
#include "generic_functions.h" //  DateConversions::formatDateTimeConfig, getDateTimeConfig
#include "xml_functions.h" // getChildren
#include "fontmanager.h"
#include "btbutton.h"
#include "message_device.h"
#include "devices_cache.h"
#include "btmain.h" // showHomePage
#include "displaycontrol.h" // makeActive
#include "pagestack.h"
#include "audiostatemachine.h" // bt_global::audio_states
#include "ringtonesmanager.h" // bt_global::ringtones

#include <QLabel>
#include <QLayout>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QFont>
#include <QButtonGroup>
#include <QXmlStreamWriter>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>


#define DATE_FORMAT_AS_STRING "yyyy/MM/dd HH:mm"

class QBoxLayout;


enum {
	BUTTON_ICON = 0
};


MessageList::MessageList(int rows_per_page, QWidget *parent) :
		ItemList(rows_per_page, parent)
{
}

void MessageList::addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn)
{
	const QFont &font = bt_global::font->get(FontManager::TEXT);

	// top level widget (to set background using stylesheet)
	QWidget *box_widget = new QWidget;
	box_widget->setFixedHeight(68);

	QHBoxLayout *box = new QHBoxLayout(box_widget);
	box->setContentsMargins(5, 5, 5, 5);

	// centered file name and description
	QVBoxLayout *labels = new QVBoxLayout;
	QLabel *name = new QLabel(item.name);
	name->setProperty("UnreadMessage", !item.data.toBool());

	name->setFont(font);
	labels->addWidget(name, 1);

	QString desc = item.description;
	if (desc.contains("\n"))
		desc = desc.split("\n")[0] + "... ";
	else if (desc.length() > CHARS_MAX)
	{
		if (desc.contains(" "))
			desc = desc.split(" ")[0] + "... ";
		else
			desc = desc.split(QRegExp("\\w+"))[1] + "... ";
	}

	QLabel *description = new QLabel(desc);
	description->setFont(font);
	labels->addWidget(description, 1);

	box->addLayout(labels, 1);

	// button on the right
	BtButton *btn = new BtButton(item.icons[BUTTON_ICON]);
	box->addWidget(btn, 0, Qt::AlignRight);

	buttons_group->addButton(btn, id_btn);
	layout->addWidget(box_widget, layout->rowCount(), 0);
}


DeleteMessagesPage::DeleteMessagesPage()
{
	const QFont &font = bt_global::font->get(FontManager::TEXT);
	QWidget *content = new QWidget;

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	title_widget->setFont(font);
	NavigationBar *nav_bar = new NavigationBar;

	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, title_widget);

	QVBoxLayout *layout = new QVBoxLayout(content);
	layout->setContentsMargins(10, 10, 30, 17);
	QLabel *box_text = new QLabel;
	box_text->setMargin(10);
	box_text->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	box_text->setWordWrap(true);
	box_text->setText(tr("Are you sure to delete all messages?"));
	box_text->setFont(font);
	layout->addWidget(box_text);

	BtButton *ok_button = new BtButton(bt_global::skin->getImage("ok"));
	connect(ok_button, SIGNAL(clicked()), this, SIGNAL(deleteAll()));
	connect(ok_button, SIGNAL(clicked()), this, SIGNAL(Closed()));
	layout->addWidget(ok_button, 0, Qt::AlignRight | Qt::AlignBottom);
}


MessageContent::MessageContent()
{
	date_label = new QLabel;
	message_label = new QLabel;
	new_message_label = new QLabel;

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(10, 0, 10, 10);
	main_layout->setSpacing(0);

	const QFont &font = bt_global::font->get(FontManager::TEXT);

	new_message_label->setFixedHeight(30);
	new_message_label->setFont(font);
	main_layout->addWidget(new_message_label, 0, Qt::AlignHCenter);
	main_layout->addSpacing(5);

	date_label->setObjectName("Date");
	date_label->setMargin(5);
	date_label->setFixedHeight(30);
	date_label->setAlignment(Qt::AlignRight);
	date_label->setFont(font);
	main_layout->addWidget(date_label);

	message_label->setMargin(10);
	message_label->setObjectName("Text");
	message_label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	message_label->setWordWrap(true);
	message_label->setFont(font);

	text_area = new QScrollArea;
	text_area->setFrameShape(QFrame::NoFrame);
	text_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setWidgetResizable(true);
	text_area->setWidget(message_label);
	main_layout->addWidget(text_area);

	main_layout->addSpacing(5);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->setSpacing(20);
	buttons_layout->setContentsMargins(0, 0, 0, 0);

	buttons_layout->addStretch(1);

	go_home_button = new BtButton(bt_global::skin->getImage("go_home"));
	connect(go_home_button, SIGNAL(clicked()), this, SIGNAL(goHome()));
	buttons_layout->addWidget(go_home_button);

	go_message_list_button = new BtButton(bt_global::skin->getImage("go_message_list"));
	connect(go_message_list_button, SIGNAL(clicked()), this, SIGNAL(goMessagesList()));
	buttons_layout->addWidget(go_message_list_button);

	BtButton *delete_button = new BtButton(bt_global::skin->getImage("delete"));
	connect(delete_button, SIGNAL(clicked()), this, SIGNAL(deleteMessage()));
	buttons_layout->addWidget(delete_button);

	buttons_layout->addStretch(1);

	QHBoxLayout *right_layout = new QHBoxLayout;
	right_layout->setSpacing(10);
	right_layout->setContentsMargins(0, 0, 0, 0);

	prev_button = new BtButton(bt_global::skin->getImage("previous"));
	connect(prev_button, SIGNAL(clicked()), SIGNAL(prevMessage()));
	right_layout->addWidget(prev_button);

	next_button = new BtButton(bt_global::skin->getImage("next"));
	connect(next_button, SIGNAL(clicked()), SIGNAL(nextMessage()));
	right_layout->addWidget(next_button);
	buttons_layout->addLayout(right_layout);

	main_layout->addLayout(buttons_layout);
}

void MessageContent::scrollUp()
{
	QScrollBar *vbar = text_area->verticalScrollBar();
	vbar->setValue(vbar->value() - vbar->pageStep());
}

void MessageContent::scrollDown()
{
	QScrollBar *vbar = text_area->verticalScrollBar();
	vbar->setValue(vbar->value() + vbar->pageStep());
}

void MessageContent::showHomeButton(bool show)
{
	go_home_button->setVisible(show);
}

void MessageContent::showMessageListButton(bool show)
{
	go_message_list_button->setVisible(show);
}

void MessageContent::showNewMessageLabel(bool show)
{
	new_message_label->setText(show ? tr("New Message") : "");
}

void MessageContent::showPrevButton(bool show)
{
	prev_button->setVisible(show);
}

void MessageContent::showNextButton(bool show)
{
	next_button->setVisible(show);
}

void MessageContent::setDate(const QString &date)
{
	date_label->setText(date);
}

void MessageContent::setMessage(const QString &message)
{
	message_label->setText(message);
	text_area->verticalScrollBar()->setValue(0);
}


MessagePage::MessagePage()
{
	MessageContent *content = new MessageContent;
	content->showHomeButton(false);
	content->showMessageListButton(false);
	connect(content, SIGNAL(deleteMessage()), this, SIGNAL(deleteMessage()));
	connect(content, SIGNAL(prevMessage()), this, SIGNAL(prevMessage()));
	connect(content, SIGNAL(nextMessage()), this, SIGNAL(nextMessage()));

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(scrollUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(scrollDown()));

	buildPage(content, nav_bar, title_widget);
}

void MessagePage::setData(const QString &date, const QString &text, bool already_read)
{
	page_content->setDate(date);
	page_content->showNewMessageLabel(!already_read);
	page_content->setMessage(text);
}


AlertMessagePage::AlertMessagePage(const QString &date, const QString &text)
{
	_date = date;
	_text = text;

	MessageContent *content = new MessageContent;
	content->setDate(date);
	content->setMessage(text);
	content->showNewMessageLabel(true);
	content->showPrevButton(false);
	content->showNextButton(false);
	connect(content, SIGNAL(goHome()), this, SIGNAL(goHome()));
	connect(content, SIGNAL(goMessagesList()), this, SIGNAL(goMessagesList()));
	connect(content, SIGNAL(deleteMessage()), this, SIGNAL(deleteMessage()));

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);

	NavigationBar *nav_bar = new NavigationBar(QString(), "scroll_down", "scroll_up", QString());
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(scrollUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(scrollDown()));

	buildPage(content, nav_bar, title_widget);
}

int AlertMessagePage::sectionId() const
{
	return MESSAGES;
}


MessagesListPage::MessagesListPage(const QDomNode &config_node)
{
	Q_UNUSED(config_node)
	SkinContext context(getTextChild(config_node, "cid").toInt());
	skin_cid = bt_global::skin->getCidState();
	MessageList *item_list = new MessageList;

	delete_page = new DeleteMessagesPage;
	connect(delete_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(delete_page, SIGNAL(deleteAll()), SLOT(deleteAll()));

	title = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("delete_all"));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(pgDown()));
	connect(nav_bar, SIGNAL(forwardClick()), delete_page, SLOT(showPage()));

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(showMessage(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, title);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(13, 5, 25, 10);
	loadMessages(MESSAGES_FILENAME);

	message_page = new MessagePage;
	connect(message_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(message_page, SIGNAL(nextMessage()), SLOT(showNextMessage()));
	connect(message_page, SIGNAL(prevMessage()), SLOT(showPrevMessage()));
	connect(message_page, SIGNAL(deleteMessage()), SLOT(deleteMessage()));

	MessageDevice *dev = bt_global::add_device_to_cache(new MessageDevice);
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(newMessage(DeviceValues)));

	current_index = -1;
	need_update = true;
	unread_messages = false;
	// The signal changeIconState must be emitted after the construction of the page,
	// so we use this little trick.
	QTimer::singleShot(0, this, SLOT(checkForUnread()));
}

void MessagesListPage::showPage()
{
	if (need_update)
		page_content->showList();
	need_update = false;
	Page::showPage();
}

void MessagesListPage::loadMessages(const QString &filename)
{
	if (!QFile::exists(filename))
		return;

	QDomDocument qdom_messages;
	QFile fh(filename);
	if (!qdom_messages.setContent(&fh))
	{
		qWarning() << "Unable to read messages file:" << filename;
		return;
	}

	QList<ItemList::ItemInfo> message_list;
	QDomNode root = qdom_messages.documentElement();
	foreach (const QDomNode &item, getChildren(root, "item"))
	{
		QDateTime date = QDateTime::fromString(getTextChild(item, "date"), DATE_FORMAT_AS_STRING);
		QString text = getTextChild(item, "text");
		bool read = getTextChild(item, "read").toInt();
		QStringList icons;
		icons << bt_global::skin->getImage("forward");
		ItemList::ItemInfo info(DateConversions::formatDateTimeConfig(date), text, icons, read);
		message_list.append(info);
	}

	page_content->setList(message_list);
	page_content->showList();
}

void MessagesListPage::checkForUnread()
{
	bool unread = false;
	int count = page_content->itemCount();
	for (int i = 0; i < count; ++i)
		if (!page_content->item(i).data.toBool())
		{
			unread = true;
			break;
		}

	if (unread_messages != unread)
	{
		unread_messages = unread;
		emit changeIconState(unread ? StateButton::ON : StateButton::OFF);
	}
}

int MessagesListPage::sectionId() const
{
	return MESSAGES;
}

void MessagesListPage::newMessage(const DeviceValues &values_list)
{
	connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(playRingtone()));
	if (bt_global::audio_states->currentState() != AudioStates::PLAY_RINGTONE)
		bt_global::audio_states->toState(AudioStates::PLAY_RINGTONE);

	Q_ASSERT_X(values_list[MessageDevice::DIM_MESSAGE].canConvert<Message>(), "MessagesListPage::newMessage", "conversion error");
	bt_global::skin->setCidState(skin_cid);
	Message message = values_list[MessageDevice::DIM_MESSAGE].value<Message>();

	int count = page_content->itemCount();

	// delete the last message if the number of messages is => MESSAGES_MAX
	if (count >= MESSAGES_MAX)
	{
		// We have to search if exists an alert page which has the same text and
		// the same date, in order to delete it.
		const ItemList::ItemInfo &item = page_content->item(count - 1);

		for (int i = 0; i < alert_pages.size(); ++i)
		{
			if (alert_pages[i]->date() == item.name && alert_pages[i]->text() == item.description)
			{
				AlertMessagePage *page = alert_pages.takeAt(i);
				page->deleteLater();
				break;
			}
		}

		page_content->removeItem(count - 1);
	}

	QString date = DateConversions::formatDateTimeConfig(message.datetime);
	QStringList icons;
	icons << bt_global::skin->getImage("forward");
	ItemList::ItemInfo info(date, message.text, icons, false);
	page_content->insertItem(0, info);
	need_update = true;
	saveMessages();

	if (!unread_messages)
	{
		unread_messages = true;
		emit changeIconState(StateButton::ON);
	}

	// Set the current index to the newly inserted item.
	current_index = 0;

	AlertMessagePage *page = new AlertMessagePage(date, message.text);
	connect(page, SIGNAL(goHome()), SLOT(goHome()));
	connect(page, SIGNAL(goMessagesList()), SLOT(goMessagesList()));
	connect(page, SIGNAL(deleteMessage()), SLOT(deleteAlertMessage()));
	connect(page, SIGNAL(destroyed()), SLOT(cleanUpAlert()));

	alert_pages.prepend(page);
	bt_global::display->makeActive();
	bt_global::page_stack.showUserPage(page);
	page->showPage();
}

void MessagesListPage::playRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(playRingtone()));
	connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
	bt_global::ringtones->playRingtone(Ringtones::MESSAGE);
}

void MessagesListPage::ringtoneFinished()
{
	bt_global::audio_states->removeState(AudioStates::PLAY_RINGTONE);
	disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
}

void MessagesListPage::cleanUpAlert()
{
	foreach (AlertMessagePage *page, alert_pages)
		if (page == sender())
		{
			alert_pages.removeOne(page);
			break;
		}
}

void MessagesListPage::showMessage(int index)
{
	current_index = index;
	ItemList::ItemInfo &item = page_content->item(index);

	if (!item.data.toBool())
	{
		item.data = true;
		need_update = true;
		saveMessages();
	}

	// For some weird reason if we set the data of the message before showing
	// the page the QScrollBar::minimum and QScrollBar::maximum have wrong
	// values. As result, the message content can scroll even if the message
	// is short or don't scroll if the message is long.
	message_page->showPage();
	message_page->setData(item.name, item.description, item.data.toBool());
}

void MessagesListPage::showPrevMessage()
{
	if (current_index > 0 && page_content->itemCount())
	{
		--current_index;
		ItemList::ItemInfo &item = page_content->item(current_index);
		message_page->setData(item.name, item.description, item.data.toBool());
		if (!item.data.toBool())
		{
			item.data = true;
			need_update = true;
			saveMessages();
		}
	}
}

void MessagesListPage::showNextMessage()
{
	if (current_index >= 0 && current_index < page_content->itemCount() - 1)
	{
		++current_index;
		ItemList::ItemInfo &item = page_content->item(current_index);
		message_page->setData(item.name, item.description, item.data.toBool());
		if (!item.data.toBool())
		{
			item.data = true;
			need_update = true;
			saveMessages();
		}
	}
}

void MessagesListPage::deleteAll()
{
	QList<ItemList::ItemInfo> empty_list;
	page_content->setList(empty_list);
	page_content->showList();
	saveMessages();
	title->setCurrentPage(1, 1);

	if (unread_messages)
	{
		unread_messages = false;
		emit changeIconState(StateButton::OFF);
	}
}

void MessagesListPage::deleteAlertMessage()
{
	Q_ASSERT_X(!alert_pages.isEmpty(), "MessagesListPage::deleteAlertMessage", "List of alert empty!");
	AlertMessagePage *page = alert_pages.takeAt(current_index);
	page->deleteLater();
	page_content->removeItem(current_index);
	saveMessages();
}

void MessagesListPage::deleteMessage()
{
	page_content->removeItem(current_index);
	page_content->showList();
	saveMessages();

	if (current_index < page_content->itemCount())
		showMessage(current_index);
	else
		showPage();
}

void MessagesListPage::saveMessages()
{
	checkForUnread();
	QString dirname = QFileInfo(MESSAGES_FILENAME).absolutePath();
	if (!QDir(dirname).exists() && !QDir().mkpath(dirname))
	{
		qWarning() << "Unable to create the directory" << dirname << "for scs messages";
		return;
	}

	QString tmp_filename = QString(MESSAGES_FILENAME) + ".new";
	QFile f(tmp_filename);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qWarning() << "Unable to save scs messages (open failed)";
		return;
	}
	QXmlStreamWriter writer(&f);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("message");

	for (int i = 0; i < page_content->itemCount(); ++i)
	{
		const ItemList::ItemInfo &item = page_content->item(i);
		QDateTime d = DateConversions::getDateTimeConfig(item.name);
		writer.writeStartElement("item");
		writer.writeTextElement("date", d.toString(DATE_FORMAT_AS_STRING));
		writer.writeTextElement("text", item.description);
		writer.writeTextElement("read", QString::number(item.data.toInt()));
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeEndDocument();

	if (::rename(qPrintable(tmp_filename), MESSAGES_FILENAME))
		qWarning() << "Unable to save scs messages (rename failed)";
}

void MessagesListPage::setMessageAsRead(int index, bool read)
{
	page_content->item(index).data.setValue<bool>(read);
	saveMessages();
}

void MessagesListPage::goHome()
{
	// Set the last message arrived (that should be the one visualized) as read
	setMessageAsRead(current_index);

	bt_global::page_stack.clear();
	foreach (AlertMessagePage *page, alert_pages)
		page->deleteLater();
	alert_pages.clear();
	bt_global::btmain->showHomePage();
}

void MessagesListPage::goMessagesList()
{
	// Set the last message arrived (that should be the one visualized) as read
	setMessageAsRead(current_index);

	bt_global::page_stack.clear();
	foreach (AlertMessagePage *page, alert_pages)
		page->deleteLater();
	alert_pages.clear();
	showPage();
}
