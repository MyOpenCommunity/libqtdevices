/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** MultimediaSource.cpp
 **
 ** finestra di dati sulla sorgente MultimediaSource
 **
 ****************************************************************/

#include <qlayout.h>

#include "multimedia_source.h"
#include "playwindow.h"
#include "fontmanager.h"
#include "buttons_bar.h"
#include "listbrowser.h"

MultimediaSource::MultimediaSource(QWidget *parent, const char *name, const char *amb, int _where_address) :
	QWidget(parent, name),
	audio_initialized(true)
{
	// Set main geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	where_address = _where_address;
	qDebug("[AUDIO] MultimediaSource ctor: where_address is %d", _where_address);

	// Create play_window and set style
	play_window = new MediaPlayWindow(this);
	play_window->setBGColor(paletteBackgroundColor());
	play_window->setFGColor(paletteForegroundColor());
	play_window->setPalette(palette());
	play_window->setFont(font());

	// Create filesWindow, Set geometry and Font Style
	filesWindow = new FileSelector(this, 4 /* this means number of rows for the browser */);
	filesWindow->setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE);
	QFont aFont;
	FontManager::instance()->getFont(font_multimedia_source_filesWindow, aFont);
	filesWindow->setFont(aFont);

	// Start to Browse Files
	if (!filesWindow->browseFiles(MEDIASERVER_PATH))
	{
		// FIXME display error?
	}

	// Create Banner Standard di Navigazione (scroll degli Items e la possibilità di tornare indietro)
	bannNavigazione = new bannFrecce(this, "bannerfrecce", 4, ICON_DIFFSON);
	bannNavigazione->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	// Pulsanti up, down e back
	connect(bannNavigazione, SIGNAL(downClick()), filesWindow, SLOT(prevItem()));
	connect(bannNavigazione, SIGNAL(upClick()), filesWindow, SLOT(nextItem()));
	connect(bannNavigazione, SIGNAL(backClick()), filesWindow, SLOT(browseUp()));
	connect(bannNavigazione, SIGNAL(forwardClick()), filesWindow, SIGNAL(notifyExit()));

	// Connection to be notified about Start and Stop Play
	connect(play_window, SIGNAL(notifyStartPlay()), this, SLOT(handleStartPlay()));
	connect(play_window, SIGNAL(notifyStopPlay()), this, SLOT(handleStopPlay()));
	connect(play_window, SIGNAL(settingsBtn()), SIGNAL(Closed()));
	connect(filesWindow, SIGNAL(notifyExit()), this, SIGNAL(Closed()));


	connect(filesWindow, SIGNAL(startPlayer(QValueVector<QString>, unsigned)),
			this, SLOT(startPlayer(QValueVector<QString>, unsigned)));
}

void MultimediaSource::initAudio()
{
	// Now init is done by BtMain.cpp calling dm->inizializza()
	qDebug("[AUDIO] MultimediaSource::initAudio()");
	// perform Audio Init
	if (!audio_initialized)
	{
		emit sendFrame((char *)(QString("*#22*7*#15*%1***4**0**1***0##").arg(where_address).ascii()));
		audio_initialized = true;
	}
}

void MultimediaSource::nextTrack() { play_window->nextTrack(); }
void MultimediaSource::prevTrack() { play_window->prevTrack(); }
void MultimediaSource::stop()      { play_window->stop(); }

void MultimediaSource::pause()
{
	play_window->pause();
}

void MultimediaSource::resume()
{
	play_window->resume();
}

void MultimediaSource::showAux()
{
	// draw and show itself
	draw();
	showFullScreen();

	if (play_window->isPlaying())
		play_window->show();
}

void MultimediaSource::handleStartPlay()
{
	emit sendFrame((char *)(QString("*22*1#4#1*2#%1##").arg(where_address).ascii()));
}

void MultimediaSource::handleStopPlay()
{
	emit sendFrame((char *)(QString("*22*0#4#1*2#%1##").arg(where_address).ascii()));
}

void MultimediaSource::setBGColor(int r, int g, int b)
{
	setBGColor(QColor::QColor(r,g,b));
}

void MultimediaSource::setFGColor(int r, int g, int b)
{
	setFGColor(QColor::QColor(r,g,b));
}

void MultimediaSource::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	bannNavigazione->setBGColor(c);
	filesWindow->setBGColor(c);
	play_window->setBGColor(c);
}
void MultimediaSource::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	bannNavigazione->setFGColor(c);
	filesWindow->setFGColor(c);
	play_window->setFGColor(c);
}

