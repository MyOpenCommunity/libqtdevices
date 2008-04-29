
#include <algorithm>
#include <string>

#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qprocess.h>
#include <qstring.h>
#include <qfile.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qevent.h>
#include <qvaluevector.h>

#include "multimedia_source.h"
#include "mediaplayer.h"
#include "banner.h"
#include "bannondx.h"
#include "bannfrecce.h"
#include "main.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Scripts launched before and after a track is played.
 */
static const char *start_play_script = "/bin/audio_on.tcl";
static const char *stop_play_script = "/bin/audio_off.tcl";

/*
 * Interface icon paths.
 */
static const char *IMG_PLAY = IMG_PATH "btnplay.png"; 
static const char *IMG_STOP = IMG_PATH "btnsdstop.png";
static const char *IMG_PAUSE = IMG_PATH "btnpause.png";
static const char *IMG_NEXT = IMG_PATH "btnforward.png";
static const char *IMG_PREV = IMG_PATH "btnbackward.png";
static const char *IMG_BACK = IMG_PATH "arrlf.png";
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_SETTINGS = IMG_PATH "appdiffsmall.png";

static const char *IMG_PLAY_P = IMG_PATH "btnplayp.png"; 
static const char *IMG_STOP_P = IMG_PATH "btnsdstopp.png";
static const char *IMG_PAUSE_P = IMG_PATH "btnpausep.png";
static const char *IMG_NEXT_P = IMG_PATH "btnforwardp.png";
static const char *IMG_PREV_P = IMG_PATH "btnbackwardp.png";
static const char *IMG_BACK_P = IMG_PATH "arrlfp.png";
static const char *IMG_SELECT_P = IMG_PATH "arrrgp.png";
static const char *IMG_SETTINGS_P = IMG_PATH "appdiffsmallp.png";



MultimediaSource::MultimediaSource( QWidget *parent, const char *name, const char *amb, int _where_address ) :
	QWidget( parent, name ),
	audio_initialized(false)
{
	/// Set main geometry
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	where_address = _where_address;
	qDebug("[AUDIO] MultimediaSource ctor: where_address is %d", _where_address);

	/// Create filesWindow, Set geometry and Font Style
	filesWindow = new FileBrowser(this, 4 /* this means number of rows for the browser */);
	filesWindow->setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE);
	filesWindow->setFont( QFont( "Helvetica", 18 ) );

	/// Start to Browse Files
	if (!filesWindow->browseFiles(MEDIASERVER_PATH))
	{
		// FIXME display error?
	}

	/// Create Banner Standard di Navigazione (scroll degli Items e la possibilità di tornare indietro )
	bannNavigazione = new bannFrecce(this, "bannerfrecce", 4, ICON_DIFFSON);
	bannNavigazione->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	/// Pulsanti up, down e back
	connect(bannNavigazione, SIGNAL(downClick()), filesWindow, SLOT(prevItem()));
	connect(bannNavigazione, SIGNAL(upClick()), filesWindow, SLOT(nextItem()));
	connect(bannNavigazione, SIGNAL(backClick()), filesWindow, SLOT(browseUp()));
	connect(bannNavigazione, SIGNAL(forwardClick()), filesWindow, SIGNAL(notifyExit()));

	/// Connection to be notified about Start and Stop Play
	connect(filesWindow, SIGNAL(notifyStartPlay()), this, SLOT(handleStartPlay()));
	connect(filesWindow, SIGNAL(notifyStopPlay()), this, SLOT(handleStopPlay()));
	connect(filesWindow, SIGNAL(notifyExit()), this, SIGNAL(Closed()));
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


void MultimediaSource::showAux()
{
	// draw and show itself
	draw();
	showFullScreen();

	filesWindow->showPlayingStatusIfPlaying();
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
	setBGColor( QColor :: QColor(r,g,b));
}

void MultimediaSource::setFGColor(int r, int g, int b)
{
	setFGColor( QColor :: QColor(r,g,b));
}

void MultimediaSource::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	bannNavigazione->setBGColor(c);
	filesWindow->setBGColor(c);
}
void MultimediaSource::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	bannNavigazione->setFGColor(c);
	filesWindow->setFGColor(c);
}

