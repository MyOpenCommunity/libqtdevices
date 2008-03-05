/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** MultimediaSource.cpp
 **
 ** finestra di dati sulla sorgente MultimediaSource
 **
 ****************************************************************/


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

/// ***********************************************************************************************************************
/// Methods for MultimediaSource
/// ***********************************************************************************************************************

MultimediaSource::MultimediaSource( QWidget *parent, const char *name, const char *amb, int _where_address ) :
	QWidget( parent, name ),
	audio_initialized(false)
{
	/// Set main geometry
	setGeometry(0,0,MAX_WIDTH,MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	where_address = _where_address;
	qDebug("MultimediaSource ctor: where_address is %d", _where_address);

	/// Create filesWindow, Set geometry and Font Style
	filesWindow = new FileBrowser(this, 4 /* this means number of rows for the browser */);
	filesWindow->setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE);
	filesWindow->setFont( QFont( "Helvetica", 18 ) );

	/// Start to Browse Files
	filesWindow->browseFiles(MEDIASERVER_PATH);

	/// Create Banner Standard di Navigazione (scroll degli Items e la possibilità di tornare indietro )
	bannNavigazione = new bannFrecce(this,"bannerfrecce",4);
	bannNavigazione->setGeometry(0, MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	/// Pulsanti up, down e back
	connect( bannNavigazione, SIGNAL(downClick()), filesWindow, SLOT(prevItem()) );
	connect( bannNavigazione, SIGNAL(upClick()), filesWindow, SLOT(nextItem()) );
	connect( bannNavigazione, SIGNAL(longBackPress()), SLOT(handleLongBackPress()) );
	connect( bannNavigazione, SIGNAL(forwardClick()),  filesWindow, SLOT(showPlayingWindow()) );

	/// Connection to be notified about Start and Stop Play
	connect( filesWindow, SIGNAL(notifyStartPlay()), this, SLOT(handleStartPlay()) );
	connect( filesWindow, SIGNAL(notifyStopPlay()), this, SLOT(handleStopPlay()) );
	connect( filesWindow, SIGNAL(notifyExit()), this, SIGNAL(Closed()) );
}


void MultimediaSource::initAudio()
{
	// Now init is done by BtMain.cpp calling dm->inizializza()
	qDebug("MultimediaSource::initAudio()");
	// perform Audio Init
	if (!audio_initialized)
	{
		emit sendFrame((char *)(QString("*#22*7*#15*%1***4**0**1***0##").arg(where_address).ascii()));
		audio_initialized = true;
	}
}


void MultimediaSource::showAux()
{
	/*
	 * Disconnect first because showAux is called serveral times in the object lifetime
	 * and we do not want duplicate connections.
	 */
	disconnect(bannNavigazione, SIGNAL(backClick()), filesWindow, SLOT(browseUp()));
	connect(bannNavigazione, SIGNAL(backClick()), filesWindow, SLOT(browseUp()));

	// draw and show itself
	draw();
	showFullScreen();
}

void MultimediaSource::handleStartPlay()
{
	emit sendFrame((char *)(QString("*22*1#4#1*2#%1##").arg(where_address).ascii()));
}

void MultimediaSource::handleStopPlay()
{
	emit sendFrame((char *)(QString("*22*0#4#1*2#%1##").arg(where_address).ascii()));
}

void MultimediaSource::handleLongBackPress()
{
	disconnect(bannNavigazione, SIGNAL(backClick()), filesWindow, SLOT(browseUp()));
	emit(Closed());
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

void MultimediaSource::setAmbDescr(char *d)
{
	ambDescr->setText(d);
}

void MultimediaSource::draw()
{
}

void MultimediaSource::freezed(bool f)
{
	qDebug("MultimediaSource::freezed()");
	
	// FIXED by Kemosh:
	// a removed setDisabled(f) to avoid the "Bold effect"
	// when freezed

	// Disable MultimediaSource and all of its children
	//setDisabled(f);
}


/// ***********************************************************************************************************************
/// Methods for TitleLabel
/// ***********************************************************************************************************************
TitleLabel::TitleLabel(QWidget *parent, int w, int h, int w_offset, int h_offset, bool scrolling) :
	QLabel("", parent)
{
	// Style
	setFixedWidth(w);
	setFixedHeight(h);
	setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// init
	time_per_step    = 333;
	visible_chars    = 18;  // how many chars can be shown at the same time
	_text_length     = 0;   // total chars
	current_shift    = 0;   // current chars shifted
	step_shift       = 1;   // char per step
	_w_offset        = w_offset;
	_h_offset        = h_offset;

	// separator for ciclic text scrolling
	separator        = "   --   ";

	// define if it is scrolling label or not
	_scrolling = scrolling;
	
	// connect timer to scroll text
	connect( &scrolling_timer, SIGNAL( timeout() ), this, SLOT( handleScrollingTimer() ) );
}

void TitleLabel::drawContents( QPainter *p )
{
	p->translate(_w_offset, _h_offset);
	QLabel::drawContents( p );
}

void TitleLabel::resetTextPosition()
{
	current_shift = 0;
}

void TitleLabel::setText( const QString & text_to_set )
{
	// store full string and full lenght
	_text         = text_to_set;
	_text_length  = text_to_set.length();
	current_shift = 0;
	
	// call method of ancestor
	QLabel::setText( text_to_set );

	// start the timer if scroll is needed
	if ( _scrolling == TRUE && _text_length>visible_chars )
		scrolling_timer.start( time_per_step );
	else
		scrolling_timer.stop();
}

void TitleLabel::refreshText()
{
	QString banner_string = QString("%1%2").arg(_text).arg(separator);

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
	if (current_shift <_text_length + separator.length())
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
	/// Create main Layout
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->setSpacing(0);

	/// Set the number of elements shown
	this->rows_per_page = rows_per_page;

	/// Create labels_layout
	QVBoxLayout *labels_layout = new QVBoxLayout( main_layout );
	labels_layout->setMargin(0);
	labels_layout->setSpacing(0);

	/// Create labels and add them to label_layout
	// WARNING Quick and Dirty alignment using offsets of TitleLabel
	QValueVector<int> h_offsets;
	h_offsets.append(-4);
	h_offsets.append(-2);
	h_offsets.append(-1);
	h_offsets.append(0);
	labels_list.resize(rows_per_page);
	for (int i = 0; i < rows_per_page; i++)
	{
		// Create label and add it to labels_layout
		labels_list.insert( i, new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], TRUE) );
		labels_layout->addWidget( labels_list[i] );
	}

	/// Create buttons_bar
	buttons_bar = new ButtonsBar(this, rows_per_page, Qt::Vertical);

	/// Set Icons for buttons_bar (using icons_library cache)
	QPixmap *icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("arrrg.png") );
	QPixmap *pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("arrrgp.png") );
	for (int i = 0; i < rows_per_page; i++)
		buttons_bar->setButtonIcons(i, *icon, *pressed_icon);

	/// Add buttons_bar to main_layout
	main_layout->addWidget( buttons_bar );

	/// Create Set File Browsing Props
	files_handler.setSorting(QDir::DirsFirst | QDir::Name);
	files_handler.setMatchAllDirs(TRUE);
	files_handler.setNameFilter("*.[mM][pP]3;*.[wW][[aA][vV];*.[oO][gG][gG];*.[wW][mM][aA]");

	/// Set Style
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

	/// Create playing_window and set style
	playing_window = new AudioPlayingWindow(this);
	playing_window->setBGColor(paletteBackgroundColor());
	playing_window->setFGColor(paletteForegroundColor());
	playing_window->setPalette(palette());
	playing_window->setFont(font());

	/// Connette il FilesBrowser con se stesso per il CLICK
	connect(buttons_bar, SIGNAL(clicked(int)), this, SLOT(itemIsClicked(int)));

	/// Connect signal to notify start and stop play and to send relative frames
	connect(playing_window, SIGNAL(notifyStartPlay()), this, SIGNAL(notifyStartPlay()));
	connect(playing_window, SIGNAL(notifyStopPlay()), this, SIGNAL(notifyStopPlay()));
}

