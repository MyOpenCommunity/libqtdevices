#include "messages.h"
#include "itemlist.h"
#include "skinmanager.h"
#include "navigation_bar.h"
#include "main.h"
#include "generic_functions.h" // DateConvesion::formatDateTimeConfig, getDateTimeConfig
#include "xml_functions.h" // getChildren
#include "fontmanager.h"
#include "btbutton.h"
#include "message_device.h"
#include "devices_cache.h"

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


#define MESSAGES_MAX 10
#define MESSAGES_FILENAME "cfg/extra/4/messages.xml"
#define DATE_FORMAT_AS_STRING "yyyy/MM/dd HH:mm"

class QBoxLayout;
class AlertMessagePage;

namespace
{
	QWidget *buildMessagePage(QVBoxLayout *box_layout, QLabel *new_message_label, QLabel *date_label, QLabel *message_label)
	{
		QWidget *content = new QWidget;
		content->setContentsMargins(10, 0, 10, 10);
		content->setFont(bt_global::font->get(FontManager::TEXT));
		content->setLayout(box_layout);

		box_layout->setSpacing(0);

		new_message_label->setFixedHeight(30);
		box_layout->addWidget(new_message_label, 0, Qt::AlignHCenter);
		box_layout->addSpacing(5);

		date_label->setObjectName("Date");
		date_label->setMargin(5);
		date_label->setFixedHeight(30);
		date_label->setAlignment(Qt::AlignRight);
		box_layout->addWidget(date_label);

		message_label->setMargin(10);
		message_label->setObjectName("Text");
		message_label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		message_label->setWordWrap(true);
		box_layout->addWidget(message_label);

		box_layout->addSpacing(5);

		return content;
	}
}


class AlarmMessageStack
{
public:
	void push(AlertMessagePage *page)
	{
		pages.prepend(page);
	}

	AlertMessagePage *pop()
	{
		AlertMessagePage *page = pages.last();
		pages.removeLast();
		return page;
	}

private:
	QList<AlertMessagePage *>pages;
};


MessageList::MessageList(QWidget *parent, int rows_per_page) :
		ItemList(parent, rows_per_page)
{
}

void MessageList::addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn)
{
	QFont font = bt_global::font->get(FontManager::TEXT);

	// top level widget (to set background using stylesheet)
	QWidget *boxWidget = new QWidget;
	boxWidget->setFixedHeight(68);

	QHBoxLayout *box = new QHBoxLayout(boxWidget);
	box->setContentsMargins(5, 5, 5, 5);

	// centered file name and description
	QVBoxLayout *labels = new QVBoxLayout;
	QLabel *name = new QLabel(item.name);
	name->setProperty("UnreadMessage", !item.data.toBool());

	name->setFont(font);
	labels->addWidget(name, 1);

	QLabel *description = new QLabel(item.description);
	description->setFont(font);
	labels->addWidget(description, 1);

	box->addLayout(labels, 1);

	// button on the right
	BtButton *btn = new BtButton;
	btn->setImage(item.button_icon);
	box->addWidget(btn, 0, Qt::AlignRight);

	buttons_group->addButton(btn, id_btn);
	layout->addWidget(boxWidget);
}


DeleteMessagesPage::DeleteMessagesPage()
{
	QWidget *content = new QWidget;
	content->setFont(bt_global::font->get(FontManager::TEXT));
	content->setContentsMargins(10, 10, 30, 35);
	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar;

	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, 0, title_widget);

	QVBoxLayout *layout = new QVBoxLayout(content);
	QLabel *box_text = new QLabel;
	box_text->setMargin(10);
	box_text->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	box_text->setWordWrap(true);
	box_text->setText(tr("Are you sure to delete all messages?"));
	layout->addWidget(box_text);

	BtButton *ok_button = new BtButton(bt_global::skin->getImage("ok"));
	connect(ok_button, SIGNAL(clicked()), this, SIGNAL(deleteAll()));
	connect(ok_button, SIGNAL(clicked()), this, SIGNAL(Closed()));
	layout->addWidget(ok_button, 0, Qt::AlignRight);
}


