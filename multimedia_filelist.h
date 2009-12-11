#ifndef MULTIMEDIA_FILELIST_H
#define MULTIMEDIA_FILELIST_H

#include "fileselector.h"

class QDomNode;
class FileBrowser;
class SlideshowPage;


class MultimediaFileListPage : public FileSelector
{
Q_OBJECT
public:
	enum Type
	{
		UNKNOWN = -1,
		DIRECTORY = 0,
		AUDIO = 1,
		VIDEO = 2,
		IMAGE = 3
	};

	typedef FileBrowser ContentType;

	MultimediaFileListPage();

public slots:
	virtual void nextItem();
	virtual void prevItem();

signals:
	void displayImages(QList<QString> images, unsigned element);

protected:
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files);
	virtual int currentPage();

private:
	static Type fileType(const QFileInfo &file);

	// returns the files in the current directory with the same type as the "item"
	// file; on return, type contains the file type and current is the index of the
	// item in the output list
	QList<QString> filterFileList(int item, Type &type, int &current);

private slots:
	void startPlayback(int item);

private:
	// icons for different file type
	QList<QString> file_icons;

	// button icons for files/directories
	QString play_file, browse_directory;

	// page to display images
	SlideshowPage *slideshow;
};

#endif // MULTIMEDIA_FILELIST_H