void FileBrowser::showEvent( QShowEvent *event )
{
	for (int i = 0; i < rows_per_page; i++)
		labels_list[i]->resetTextPosition();
}

void FileBrowser::itemIsClicked(int item)
{
	qDebug(QString("FileBrowser::itemIsClicked -> %1 is clicked and index is %2").arg(item).arg(pages_indexes[current_path]));
	QFileInfo *clicked_element;
	int absolute_position_item;

	/// REVERSE SEARCH
	// we have the number of the pressed button we want the file
	absolute_position_item = pages_indexes[current_path] + item;
	if ( absolute_position_item < files_list.count() )
		clicked_element = files_list[pages_indexes[current_path] + item];
	else
		return;

	if ( clicked_element->exists())
	{
		if (clicked_element->isDir() )
		{
			level++;
			browseFiles(clicked_element->absFilePath());
		}
		else
		{
			/// Load play list in playing window and show it
			playing_window->startPlay(files_list, clicked_element);
			playing_window->show();
		}
	}
	
	// this is added to browse again the current path and verify if something is changed
	// it is useful if some file is removed so the current directory is refreshed
	qDebug( QString("Current absFilePath is %1").arg( QFileInfo(current_path).absFilePath() ) );
	browseFiles();
}

void FileBrowser::browseUp()
{
	if (level)
	{
		level--;
		//browseFiles(current_path + "/..");
		browseFiles( QFileInfo(current_path + "/..").absFilePath() );
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

void FileBrowser::browseFiles(QString new_path)
{
	// if new_path is valid changes the path and run browsFiles()
	if ( QFileInfo(new_path).exists() )
	{
		QString new_path_string = QFileInfo(new_path).absFilePath();
		// change path
		files_handler.setPath( new_path_string );
		current_path = new_path_string;
		
	}
	browseFiles();
}

void FileBrowser::browseFiles()
{
	// refresh QDir information
	files_handler.refresh();

	// Clear list of pointers (so we can destroy the old files list owned by QDir object)
	files_list.clear();

	// Create fileslist from files
	const QFileInfoList *temp_files_list = files_handler.entryInfoList();
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
}

void FileBrowser::showFiles()
{
	unsigned start = pages_indexes[current_path];
	unsigned end   = std::min(start+rows_per_page, files_list.count());

	for (int i = 0; i < labels_list.size(); ++i)
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
	if (pages_indexes[current_path] < files_list.count()-rows_per_page)
		pages_indexes[current_path] += rows_per_page;
	showFiles();
}

void FileBrowser::prevItem()
{
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
	qDebug("AudioPlayingWindow costructor");

	/// set self Geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);

	/// Create Labels (that contain tags)
 	QFont font( "helvetica", 18 );

	// create Labels containing INFO
	//labels_list.insert( i, new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], TRUE) );
	meta_title_label  = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, TRUE );
	meta_artist_label = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, TRUE );
	meta_album_label  = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, TRUE );
	time_pos_label    = new TitleLabel( this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, TRUE );
	// Set Font
	meta_title_label->setFont(font);
	meta_artist_label->setFont(font);
	meta_album_label->setFont(font);
	time_pos_label->setFont(font);

	/// Create Main Layout
	// all others layout must have this as parent, this is not more needed in Qt4
	// where we can use setMainLayout
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QString audioplay_title = app_config.get(CFG_LABELS_MEDIAPLAYER "status", "Playing audio").c_str();
	TitleLabel *window_title_label = new TitleLabel( this, MAX_WIDTH, 120, 9, 0, TRUE );
	window_title_label->setText( audioplay_title );
	window_title_label->setFont(font);

	QVBoxLayout *title_layout  = new QVBoxLayout(main_layout);
	title_layout->addWidget(window_title_label);

	// layouts for both labels
	QHBoxLayout *tags_layout = new QHBoxLayout(main_layout);
	QVBoxLayout *tags_name_layout = new QVBoxLayout(tags_layout);

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
	QVBoxLayout *tags_text_layout = new QVBoxLayout(tags_layout);
	tags_text_layout->addWidget(meta_title_label);
	tags_text_layout->addWidget(meta_artist_label);
	tags_text_layout->addWidget(meta_album_label);
	tags_text_layout->addWidget(time_pos_label);


	/// Create ButtonsBar and set its geometry
	buttons_bar = new ButtonsBar(this, 4, Qt::Horizontal);
	buttons_bar->setGeometry(0, MAX_HEIGHT- MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);


	/// Set Icons for ButtonsBar (using icons_library cache)
	QPixmap *icon;
	QPixmap *pressed_icon;

	icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("arrlf.png") );
	pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("arrlfp.png") );
	buttons_bar->setButtonIcons(0, *icon, *pressed_icon);

	icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnbackward.png") );
	pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnbackwardp.png") );
	buttons_bar->setButtonIcons(1, *icon, *pressed_icon);

	icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnforward.png") );
	pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnforwardp.png") );
	buttons_bar->setButtonIcons(2, *icon, *pressed_icon);

	icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstop.png") );
	pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstopp.png") );
	buttons_bar->setButtonIcons(3, *icon, *pressed_icon);

	/// add ButtonsBar to main_layout
	main_layout->addWidget(buttons_bar);

	/// Create Timer
	data_refresh_timer = new QTimer(this);

	/// Create MediaPlayer Object (to Handle MPLAYER Process)
	media_player = new MediaPlayer(this);

	/// Connect buttons_bar to the handler
	connect( buttons_bar, SIGNAL(clicked(int )), this, SLOT(handle_buttons(int )) );

	/// Connect the timer to the handler, the timer get and displays data from mplayer
	connect( data_refresh_timer, SIGNAL(timeout()), this, SLOT(handle_data_refresh_timer()) );

	/// Connect the mplayerTerminated signal from Mediaplayer to AudioPlayingWindow
	connect( media_player, SIGNAL(mplayerTerminated()), this, SLOT(handle_playing_terminated()) );

	/// Set Timer
	refresh_time = 500;
}