MessagePage::MessagePage() :
		date_label(new QLabel), message_label(new QLabel), new_message_label(new QLabel)
{
	QVBoxLayout *box_layout = new QVBoxLayout;

	QWidget *content = buildMessagePage(box_layout, new_message_label, date_label, message_label);

	BtButton *delete_button = new BtButton(bt_global::skin->getImage("delete"));
	connect(delete_button, SIGNAL(clicked()), this, SIGNAL(deleteMessage()));
	box_layout->addWidget(delete_button, 0, Qt::AlignHCenter);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(upClick()), this, SIGNAL(prevMessage()));
	connect(nav_bar, SIGNAL(downClick()), this, SIGNAL(nextMessage()));
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	buildPage(content, nav_bar, 0, title_widget);
}

void MessagePage::setData(const QString &date, const QString &text, bool already_read)
{
	date_label->setText(date);
	new_message_label->setText(!already_read ? tr("New Message") : "");
	message_label->setText(text);
}


AlertMessagePage::AlertMessagePage(const QString &date, const QString &text)
{
	QVBoxLayout *box_layout = new QVBoxLayout;
	QLabel *new_message_label = new QLabel(tr("New Message"));
	QLabel *date_label = new QLabel(date);
	QLabel *message_label = new QLabel(text);

	QWidget *content = buildMessagePage(box_layout, new_message_label, date_label, message_label);

	BtButton *delete_button = new BtButton(bt_global::skin->getImage("delete"));
	connect(delete_button, SIGNAL(clicked()), this, SIGNAL(deleteMessage()));
	box_layout->addWidget(delete_button, 0, Qt::AlignHCenter);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);

	buildPage(content, 0, 0, title_widget);
}


MessagesListPage::MessagesListPage(const QDomNode &config_node) :
		alarm_message_stack(new AlarmMessageStack)
{
	Q_UNUSED(config_node)
	SkinContext context(getTextChild(config_node, "cid").toInt());
	MessageList *item_list = new MessageList(0, 4);

	title = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("delete_all"));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(showDeletePage()));

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(showMessage(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, 0, title);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(0, 5, 25, 10);
	loadMessages(MESSAGES_FILENAME);

	message_page = new MessagePage;
	connect(message_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(message_page, SIGNAL(nextMessage()), SLOT(showNextMessage()));
	connect(message_page, SIGNAL(prevMessage()), SLOT(showPrevMessage()));
	connect(message_page, SIGNAL(deleteMessage()), SLOT(deleteMessage()));

	delete_page = new DeleteMessagesPage;
	connect(delete_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(delete_page, SIGNAL(deleteAll()), SLOT(deleteAll()));

	MessageDevice *dev = bt_global::add_device_to_cache(new MessageDevice);
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(newMessage(DeviceValues)));

	current_index = -1;
	need_update = false;
}

MessagesListPage::~MessagesListPage()
{
	delete alarm_message_stack;
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
	{
		qWarning() << "The messages file:" << filename << "doesn't exist.";
		return;
	}

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
		ItemList::ItemInfo info(DateConversions::formatDateTimeConfig(date), text, "", bt_global::skin->getImage("forward"), read);
		message_list.append(info);
	}

	page_content->setList(message_list);
	page_content->showList();
}

int MessagesListPage::sectionId()
{
		return MESSAGES;
}

void MessagesListPage::newMessage(const DeviceValues &values_list)
{
	Q_ASSERT_X(values_list[MessageDevice::DIM_MESSAGE].canConvert<Message>(), "MessageListPage::newMessage", "conversion error");
	Message message = values_list[MessageDevice::DIM_MESSAGE].value<Message>();

	int count = page_content->itemCount();

	// delete the last message if the number of messages is > MESSAGES_MAX
	if (count > MESSAGES_MAX)
	{
		page_content->removeItem(count - 1);
		AlertMessagePage *page = alarm_message_stack->pop();
		page->hide();
		page->deleteLater();
	}

	QString date = DateConversions::formatDateTimeConfig(message.datetime);
	ItemList::ItemInfo info(date, message.text, "", bt_global::skin->getImage("forward"), false);
	page_content->insertItem(0, info);

	alarm_message_stack->push(new AlertMessagePage(date, message.text));
	saveMessages();
}

void MessagesListPage::showMessage(int index)
{
	current_index = index;
	ItemList::ItemInfo &item = page_content->item(index);
	message_page->setData(item.name, item.description, item.data.toBool());
	if (!item.data.toBool())
	{
		item.data = true;
		need_update = true;
		saveMessages();
	}
	message_page->showPage();
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

void MessagesListPage::showDeletePage()
{
	delete_page->showPage();
}

void MessagesListPage::saveMessages()
{
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
