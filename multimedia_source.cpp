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

/*
 * Interface icon paths.
 */
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_SELECT_P = IMG_PATH "arrrgp.png";


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
	filesWindow = new FileBrowser(this, 4 /* this means number of rows for the browser */);
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
/// Methods for TitleLabel
/// ***********************************************************************************************************************
TitleLabel::TitleLabel(QWidget *parent, int w, int h, int _w_offset, int _h_offset, bool _scrolling, WFlags f) :
	QLabel("", parent, 0, f)
{
	// Style
	setFixedWidth(w);
	setFixedHeight(h);
	setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// init
	time_per_step   = 333;
	visible_chars   = 18;
	text_length     = 0;
	current_shift   = 0;
	w_offset        = _w_offset;
	h_offset        = _h_offset;

	// separator for ciclic text scrolling
	separator        = "   --   ";

	// define if it is scrolling label or not
	scrolling = _scrolling;

	// connect timer to scroll text
	connect( &scrolling_timer, SIGNAL( timeout() ), this, SLOT( handleScrollingTimer() ) );
}

void TitleLabel::drawContents(QPainter *p)
{
	p->translate(w_offset, h_offset);
	QLabel::drawContents(p);
}

void TitleLabel::resetTextPosition()
{
	current_shift = 0;
}

void TitleLabel::setText(const QString & text_to_set)
{
	// store full string and full lenght
	text         = text_to_set;
	text_length  = text_to_set.length();
	current_shift = 0;

	// call method of ancestor
	QLabel::setText( text_to_set );

	// start the timer if scroll is needed
	if (scrolling == TRUE && text_length > visible_chars)
		scrolling_timer.start(time_per_step);
	else
		scrolling_timer.stop();
}

void TitleLabel::refreshText()
{
	QString banner_string = QString("%1%2").arg(text).arg(separator);

	QString head = banner_string.mid(current_shift, banner_string.length() - current_shift);
	QString tail = banner_string.mid(0, current_shift);

	QLabel::setText( QString("%1%2").arg(head).arg(tail) );
}

void TitleLabel::setMaxVisibleChars(int n)
{
	visible_chars = n;
	current_shift = 0;
	refreshText();
}

void TitleLabel::handleScrollingTimer()
{
	if (current_shift < text_length + separator.length())
		++current_shift;
	else
		current_shift = 0;

	refreshText();
	repaint();
}

/// ***********************************************************************************************************************
/// Methods for FileBrowser
/// ***********************************************************************************************************************

FileBrowser::FileBrowser(QWidget *parent, unsigned rows_per_page, const char *name, WFlags f) :
	QWidget(parent, name, f)
{
	level = 0;

	// Set Style
	// Look QColorGroup Class Reference
	QPalette current_color_palette = palette();
	current_color_palette.setColor( QColorGroup::Text, Qt::white );
	current_color_palette.setColor( QColorGroup::Base, Qt::black );
	current_color_palette.setColor( QColorGroup::Background, Qt::black );
	current_color_palette.setColor( QColorGroup::Foreground, Qt::white );
	// 3D Effect
	current_color_palette.setColor( QColorGroup::Shadow, Qt::black );
	current_color_palette.setColor( QColorGroup::Midlight, Qt::black );
	current_color_palette.setColor( QColorGroup::Dark, Qt::black );
	setPalette(current_color_palette);

	// label that is shown when no file is present
	//no_files_label = new TitleLabel(this, MAX_WIDTH, 200, 0, 0, FALSE, Qt::WStyle_StaysOnTop);
	//no_files_label->setPalette(current_color_palette);
	//no_files_label->hide();

	// Create main Layout
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->setSpacing(0);

	// Set the number of elements shown
	this->rows_per_page = rows_per_page;

	// Create labels_layout
	QVBoxLayout *labels_layout = new QVBoxLayout( main_layout );
	labels_layout->setMargin(0);
	labels_layout->setSpacing(0);

	// Create labels and add them to label_layout
	// WARNING Quick and Dirty alignment using offsets of TitleLabel
	QValueVector<int> h_offsets;
	h_offsets.append(-4);
	h_offsets.append(-2);
	h_offsets.append(-1);
	h_offsets.append(0);
	labels_list.resize(rows_per_page);
	labels_list.setAutoDelete(TRUE);
	for (unsigned i = 0; i < rows_per_page; i++)
	{
		// Create label and add it to labels_layout
		labels_list.insert( i, new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], TRUE) );
		labels_layout->addWidget( labels_list[i] );
	}

	// Create buttons_bar
	buttons_bar = new ButtonsBar(this, rows_per_page, Qt::Vertical);

	// Set Icons for buttons_bar (using icons_library cache)
	QPixmap *icon         = icons_library.getIcon(IMG_SELECT);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_SELECT_P);
	for (unsigned i = 0; i < rows_per_page; i++)
		buttons_bar->setButtonIcons(i, *icon, *pressed_icon);

	// Add buttons_bar to main_layout
	main_layout->addWidget( buttons_bar );

	// Create Set File Browsing Props
	files_handler.setSorting(QDir::DirsFirst | QDir::Name);
	files_handler.setMatchAllDirs(TRUE);
	files_handler.setNameFilter("*.[mM][pP]3;*.[wW][[aA][vV];*.[oO][gG][gG];*.[wW][mM][aA]");

	// Connette il FilesBrowser con se stesso per il CLICK
	connect(buttons_bar, SIGNAL(clicked(int)), this, SLOT(itemIsClicked(int)));

}