void AudioPlayingWindow::startPlay(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element)
{
	/// send the quit command to mediaplayer to stop and terminate it if it is already playing
	media_player->quit();

	/// fill play_list and set current track
	int     track_number = 0;
	QString track_name;
	for (int i = 0; i < files_list.count(); i++)
	{
		if (files_list[i]->isDir())
			continue;

		track_name = files_list[i]->absFilePath().latin1();
		play_list.append(track_name);
		if (clicked_element->absFilePath().latin1()==track_name)
			current_track = track_number;
		track_number++;
	}

	/// Turn On Audio System
	turnOnAudioSystem();

	/// Start playing and point next Track
	cleanPlayingInfo();
	media_player->play(play_list[current_track]);
	current_track++;
	
	/// Start Timer
	data_refresh_timer->start(refresh_time);

	// Change Button Icon
	QPixmap *icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstop.png") );
	QPixmap *pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstopp.png") );
	buttons_bar->setButtonIcons(3, *icon, *pressed_icon);

	qDebug(QString("Nome Traccia: %1, numero: %2/%3").arg(play_list[current_track-1]).arg(current_track).arg(play_list.count()));
}

void AudioPlayingWindow::turnOnAudioSystem()
{
	/// Start Play Scripts
	int rc;

	if ( (rc = system("/bin/audio_on.tcl")) != 0 )
		qDebug("MediaPlayer::play error on audio_on.tcl SCRIPT, exit code %d", rc);

	qDebug("************************************************************ Running START PLAY SCRIPT");
	emit notifyStartPlay();
}

