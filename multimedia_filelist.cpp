#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "filebrowser.h"

#include <QLayout>
#include <QDebug>


const char *audio_files[] = {"m3u", "mp3", "wav", "ogg", "wma"};
const char *video_files[] = {"mpg", "avi"};
const char *image_files[] = {"png", "gif", "jpg", "jpeg"};

#define ARRAY_SIZE(x) int(sizeof(x) / sizeof((x)[0]))

// transforms an extension to a pattern (es. "wav" -> "*.[wW][aA][vV]")
void addFilters(QStringList &filters, const char **extensions, int size)
{
	for (int i = 0; i < size; ++i)
	{
		QString pattern = "*.";

		for (const char *c = extensions[i]; *c; ++c)
			pattern += QString("[%1%2]").arg(QChar(*c)).arg(QChar::toUpper((unsigned short)*c));

		filters.append(pattern);
	}
}

MultimediaFileListPage::MultimediaFileListPage()
	: FileSelector(4, "/video/music")
{
	FileBrowser *file_browser = new FileBrowser(0, 4);
	connect(file_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Folder"), 35);
	connect(file_browser, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	NavigationBar *nav_bar = new NavigationBar("eject");

	buildPage(file_browser, nav_bar, 0, title_widget);
	layout()->setContentsMargins(5, 0, 25, 15);

	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(this, SIGNAL(notifyExit()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), file_browser, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), file_browser, SLOT(nextItem()));
	connect(file_browser, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	// order here must match the order in enum Type
	file_icons.append(bt_global::skin->getImage("directory_icon"));
	file_icons.append(bt_global::skin->getImage("audio_icon"));
	file_icons.append(bt_global::skin->getImage("video_icon"));
	file_icons.append(bt_global::skin->getImage("image_icon"));

	play_file = bt_global::skin->getImage("play_file");
	browse_directory = bt_global::skin->getImage("browse_directory");
}

bool MultimediaFileListPage::browseFiles(const QDir &directory, QList<QFileInfo> &files)
{
	QStringList filters;
	addFilters(filters, image_files, ARRAY_SIZE(image_files));
	addFilters(filters, video_files, ARRAY_SIZE(video_files));
	addFilters(filters, audio_files, ARRAY_SIZE(audio_files));

	// Create fileslist from files
	QList<QFileInfo> temp_files_list = directory.entryInfoList(filters);

	if (temp_files_list.empty())
	{
		qDebug() << "[AUDIO] empty directory: " << directory.absolutePath();
		return false;
	}

	files.clear();

	QList<FileBrowser::FileInfo> names_list;

	for (int i = 0; i < temp_files_list.size(); ++i)
	{
		const QFileInfo& f = temp_files_list.at(i);
		if (f.fileName() == "." || f.fileName() == "..")
			continue;

		if (f.isFile())
		{
			FileBrowser::FileInfo info(f.fileName(), QString(),
						   file_icons[fileType(f)],
						   play_file);

			names_list.append(info);
			files.append(f);
		}
		else if (f.isDir())
		{
			FileBrowser::FileInfo info(f.fileName(), QString(),
						   file_icons[DIRECTORY],
						   browse_directory);

			names_list.append(info);
			files.append(f);
		}
	}

	page_content->setList(names_list, displayedPage(directory));
	page_content->showList();

	return true;
}

MultimediaFileListPage::Type MultimediaFileListPage::fileType(const QFileInfo &file)
{
	QString ext = file.suffix().toLower();

	for (int i = 0; i < ARRAY_SIZE(image_files); ++i)
		if (ext == image_files[i])
			return IMAGE;

	for (int i = 0; i < ARRAY_SIZE(video_files); ++i)
		if (ext == video_files[i])
			return VIDEO;

	for (int i = 0; i < ARRAY_SIZE(audio_files); ++i)
		if (ext == audio_files[i])
			return AUDIO;

	return UNKNOWN;
}

int MultimediaFileListPage::currentPage()
{
	return page_content->getCurrentPage();
}

void MultimediaFileListPage::nextItem()
{
	page_content->nextItem();
}

void MultimediaFileListPage::prevItem()
{
	page_content->prevItem();
}