int MultimediaSource::setBGPixmap(char* backImage)
{
	QPixmap Back;
	if(Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		return (0);
	}
	return (1);
}

void MultimediaSource::enableSource()
{
	filesWindow->enableSource();
}

void MultimediaSource::disableSource()
{
	filesWindow->disableSource();
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

void TitleLabel::drawContents( QPainter *p )
{
	p->translate(w_offset, h_offset);
	QLabel::drawContents( p );
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
	QWidget(parent, name, f),
	level(0)
{
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

	// Create playing_window and set style
	playing_window = new AudioPlayingWindow(this);
	playing_window->setBGColor(paletteBackgroundColor());
	playing_window->setFGColor(paletteForegroundColor());
	playing_window->setPalette(palette());
	playing_window->setFont(font());

	// Connette il FilesBrowser con se stesso per il CLICK
	connect(buttons_bar, SIGNAL(clicked(int)), this, SLOT(itemIsClicked(int)));

	// Signal from playing window
	connect(playing_window, SIGNAL(notifyStartPlay()), SIGNAL(notifyStartPlay()));
	connect(playing_window, SIGNAL(notifyStopPlay()), SIGNAL(notifyStopPlay()));
	connect(playing_window, SIGNAL(settingsBtn()), SIGNAL(notifyExit()));
}

void FileBrowser::showEvent( QShowEvent *event )
{
	for (unsigned i = 0; i < rows_per_page; i++)
		labels_list[i]->resetTextPosition();

	if (!browseFiles())
	{
		// FIXME display error?
		emit notifyExit();
	}
}

void FileBrowser::showPlayingStatusIfPlaying()
{
	if (playing_window->isPlaying())
		playing_window->show();
}

void FileBrowser::enableSource()
{
	playing_window->turnOnAudioSystem();
}

void FileBrowser::disableSource()
{
	playing_window->turnOffAudioSystem();
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
			/// Load play list in playing window and show it
			playing_window->startPlay(files_list, clicked_element);
			playing_window->show();
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

void FileBrowser::showPlayingWindow()
{
	playing_window->show();
}

void FileBrowser::hidePlayingWindow()
{
	playing_window->hide();
}

bool FileBrowser::browseFiles(QString new_path)
{
	// if new_path is valid changes the path and run browsFiles()
	if ( QFileInfo(new_path).exists() )
	{
		QString new_path_string = QFileInfo(new_path).absFilePath();
		// change path
		files_handler.setPath( new_path_string );
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
			labels_list[i]->setText( getTextRepresentation(files_list[start+i]) );
			buttons_bar->showButton( i );
		}
		else
		{
			labels_list[i]->setText( "" );
			buttons_bar->hideButton( i );
		}

	}

	if (start==end)
	{
		buttons_bar->showButton( 0 );
		buttons_bar->setEnabled( FALSE );
	}
	else
	{
		buttons_bar->setEnabled( TRUE );
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
	playing_window->setBGColor(c);
}
void FileBrowser::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_bar->setFGColor(c);
	playing_window->setFGColor(c);
}

/*
 * Path in conf.xml where the configurable label texts are found.
 */
#define CFG_LABELS_MEDIAPLAYER "configuratore/setup/labels/mediaplayer/"

/// ***********************************************************************************************************************
/// Methods for AudioPlayingWindow
/// ***********************************************************************************************************************

AudioPlayingWindow::AudioPlayingWindow(QWidget *parent, const char * name) :
	QWidget(parent, name, WStyle_NoBorder | WType_TopLevel | WStyle_Customize)
{
	qDebug("[AUDIO] AudioPlayingWindow costructor");

	/// set self Geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);

	/// Create Labels (that contain tags)
	QFont font( "helvetica", 18 );

	// create Labels containing INFO
	//labels_list.insert( i, new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], TRUE) );
	meta_title_label  = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE );
	meta_artist_label = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE );
	meta_album_label  = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE );
	time_pos_label    = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE );
	// Set Font
	meta_title_label->setFont(font);
	meta_artist_label->setFont(font);
	meta_album_label->setFont(font);
	time_pos_label->setFont(font);

	/// Create Main Layout
	// all others layout must have this as parent, this is not more needed in Qt4
	// where we can use setMainLayout
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->addSpacing(20);

	// layouts for both labels
	QHBoxLayout *tags_layout = new QHBoxLayout(main_layout);
	QVBoxLayout *tags_name_layout = new QVBoxLayout(tags_layout);
	QVBoxLayout *tags_text_layout = new QVBoxLayout(tags_layout);

	/// Create Labels (that contain tag names)
	// Get label names from app_confif
	QString label_a = app_config.get(CFG_LABELS_MEDIAPLAYER "meta_title",  "Name: ").c_str();
	QString label_b = app_config.get(CFG_LABELS_MEDIAPLAYER "meta_artist", "Artist: ").c_str();
	QString label_c = app_config.get(CFG_LABELS_MEDIAPLAYER "meta_album",  "Album: ").c_str();
	QString label_d = app_config.get(CFG_LABELS_MEDIAPLAYER "meta_time",   "Time: ").c_str();
	// Set label names
	TitleLabel *name_label   = new TitleLabel( this, MAX_WIDTH/3, 30, 9, 0 );
	TitleLabel *artist_label = new TitleLabel( this, MAX_WIDTH/3, 30, 9, 0 );
	TitleLabel *album_label  = new TitleLabel( this, MAX_WIDTH/3, 30, 9, 0 );
	TitleLabel *time_label   = new TitleLabel( this, MAX_WIDTH/3, 30, 9, 0 );
	// set font
	name_label->setFont(font);
	artist_label->setFont(font);
	album_label->setFont(font);
	time_label->setFont(font);
	// set text
	name_label->setText( label_a );
	artist_label->setText( label_b );
	album_label->setText( label_c );
	time_label->setText( label_d );

	// add all labels to layouts
	tags_name_layout->addWidget(name_label);
	tags_name_layout->addWidget(artist_label);
	tags_name_layout->addWidget(album_label);
	tags_name_layout->addWidget(time_label);
	tags_text_layout->addWidget(meta_title_label);
	tags_text_layout->addWidget(meta_artist_label);
	tags_text_layout->addWidget(meta_album_label);
	tags_text_layout->addWidget(time_pos_label);


	/*
	 * Create Buttons and set their geometry
	 */
	QPixmap *icon;
	QPixmap *pressed_icon;

	play_controls = new ButtonsBar(this, 4, Qt::Horizontal);
	play_controls->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/(NUM_RIGHE+1), MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	showPauseBtn();

	icon         = icons_library.getIcon(IMG_STOP);
	pressed_icon = icons_library.getIcon(IMG_STOP_P);
	play_controls->setButtonIcons(1, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_PREV);
	pressed_icon = icons_library.getIcon(IMG_PREV_P);
	play_controls->setButtonIcons(2, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_NEXT);
	pressed_icon = icons_library.getIcon(IMG_NEXT_P);
	play_controls->setButtonIcons(3, *icon, *pressed_icon);

	main_layout->addWidget(play_controls);

	/*
	 * Main controls
	 */
	QHBoxLayout *main_controls_layout = new QHBoxLayout(main_layout);

	back_btn = new BtButton(this, "back_btn");
	settings_btn = new BtButton(this, "settings_btn");
	main_controls_layout->addWidget(back_btn);
	main_controls_layout->addStretch();
	main_controls_layout->addWidget(settings_btn);

	back_btn->setPixmap(*icons_library.getIcon(IMG_BACK));
	back_btn->setPressedPixmap(*icons_library.getIcon(IMG_BACK_P));

	settings_btn->setPixmap(*icons_library.getIcon(IMG_SETTINGS));
	settings_btn->setPressedPixmap(*icons_library.getIcon(IMG_SETTINGS_P));


	data_refresh_timer = new QTimer(this);

	media_player = new MediaPlayer(this);

	connect(play_controls, SIGNAL(clicked(int)), SLOT(handle_buttons(int)));
	connect(back_btn, SIGNAL(released()), SLOT(handleBackBtn()));
	connect(settings_btn, SIGNAL(released()), SLOT(handleSettingsBtn()));

	// Connect the timer to the handler, the timer get and displays data from mplayer
	connect(data_refresh_timer, SIGNAL(timeout()), SLOT(handle_data_refresh_timer()));

	connect(media_player, SIGNAL(mplayerDone()), SLOT(handlePlayingDone()));
	connect(media_player, SIGNAL(mplayerAborted()), SLOT(handlePlayingAborted()));

	// Set Timer
	refresh_time = 500;
}