void AudioPlayingWindow::turnOffAudioSystem()
{
	/// Stop Play Scripts
	int rc;

	if ( (rc = system("/bin/audio_off.tcl")) != 0)
		qDebug("MediaPlayer::play error on audio_off.tcl SCRIPT");

	qDebug("************************************************************ Running STOP PLAY SCRIPT");
	emit notifyStopPlay();
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


void AudioPlayingWindow::prevTrack()
{
	/// go to prev track
	/// We set new current track and we just kill the process
	// ATTENZIONE, dato che dopo il play ci posizioniamo già sulla traccia successiva, è
	// necessario tornare indietro di due tracce se possibile
	if (current_track>1)
	{
		current_track--;
		current_track--;
		media_player->killMPlayer();
	}

	// Se la current_track è uguale a 1 vuol dire che stiamo suonando la traccia zero
	// il questo caso la riportiamo all'inizio
	else if ( current_track==1 )
	{
		current_track = 0;
		media_player->killMPlayer();
	}
	qDebug(QString("AudioPlayingWindow::prevTrack() now playing: %2/%3").arg(current_track-1).arg(play_list.count()));
}


void AudioPlayingWindow::nextTrack()
{
	/// go to next track
	/// We set new current track and we just kill the process
	// ATTENZIONE, dato che dopo il play ci posizioniamo già sulla traccia successiva, è
	// necessario uccidere il processo se esiste una traccia successiva
	if ( current_track<play_list.count() )
	{
		//current_track++;
		media_player->killMPlayer();
	}
	qDebug(QString("AudioPlayingWindow::nextTrack() now playing: %2/%3").arg(current_track-1).arg(play_list.count()));
}


void AudioPlayingWindow::handle_buttons(int button_number)
{
	qDebug(QString("AudioPlayingWindow::handle_buttons() BUTTON %1 PRESSED").arg(button_number));

	data_refresh_timer->stop();

	switch (button_number)
	{
	case 0:
		// don't stop, just go back
		hide();
		break;
	case 1:
		// go to prev track
		prevTrack();
		break;
	case 2:
		// go to next track
		nextTrack();
		break;
	case 3:
		if (media_player->isPaused())
		{
			qDebug("media_player: resume play");
			turnOnAudioSystem();
			media_player->resume();
			
			// Change Button Icon
			QPixmap *icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstop.png") );
			QPixmap *pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstopp.png") );
			buttons_bar->setButtonIcons(3, *icon, *pressed_icon);
		}
		else
		{
			qDebug("media_player: pause play");
			turnOffAudioSystem();
			media_player->pause();
			
			// Change Button Icon
			QPixmap *icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstart.png") );
			QPixmap *pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("btnstartp.png") );
			buttons_bar->setButtonIcons(3, *icon, *pressed_icon);
		}
		break;
	}

	data_refresh_timer->start(refresh_time);
}

