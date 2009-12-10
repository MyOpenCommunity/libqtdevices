#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

#include "page.h"

#include <QDir>
#include <QMap>
#include <QList>
#include <QFileInfo>

class QLabel;


/**
 * \class Selector
 *
 * Realize a common interface for all selector classes.
 *
 */
class Selector : public Page
{
Q_OBJECT
public:
	Selector() {}

public slots:
	virtual void nextItem() = 0;
	virtual void prevItem() = 0;

	virtual void itemIsClicked(int item) = 0;
	virtual void browseUp() = 0;

signals:
	virtual void notifyExit();
};


/**
 * \class FileSelector
 *
 * implements a File Selector Windows with methods to navigate and play files.
 */
class FileSelector : public Selector
{
Q_OBJECT
public:
	FileSelector(unsigned rows_per_page, QString start_path);

public slots:
	void itemIsClicked(int item);
	void browseUp();
	virtual void showPage();

signals:
	void fileClicked(int item);

protected:
	/**
	 * Browse current path, return false in case of error.  In case of success
	 * populates the files list with the files found on the directory.
	 */
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files) = 0;
	virtual int currentPage() = 0;

	/// Browse given path, return false in case of error.
	bool browseDirectory(QString new_path);

	/// returns the list of currently displayed files
	const QList<QFileInfo> &getFiles() const;

	/// returns the page that should be displayed for the given directory
	int displayedPage(const QDir &directory);

private:
	/// Change the current dir, return false in case of error.
	bool changePath(QString new_path);

	QLabel *createWaitDialog();
	void destroyWaitDialog(QLabel *l);

private:
	/// The handler of current directory
	QDir current_dir;

	// set by browseFiles()
	QList<QFileInfo> files_list;

	// How many subdirs we are descending from root.
	unsigned level;

	QMap<QString, unsigned>  pages_indexes;
};

#endif // FILE_SELECTOR_H