void AudioPlayingWindow::startPlay(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element)
{
	// quit mplayer if it is already playing
	media_player->quit();

	// fill play_list and set current track
	int     track_number = 0;
	QString track_name;
	play_list.clear();

	for (unsigned i = 0; i < files_list.count(); i++)
	{
		if (files_list[i]->isDir())
			continue;

		track_name = files_list[i]->absFilePath().latin1();
		play_list.append(track_name);
		if (clicked_element->absFilePath().latin1() == track_name)
			current_track = track_number;
		track_number++;
	}

	// Turn On Audio System
	turnOnAudioSystem();

	// Start playing and point next Track
	cleanPlayingInfo();
	media_player->play(play_list[current_track]);
	current_track++;

	// Start Timer
	data_refresh_timer->start(refresh_time);
}

void AudioPlayingWindow::turnOnAudioSystem()
{
	qDebug("[AUDIO] Running start play script: %s", start_play_script);

	int rc;
	if ((rc = system(start_play_script)) != 0)
		qDebug("[AUDIO] Error on start play script, exit code %d", WEXITSTATUS(rc));

	emit notifyStartPlay();
}

void AudioPlayingWindow::turnOffAudioSystem()
{
	qDebug("[AUDIO] Running stop play script: %s", stop_play_script);

	int rc;
	if ((rc = system(stop_play_script)) != 0)
		qDebug("[AUDIO] Error on stop play script, exit code %d", rc);

	emit notifyStopPlay();
}