void AudioPlayingWindow::handle_data_refresh_timer()
{
	qDebug("AudioPlayingWindow::handle_data_refresh_timer()");
	refreshPlayingInfo();
}


void AudioPlayingWindow::handle_playing_terminated()
{
	// mplayer is terminated because the track is finished or because some error occurs
	// so we go to the next track if exists
	qDebug("AudioPlayingWindow::handle_playing_terminated()");
	int rc;
	
	if ( current_track > -1 && current_track < play_list.count() )
	{
		cleanPlayingInfo();
		media_player->play(play_list[current_track]);
		// una volta lanciata una traccia si posiziona sulla successiva
		// se quella appena mandata in play è l'ultima la prossima volta 
		// che viene lanciata questa funzione non verrà eseguito il play
		current_track++;
	}
	else
	{
		// In questo caso ha finito di suonare l'ultimo pezzo e si chiude
		turnOffAudioSystem();

		// Hide playing_window
		hide();
	}
}

void AudioPlayingWindow::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	buttons_bar->setBGColor(c);
}

void AudioPlayingWindow::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_bar->setFGColor(c);
}



/// ***********************************************************************************************************************
/// Methods for ButtonsBar
/// ***********************************************************************************************************************

ButtonsBar::ButtonsBar(QWidget *parent, int number_of_buttons, Orientation orientation) :
QWidget(parent, 0, WStyle_NoBorder | WStyle_Customize)
//,  WStyle_NoBorder | WType_TopLevel | WStyle_Customize)
{
	/// Create ButtonGroup, this can handle QButton objects
	buttons_group = new QButtonGroup(number_of_buttons, orientation, this);
	// Permettono la corretta disposizione dei pulsanti
	buttons_group->setInsideMargin(0);
	buttons_group->setInsideSpacing(0);
	// Elimina il bordo bianco
	buttons_group->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	/// Init Vector to store BtButton pointers
	buttons_list.clear();
	buttons_list.resize(number_of_buttons);

	for (uint i = 0; i < number_of_buttons; i++)
	{
		// the BtButton is created and inserted in ButtonGroup automatically because it is
		// its parent. We also need to store the pointer in the button_list vector because
		// when we call find method of buttons group we obtain a QButton pointer and no a
		// BtButton pointer!
		buttons_list.insert( i, new BtButton(buttons_group, QString("Button%1").arg(i)) );
	}

	// Senza il seguente Layout la barra non viene visualizzata in modo corretto
	if (orientation == Horizontal)
	{
		QHBoxLayout *main_layout = new QHBoxLayout(this);//, 0, 0);
		main_layout->addWidget(buttons_group);
	}
	if (orientation == Vertical)
	{
		QVBoxLayout *main_layout = new QVBoxLayout(this);//, 0, 0);
		main_layout->addWidget(buttons_group);
		main_layout->addStretch();
	}

	/// Connect Signal
	connect( buttons_group, SIGNAL(clicked(int)), this, SIGNAL(clicked(int)) );
}

bool ButtonsBar::setButtonIcons(int button_number, const QPixmap &icon, const QPixmap &pressed_icon)
{
	if (!buttons_list.at(button_number))
		return false;

	buttons_list.at(button_number)->setPixmap(icon);
	buttons_list.at(button_number)->setPressedPixmap(pressed_icon);
	// Non è necessario il setGeometry o il resize dei pulsanti,
	// ma è molto importante nel costruttore del ButtonGroup indicare il numero di pulsanti
	return true;
}

void ButtonsBar::showButton(int idx)
{
	buttons_list[idx]->show();
}

void ButtonsBar::hideButton(int idx)
{
	buttons_list[idx]->hide();
}

void ButtonsBar::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	buttons_group->setPaletteBackgroundColor(c);

	for (uint i = 0; i < buttons_list.size(); i++)
		buttons_list[i]->setPaletteBackgroundColor(c);

}
void ButtonsBar::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_group->setPaletteForegroundColor(c);

	for (uint i = 0; i < buttons_list.size(); i++)
		buttons_list[i]->setPaletteForegroundColor(c);
}


