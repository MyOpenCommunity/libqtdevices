#include "fileselector.h"
#include "icondispatcher.h"
#include "main.h" // IMG_PATH

#include <QTime>
#include <QDebug>
#include <QLabel>
#include <QApplication>


#define MEDIASERVER_MSEC_WAIT_TIME 2000

inline QTime startTimeCounter()
{
	QTime timer;
	timer.start();
	return timer;
}

inline void waitTimeCounter(const QTime& timer, int msec)
{
	int wait_time = msec - timer.elapsed();
	if (wait_time > 0)
		usleep(wait_time * 1000);
}


FileSelector::FileSelector(unsigned rows_per_page, QString start_path)
{
	level = 0;

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
}

const QList<QFileInfo> &FileSelector::getFiles() const
{
	return files_list;
}

void FileSelector::showPage()
{
	Selector::showPage();
	// refresh QDir information
	current_dir.refresh();

	QLabel *l = createWaitDialog();
	QTime time_counter = startTimeCounter();

	if (!browseFiles(current_dir, files_list))
	{
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
		emit notifyExit();
		return;
	}

	waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
	destroyWaitDialog(l);
}

void FileSelector::itemIsClicked(int item)
{
	QLabel *l = createWaitDialog();
	QTime time_counter = startTimeCounter();

	const QFileInfo& clicked_element = files_list[item];
	qDebug() << "[AUDIO] FileSelector::itemIsClicked " << item << "-> " << clicked_element.fileName();

	if (!clicked_element.exists())
		qWarning() << "[AUDIO] Error retrieving file: " << clicked_element.absoluteFilePath();

	if (clicked_element.isDir())
	{
		++level;
		if (!browseDirectory(clicked_element.absoluteFilePath()))
		{
			destroyWaitDialog(l);
			emit notifyExit();
			return;
		}
	}
	else
		emit fileClicked(item);

	waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
	destroyWaitDialog(l);
}

void FileSelector::browseUp()
{
	if (level)
	{
		--level;
		QLabel *l = createWaitDialog();
		QTime time_counter = startTimeCounter();

		if (!browseDirectory(QFileInfo(current_dir, "..").absoluteFilePath()))
		{
			destroyWaitDialog(l);
			emit notifyExit();
			return;
		}
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
	}
	else
		emit notifyExit();
}

bool FileSelector::browseDirectory(QString new_path)
{
	QString old_path = current_dir.absolutePath();
	if (changePath(new_path))
	{
		if (!browseFiles(current_dir, files_list))
		{
			qDebug() << "[AUDIO] empty directory: "<< new_path;
			changePath(old_path);
			--level;
		}
		return true;
	}
	else
	{
		qDebug() << "[AUDIO] browseFiles(): path '" << new_path << "%s' doesn't exist";
		changePath(old_path);
		--level;
	}

	return browseFiles(current_dir, files_list);
}

bool FileSelector::changePath(QString new_path)
{
	// if new_path is valid changes the path and run browseFiles()
	if (QFileInfo(new_path).exists())
	{
		// save the info of old directory
		pages_indexes[current_dir.absolutePath()] = currentPage();

		QString new_path_string = QFileInfo(new_path).absoluteFilePath();
		// change path
		current_dir.setPath(new_path_string);
		return true;
	}
	return false;
}

int FileSelector::displayedPage(const QDir &directory)
{
	if (pages_indexes.contains(directory.absolutePath()))
		return pages_indexes[directory.absolutePath()];
	else
		return 0;
}

void FileSelector::destroyWaitDialog(QLabel *l)
{
	l->hide();
	l->deleteLater();
}

QLabel *FileSelector::createWaitDialog()
{
	QLabel* l = new QLabel(0);
	QPixmap *icon = bt_global::icons_cache.getIcon(IMG_PATH "loading.png");
	l->setPixmap(*icon);

	QRect r = icon->rect();
	r.moveCenter(QPoint(width() / 2, height() / 2));
	l->setGeometry(r);

	l->showFullScreen();
	qApp->processEvents();
	return l;
}