bool AudioPlayingWindow::isPlaying()
{
	return media_player->isPlaying();
}

void AudioPlayingWindow::refreshPlayingInfo()
{
	QMap<QString, QString> updated_playing_info;
	updated_playing_info = media_player->getPlayingInfo();

	// Now we iterate on updated_playing_info and import new entries in playing_info
	QMap<QString, QString>::Iterator it;
	for ( it = updated_playing_info.begin(); it != updated_playing_info.end(); ++it )
		playing_info[it.key()] = it.data();

	// Extract Time Data
	QStringList total   = QStringList::split(".", playing_info["total_time"]);
	QStringList current = QStringList::split(".", playing_info["current_time"]);

	// Set INFO in Labels
	meta_title_label->setText(playing_info["meta_title"]);
	meta_artist_label->setText(playing_info["meta_artist"]);
	meta_album_label->setText(playing_info["meta_album"]);
	if (playing_info["total_time"] == "" || playing_info["current_time"] == "")
		time_pos_label->setText(QString("- of -"));
	else
		time_pos_label->setText(QString("%1 of %2").arg(current[0]).arg(total[0]));
}

void AudioPlayingWindow::cleanPlayingInfo()
{
	meta_title_label->setText(QString("-"));
	meta_artist_label->setText(QString("-"));
	meta_album_label->setText(QString("-"));
	time_pos_label->setText(QString("- of -"));
	playing_info.clear();
}

void AudioPlayingWindow::showPlayBtn()
{
	QPixmap *icon         = icons_library.getIcon(IMG_PLAY);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_PLAY_P);
	play_controls->setButtonIcons(0, *icon, *pressed_icon);
}

void AudioPlayingWindow::showPauseBtn()
{
	QPixmap *icon         = icons_library.getIcon(IMG_PAUSE);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_PAUSE_P);
	play_controls->setButtonIcons(0, *icon, *pressed_icon);
}