int MultimediaSource::setBGPixmap(char* backImage)
{
	QPixmap Back;
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return 0;
	}
	return 1;
}

void MultimediaSource::enableSource(bool send_frame)
{
	play_window->turnOnAudioSystem(send_frame);
}

void MultimediaSource::disableSource(bool send_frame)
{
	play_window->turnOffAudioSystem(send_frame);
}

void MultimediaSource::startPlayer(QValueVector<QString> list, unsigned element)
{
	play_window->startPlayer(list, element);
	play_window->show();
}

/// ***********************************************************************************************************************
/// Methods for FileSelector
/// ***********************************************************************************************************************

FileSelector::FileSelector(QWidget *parent, unsigned rows_per_page, const char *name, WFlags f) :
	QWidget(parent, name, f)
{
	level = 0;
	list_browser = new ListBrowser(this, rows_per_page, name, f);

	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->addWidget(list_browser);

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setMatchAllDirs(true);
	current_dir.setNameFilter("*.[mM][pP]3;*.[wW][[aA][vV];*.[oO][gG][gG];*.[wW][mM][aA]");

	connect(list_browser, SIGNAL(itemIsClicked(QString)), SLOT(itemIsClicked(QString)));
}

void FileSelector::showEvent(QShowEvent *event)
{
	if (!browseFiles())
	{
		// FIXME display error?
		emit notifyExit();
	}
}

void FileSelector::itemIsClicked(QString item)
{
	qDebug("[AUDIO] FileSelector::itemIsClicked -> %s", item.ascii());
	QFileInfo clicked_element(current_dir, item);
	if (!clicked_element.exists())
	{
		qDebug("[AUDIO] Error retrieving file by name: %s", item.ascii());
	}

	if (clicked_element.isDir())
	{
		++level;
		if (!browseFiles(clicked_element.absFilePath()))
		{
			// FIXME display error?
			emit notifyExit();
		}
	}
	else
	{
		QValueVector<QString> play_list;
		unsigned element = 0;
		unsigned track_number = 0;

		for (unsigned i = 0; i < files_list.count(); ++i)
		{
			QFileInfo fn(current_dir, files_list[i]);
			if (fn.isDir())
				continue;

			play_list.append(fn.absFilePath().latin1());
			if (clicked_element.absFilePath() == fn.absFilePath())
				element = track_number;

			++track_number;
		}

		emit startPlayer(play_list, element);
	}
}

void FileSelector::browseUp()
{
	if (level)
	{
		--level;
		if (!browseFiles(QFileInfo(current_dir, "..").absFilePath()))
		{
			// FIXME display error?
			emit notifyExit();
		}
	}
	else
		emit notifyExit();
}

bool FileSelector::browseFiles(QString new_path)
{
	qDebug("[AUDIO] browseFiles on %s", new_path.ascii());
	// if new_path is valid changes the path and run browseFiles()
	if (QFileInfo(new_path).exists())
	{
		// save the info of old directory
		pages_indexes[current_dir.absPath()] = list_browser->getCurrentPage();

		QString new_path_string = QFileInfo(new_path).absFilePath();
		// change path
		current_dir.setPath(new_path_string);
		return browseFiles();
	}
	else
	{
		qDebug("[AUDIO] browseFiles(): path '%s' doesn't exist", new_path.ascii());
		return false;
	}
}

bool FileSelector::browseFiles()
{
	// refresh QDir information
	current_dir.refresh();

	// Create fileslist from files
	const QFileInfoList *temp_files_list = current_dir.entryInfoList();
	if (!temp_files_list)
	{
		qDebug("[AUDIO] Error retrieving file list!");
		return false;
	}

	files_list.clear();

	// Create Iterator
	QFileInfoListIterator it(*temp_files_list);
	QFileInfo *file;

	while ((file = it.current()) != 0)
	{
		if (file->fileName() != "." && file->fileName() != "..")
			files_list.append(file->fileName().latin1());
		++it;
	}

	unsigned page = 0;
	if (pages_indexes.contains(current_dir.absPath()))
		page = pages_indexes[current_dir.absPath()];

	list_browser->setList(files_list, page);
	list_browser->showList();
	return true;
}

void FileSelector::nextItem()
{
	list_browser->nextItem();
}

void FileSelector::prevItem()
{
	list_browser->prevItem();
}

void FileSelector::setBGColor(QColor c)
{
	list_browser->setBGColor(c);
}

void FileSelector::setFGColor(QColor c)
{
	list_browser->setFGColor(c);
}
