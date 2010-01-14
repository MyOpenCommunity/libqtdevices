#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <QList>
#include <QString>
#include <QWidget>
#include <QVariant>


class QButtonGroup;
class QVBoxLayout;
class QBoxLayout;
class QLabel;

/**
 * \class ItemList
 *
 * This class show a list of item (files or other "virtual" items) and supply a
 * list of signal/slot for handling the user choices.
 */
class ItemList : public QWidget
{
Q_OBJECT
public:
	struct ItemInfo
	{
		ItemInfo(QString name = QString(), QString desc = QString(),
			 QString icon = QString(), QString button_icon = QString(), QVariant data=QVariant());

		QString name;
		QString description;
		QString icon;
		QString button_icon;
		QVariant data;
	};

	ItemList(QWidget *parent, int rows_per_page);

	void setList(QList<ItemInfo> items, int page = 0);
	void showList();

	unsigned getCurrentPage();

signals:
	void itemIsClicked(int item);
	void displayScrollButtons(bool);
	void contentScrolled(int, int);

public slots:
	void nextItem();
	void prevItem();

private:
	int pageCount();

private slots:
	void clicked(int item);

private:
	/// For pagination
	int rows_per_page;
	int current_page;

	QButtonGroup *buttons_group;
	QVBoxLayout *main_layout;

	/// The list of items displayed
	QList<ItemInfo> item_list;

	void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);
};

#endif