void FileBrowser::showEvent(QShowEvent *event)
{
	for (unsigned i = 0; i < rows_per_page; i++)
		labels_list[i]->resetTextPosition();

	if (!browseFiles())
	{
		// FIXME display error?
		emit notifyExit();
	}
}

void FileBrowser::itemIsClicked(int item)
{
	qDebug("[AUDIO] FileBrowser::itemIsClicked -> %d is clicked and index is %d",
		item, pages_indexes[current_path]);
	QFileInfo *clicked_element;
	unsigned absolute_position_item;

	/// REVERSE SEARCH
	// we have the number of the pressed button we want the file
	absolute_position_item = pages_indexes[current_path] + item;
	if ( absolute_position_item < files_list.count() )
		clicked_element = files_list[pages_indexes[current_path] + item];
	else
		return;

	if (clicked_element->exists())
	{
		if (clicked_element->isDir())
		{
			level++;
			if (!browseFiles(clicked_element->absFilePath()))
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
			QString track_name;

			for (unsigned i = 0; i < files_list.count(); ++i)
			{
				if (files_list[i]->isDir())
					continue;

				track_name = files_list[i]->absFilePath().latin1();
				play_list.append(track_name);
				if (clicked_element->absFilePath().latin1() == track_name)
					element = track_number;

				++track_number;
			}

			emit startPlayer(play_list, element);
		}
	}
	else
	{
		// this is added to browse again the current path and verify if something is changed
		// it is useful if some file is removed so the current directory is refreshed
		qDebug( QString("[AUDIO] Current absFilePath is %1").arg( QFileInfo(current_path).absFilePath() ) );
		if (!browseFiles())
		{
			// FIXME display error?
			emit notifyExit();
		}
	}
}

void FileBrowser::browseUp()
{
	if (level)
	{
		level--;
		if (!browseFiles(QFileInfo(current_path + "/..").absFilePath()))
		{
			// FIXME display error?
			emit notifyExit();
		}
	}
	else
		emit notifyExit();
}

bool FileBrowser::browseFiles(QString new_path)
{
	// if new_path is valid changes the path and run browseFiles()
	if (QFileInfo(new_path).exists())
	{
		QString new_path_string = QFileInfo(new_path).absFilePath();
		// change path
		files_handler.setPath(new_path_string);
		current_path = new_path_string;
		return browseFiles();
	}
	else
	{
		qDebug("[AUDIO] browseFiles(): path '%s' doesn't exist", new_path.ascii());
		return false;
	}
}

bool FileBrowser::browseFiles()
{
	// refresh QDir information
	files_handler.refresh();

	// Clear list of pointers (so we can destroy the old files list owned by QDir object)
	files_list.clear();

	// Create fileslist from files
	const QFileInfoList *temp_files_list = files_handler.entryInfoList();
	if (!temp_files_list)
	{
		qDebug("[AUDIO] Error retrieving file list!");
		return false;
	}

	// Create Iterator
	QFileInfoListIterator it( *temp_files_list );
	QFileInfo *file;

	// Build files list dictionary
	files_list.resize(temp_files_list->count());
	int index = 0;
	while ( (file = it.current()) != 0 )
	{
		files_list.insert(index, file);
		++index;
		++it;
	}

	// Store DEFAULT starting index in the dictionary if it is not present
	// DEFAULT STARTING INDEX = 2 so we avoid "." and ".."
	if (!pages_indexes.contains(current_path))
		pages_indexes[current_path] = 2;

	showFiles();
	return true;
}

void FileBrowser::showFiles()
{
	unsigned start = pages_indexes[current_path];
	unsigned end   = std::min(start+rows_per_page, files_list.count());

	for (unsigned i = 0; i < labels_list.size(); ++i)
	{
		if (i < end-start)
		{
			labels_list[i]->setText(getTextRepresentation(files_list[start+i]));
			buttons_bar->showButton(i);
		}
		else
		{
			labels_list[i]->setText("");
			buttons_bar->hideButton(i);
		}

	}

	if (start==end)
	{
		buttons_bar->showButton(0);
		buttons_bar->setEnabled(FALSE);
	}
	else
	{
		buttons_bar->setEnabled(TRUE);
	}
}

QString FileBrowser::getTextRepresentation(QFileInfo *file_info)
{
	QString text_repr;

	if (file_info->isDir())
		text_repr = "%1"; //"<%1>";
	else
		text_repr = "%1";

	QString name = file_info->fileName().latin1();

	/*
	if (name.length()>18)
	{
		name.truncate(18);
		name += "...";
	}
	*/

	return text_repr.arg(name);
}

void FileBrowser::nextItem()
{
	// FIX migliorabile, senza questo quando c'è un solo elemento crasha
	if (files_list.count()<=rows_per_page)
		return;

	if (pages_indexes[current_path] < files_list.count()-rows_per_page)
		pages_indexes[current_path] += rows_per_page;
	showFiles();
}

void FileBrowser::prevItem()
{
	// FIX migliorabile, senza questo quando c'è un solo elemento crasha
	if (files_list.count()<=rows_per_page)
		return;

	if (pages_indexes[current_path] > rows_per_page)
		pages_indexes[current_path] -= rows_per_page;
	showFiles();
}

void FileBrowser::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	buttons_bar->setBGColor(c);
}
void FileBrowser::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_bar->setFGColor(c);
}