void AudioPlayingWindow::prevTrack()
{
	/*
	 * We set new current track and we just kill the process.
	 * ATTENZIONE, dato che dopo il play ci posizioniamo già sulla traccia successiva, è
	 * necessario tornare indietro di due tracce se possibile.
	 */
	if (current_track >= 2)
	{
		current_track -= 2;
		media_player->quit();
	}
	else if (current_track == 1)
	{
		/*
		 * Se la current_track è uguale a 1 vuol dire che stiamo suonando la traccia zero
		 * il questo caso la riportiamo all'inizio.
		 */
		current_track = 0;
		media_player->quit();
	}
	qDebug("[AUDIO] AudioPlayingWindow::prevTrack() now playing: %d/%d", current_track-1, play_list.count());
}

void AudioPlayingWindow::nextTrack()
{
	/*
	 * We set new current track and we just kill the process.
	 * ATTENZIONE, dato che dopo il play ci posizioniamo già sulla traccia successiva, è
	 * necessario uccidere il processo se esiste una traccia successiva.
	 */
	if (current_track < play_list.count())
	{
		media_player->quit();
	}
	qDebug("[AUDIO] AudioPlayingWindow::nextTrack() now playing: %d/%d", current_track-1, play_list.count());
}

void AudioPlayingWindow::stop()
{
	// That's all folks
	qDebug("[AUDIO] AudioPlayingWindow::stop()");
	current_track = CURRENT_TRACK_NONE;
	media_player->quit();
}

void AudioPlayingWindow::handle_buttons(int button_number)
{
	data_refresh_timer->stop();

	switch (button_number)
	{
	case 0:
		// play/pause
		if (current_track == CURRENT_TRACK_NONE)
		{
			// Restart play after previous stop
			media_player->play(play_list[0]);
			current_track = 1;  // next due
			data_refresh_timer->start(refresh_time);
		}
		else if (media_player->isPaused())
		{
			qDebug("[AUDIO] media_player: resume play");
			turnOnAudioSystem();
			media_player->resume();
			data_refresh_timer->start(refresh_time);
		}
		else
		{
			qDebug("[AUDIO] media_player: pause play");
			turnOffAudioSystem();
			media_player->pause();
		}
		break;
	case 1:
		stop();
		break;
	case 2:
		// go to prev track
		if (current_track != CURRENT_TRACK_NONE)
		{
			prevTrack();
			data_refresh_timer->start(refresh_time);
		}
		break;
	case 3:
		// go to next track
		if (current_track != CURRENT_TRACK_NONE)
		{
			nextTrack();
			data_refresh_timer->start(refresh_time);
		}
		break;
	}

	if (media_player->isPlaying() && !media_player->isPaused())
		showPauseBtn();
	else
		showPlayBtn();
}

void AudioPlayingWindow::handleBackBtn()
{
	hide();
}

void AudioPlayingWindow::handleSettingsBtn()
{
	hide();
	emit settingsBtn();
}

void AudioPlayingWindow::handle_data_refresh_timer()
{
	refreshPlayingInfo();
}


void AudioPlayingWindow::handlePlayingDone()
{
	/*
	 * mplayer has terminated because the track is finished
	 * so we go to the next track if exists.
	 */
	cleanPlayingInfo();

	if (current_track < play_list.count())
	{
		media_player->play(play_list[current_track]);
		/*
		 * una volta lanciata una traccia si posiziona sulla successiva
		 * se quella appena mandata in play è l'ultima la prossima volta
		 * che viene lanciata questa funzione non verrà eseguito il play.
		 */
		current_track++;
	}
	else
	{
		// In questo caso ha finito di suonare l'ultimo pezzo
		turnOffAudioSystem();
		showPlayBtn();
	}
}

void AudioPlayingWindow::handlePlayingAborted()
{
	turnOffAudioSystem();

	hide();

	// FIXME display error?
	qDebug("[AUDIO] Error in mplayer, stopping playlist");
}

void AudioPlayingWindow::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	play_controls->setBGColor(c);
	back_btn->setPaletteBackgroundColor(c);
	settings_btn->setPaletteBackgroundColor(c);
}

void AudioPlayingWindow::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	play_controls->setFGColor(c);
	back_btn->setPaletteForegroundColor(c);
	settings_btn->setPaletteForegroundColor(c);
}


