#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <QList>
#include <QString>
#include <QWidget>

class QButtonGroup;
class QVBoxLayout;
class QBoxLayout;
class QLabel;

/**
 * \class FileBrowser
 *
 * This class show a list of files and supply a list of signal/slot for handling
 * the user choices.
 */
class FileBrowser : public QWidget
{
Q_OBJECT
public:
	struct FileInfo
	{
		FileInfo(QString name = QString(), QString desc = QString(),
			 QString icon = QString(), QString button_icon = QString());

		QString name;
		QString description;
		QString icon;
		QString button_icon;
	};

	FileBrowser(QWidget *parent, int rows_per_page);

	void setList(QList<FileInfo> items, int page = 0);
	void showList();

	unsigned getCurrentPage();

signals:
	void itemIsClicked(int item);
	void displayScrollButtons(bool);

public slots:
	void nextItem();
	void prevItem();

private:
	/// For pagination
	int rows_per_page;
	int current_page;

	QButtonGroup *buttons_group;
	QVBoxLayout *main_layout;

	/// The list of items displayed
	QList<FileInfo> item_list;

	void addHorizontalBox(QBoxLayout *layout, const FileInfo &item, int id_btn);

private slots:
	void clicked(int item);
};

#endif
