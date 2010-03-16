#include "messages.h"
#include "itemlist.h"
#include "skinmanager.h"
#include "navigation_bar.h"
#include "main.h"
#include "bttime.h"
#include "xml_functions.h" // getChildren
#include "fontmanager.h"
#include "btbutton.h"

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

#define MESSAGES_FILENAME "cfg/extra/4/messages.xml"
#define DATE_FORMAT_AS_STRING "yyyy/MM/dd HH:mm"

class QBoxLayout;

namespace
{
	class MessageList : public ItemList
	{
	public:
		MessageList(QWidget *parent, int rows_per_page) : ItemList(parent, rows_per_page) {}

	protected:
		virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn)
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
	};


	QString formatDateTime(const QDateTime &datetime)
	{
		return DateConversions::formatDateConfig(datetime.date()) + datetime.time().toString(" HH:mm");
	}
}


MessagePage::MessagePage()
{

	QWidget *box_message = new QWidget;

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(upClick()), this, SIGNAL(prevMessage()));
	connect(nav_bar, SIGNAL(downClick()), this, SIGNAL(nextMessage()));
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	buildPage(box_message, nav_bar, 0, title_widget);

	QVBoxLayout *box_layout = new QVBoxLayout(box_message);
	box_layout->setSpacing(0);

	new_message_label = new QLabel;
	new_message_label->setFixedHeight(30);
	box_layout->addWidget(new_message_label, 0, Qt::AlignHCenter);

	date_label = new QLabel;
	date_label->setObjectName("Date");
	date_label->setMargin(5);
	date_label->setFixedHeight(30);
	date_label->setAlignment(Qt::AlignRight);
	box_layout->addWidget(date_label);

	message_label = new QLabel;
	message_label->setMargin(10);
	message_label->setObjectName("Text");
	message_label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	message_label->setWordWrap(true);
	box_layout->addWidget(message_label);
}

void MessagePage::setData(const QString &date, const QString &text, bool already_read)
{
	date_label->setText(date);
	new_message_label->setText(!already_read ? tr("New Message") : "");
	message_label->setText(text);
}


MessagesListPage::MessagesListPage()
{
	ItemList *item_list = new MessageList(0, 4);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("delete_all"));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(deleteAll()));

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(showMessage(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, 0, title_widget);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(0, 5, 25, 10);
	loadMessages(MESSAGES_FILENAME);
	message_page = new MessagePage;
	connect(message_page, SIGNAL(Closed()), SLOT(showPage()));
	connect(message_page, SIGNAL(nextMessage()), SLOT(showNextMessage()));
	connect(message_page, SIGNAL(prevMessage()), SLOT(showPrevMessage()));
	current_index = -1;
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

	QDomNode root = qdom_messages.documentElement();
	foreach (const QDomNode &item, getChildren(root, "item"))
	{
		QDateTime date = QDateTime::fromString(getTextChild(item, "date"), DATE_FORMAT_AS_STRING);
		QString text = getTextChild(item, "text");
		bool read = getTextChild(item, "read").toInt();
		ItemList::ItemInfo info(formatDateTime(date), text, "", bt_global::skin->getImage("forward"), read);
		message_list.append(info);
	}

	page_content->setList(message_list);
	page_content->showList();
}

int MessagesListPage::sectionId()
{
		return MESSAGES;
}

void MessagesListPage::showMessage(int index)
{
	current_index = index;
	ItemList::ItemInfo item = message_list.at(index);
	message_page->setData(item.name, item.description, item.data.toBool());
	message_page->showPage();
}

void MessagesListPage::showPrevMessage()
{
	if (current_index > 0 && message_list.count())
	{
		--current_index;
		ItemList::ItemInfo item = message_list.at(current_index);
		message_page->setData(item.name, item.description, item.data.toBool());
	}
}

void MessagesListPage::showNextMessage()
{
	if (current_index >= 0 && current_index < message_list.count() - 1)
	{
		++current_index;
		ItemList::ItemInfo item = message_list.at(current_index);
		message_page->setData(item.name, item.description, item.data.toBool());
	}
}

void MessagesListPage::deleteAll()
{
	qDebug() << "delete all";
}

