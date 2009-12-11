#ifndef MULTIMEDIA_FILELIST_H
#define MULTIMEDIA_FILELIST_H

#include "fileselector.h"

class QDomNode;
class FileBrowser;


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

protected:
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files);
	virtual int currentPage();

private:
	static Type fileType(const QFileInfo &file);

private:
	// icons for different file type
	QList<QString> file_icons;

	// button icons for files/directories
	QString play_file, browse_directory;
};

#endif // MULTIMEDIA_FILELIST_H


