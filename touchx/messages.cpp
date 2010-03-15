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
			name->setProperty("UnreadMessage", item.data.toBool());

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
}


MessagesListPage::MessagesListPage()
{
	ItemList *item_list = new MessageList(0, 4);

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Messages"), SMALL_TITLE_HEIGHT);
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("delete_all"));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(readMessage(int)));
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	buildPage(item_list, nav_bar, 0, title_widget);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	layout()->setContentsMargins(0, 5, 25, 10);
	loadMessages(MESSAGES_FILENAME);
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

		ItemList::ItemInfo info(DateConversions::formatDateConfig(date.date()) + date.time().toString(" HH:mm"),
			text, "", bt_global::skin->getImage("forward"), read);

		message_list.append(info);
	}

	page_content->setList(message_list);
	page_content->showList();
}

int MessagesListPage::sectionId()
{
		return MESSAGES;
}

void MessagesListPage::readMessage(int index)
{
	ItemList::ItemInfo info = message_list.at(index);
}

