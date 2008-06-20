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
#include <qregexp.h>

#include "multimedia_source.h"
#include "playwindow.h"
#include "bannfrecce.h"
#include "listbrowser.h"
#include "buttons_bar.h"
#include "fontmanager.h"
#include "mediaplayer.h"
#include "btbutton.h"
#include "main.h"


#define BROWSER_ROWS_PER_PAGE 4

/*
 * Scripts launched before and after a track is played.
 */
static const char *start_play_script = "/bin/audio_on.tcl";
static const char *stop_play_script = "/bin/audio_off.tcl";


// Interface icon paths.
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_SELECT_P = IMG_PATH "arrrgp.png";
static const char *IMG_BACK = IMG_PATH "arrlf.png";
static const char *IMG_BACK_P = IMG_PATH "arrlfp.png";


SourceChoice::SourceChoice(QWidget *parent, const char *name) : QWidget(parent, name)
{
	QFont aFont;
	FontManager::instance()->getFont(font_listbrowser, aFont);
	setFont(aFont);

	unsigned num_choices = 2;
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);

	// Create labels_layout
	QVBoxLayout *labels_layout = new QVBoxLayout();
	labels_layout->setMargin(0);
	labels_layout->setSpacing(0);

	TitleLabel *l = new TitleLabel(this, MAX_WIDTH - 60, 50, 9, 5);
	l->setText(tr("IP Radio"));
	labels_layout->addWidget(l);

	l = new TitleLabel(this, MAX_WIDTH - 60, 50, 9, 15);
	l->setText(tr("Servers"));
	labels_layout->addWidget(l);
	labels_layout->addStretch();

	// Create buttons_bar
	buttons_bar = new ButtonsBar(this, num_choices, Qt::Vertical);

	// Set Icons for buttons_bar (using icons_library cache)
	QPixmap *icon         = icons_library.getIcon(IMG_SELECT);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_SELECT_P);
	for (unsigned i = 0; i < num_choices; ++i)
		buttons_bar->setButtonIcons(i, *icon, *pressed_icon);

	QHBoxLayout *main_controls = new QHBoxLayout();
	back_btn = new BtButton(this, "back_btn");
	back_btn->setPixmap(*icons_library.getIcon(IMG_BACK));
	back_btn->setPressedPixmap(*icons_library.getIcon(IMG_BACK_P));
	main_controls->addWidget(back_btn);

	QGridLayout *main_layout = new QGridLayout(this, 4, 2, 1);
	main_layout->addMultiCell(new QSpacerItem(MAX_WIDTH, 20), 0, 0, 0, 1);
	main_layout->addLayout(labels_layout, 1, 0);
	main_layout->addWidget(buttons_bar, 1, 1);
	main_layout->addMultiCellLayout(main_controls, 2, 2, 0, 1, Qt::AlignLeft);
	main_layout->addMultiCell(new QSpacerItem(MAX_WIDTH, 10), 3, 3, 0, 1);

	connect(back_btn, SIGNAL(released()), SIGNAL(Closed()));
	connect(buttons_bar, SIGNAL(clicked(int)), SIGNAL(clicked(int)));
}

void SourceChoice::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	buttons_bar->setBGColor(c);
	back_btn->setPaletteBackgroundColor(c);
}

void SourceChoice::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_bar->setFGColor(c);
	back_btn->setPaletteForegroundColor(c);
}


MultimediaSource::MultimediaSource(QWidget *parent, const char *name, const char *amb, int _where_address) :
	QWidget(parent, name),
	audio_initialized(true)
{
	// Set main geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	where_address = _where_address;
	qDebug("[AUDIO] MultimediaSource ctor: where_address is %d", _where_address);

	// Create Banner Standard di Navigazione (scroll degli Items e la possibilità di tornare indietro)
	bannNavigazione = new bannFrecce(this, "bannerfrecce", 4, ICON_DIFFSON);
	bannNavigazione->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	source_choice = new SourceChoice(this, name);
	source_choice->hide();
	source_type = NONE_SOURCE;
	media_player = new MediaPlayer(this);
	play_window = 0;
	selector = 0;

	connect(source_choice, SIGNAL(Closed()), SLOT(handleClose()));
	connect(source_choice, SIGNAL(clicked(int)), SLOT(handleChoiceSource(int)));

	loadRadioNode();
}

void MultimediaSource::loadRadioNode()
{
	bool diff_multi = true;
	QDomNode node_page = getPageNode(DIFSON_MULTI);
	if (node_page.isNull())
	{
		node_page = getPageNode(DIFSON);
		diff_multi = false;
	}

	if (node_page.isNull())
	{
		qDebug("[AUDIO] ERROR loading configuration");
		return;
	}

	int id = diff_multi ? SORGENTE_MULTIM_MC : SORGENTE_MULTIM;
	QDomNode n = getChildWithId(node_page, QRegExp("item\\d{1,2}"), id);
	if (!n.isNull())
	{
		QDomNode node = n.firstChild();
		while (!node.isNull() && node.nodeName() != "web_radio")
			node = node.nextSibling();

		if (!node.isNull())
			radio_node = node;
	}
}

void MultimediaSource::sourceMenu(AudioSourceType t)
{
	source_type = t;

	if (play_window)
		play_window->deleteLater();

	if (source_type == RADIO_SOURCE)
		play_window = new RadioPlayWindow(media_player, this);
	else
		play_window = new MediaPlayWindow(media_player, this);

	play_window->setBGColor(paletteBackgroundColor());
	play_window->setFGColor(paletteForegroundColor());
	play_window->setPalette(palette());
	play_window->setFont(font());

	if (selector)
		selector->deleteLater();

	if (source_type == RADIO_SOURCE)
		selector = new RadioSelector(this, BROWSER_ROWS_PER_PAGE, radio_node);
	else
		selector = new FileSelector(this, BROWSER_ROWS_PER_PAGE, MEDIASERVER_PATH);

	selector->setBGColor(paletteBackgroundColor());
	selector->setFGColor(paletteForegroundColor());

	// Pulsanti up, down e back
	connect(bannNavigazione, SIGNAL(downClick()), selector, SLOT(prevItem()));
	connect(bannNavigazione, SIGNAL(upClick()), selector, SLOT(nextItem()));
	connect(bannNavigazione, SIGNAL(backClick()), selector, SLOT(browseUp()));
	connect(bannNavigazione, SIGNAL(forwardClick()), SLOT(handleClose()));

	// Connection to be notified about Start and Stop Play
	connect(this, SIGNAL(notifyStartPlay()), SLOT(handleStartPlay()));
	connect(this, SIGNAL(notifyStopPlay()), SLOT(handleStopPlay()));
	connect(play_window, SIGNAL(notifyStopPlay()), SIGNAL(notifyStopPlay()));
	connect(play_window, SIGNAL(settingsBtn()), SLOT(handleClose()));
	connect(play_window, SIGNAL(backBtn()), SLOT(handlePlayerExit()));

	connect(selector, SIGNAL(notifyExit()), SLOT(handleSelectorExit()));

	connect(selector, SIGNAL(startPlayer(QValueVector<AudioData>, unsigned)),
			SLOT(startPlayer(QValueVector<AudioData>, unsigned)));
}

void MultimediaSource::handleClose()
{
	source_choice->hide();
	bannNavigazione->setHidden(false);
	if (source_type != NONE_SOURCE)
	{
		play_window->hide();
		selector->hide();
	}
	emit Closed();
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

void MultimediaSource::nextTrack()
{
	if (source_type != NONE_SOURCE)
		play_window->nextTrack();
}

void MultimediaSource::prevTrack()
{
	if (source_type != NONE_SOURCE)
		play_window->prevTrack();
}

void MultimediaSource::stop()
{
	if (source_type != NONE_SOURCE)
		play_window->stop();
}

void MultimediaSource::pause()
{
	if (source_type != NONE_SOURCE)
		play_window->pause();
}

void MultimediaSource::resume()
{
	if (source_type != NONE_SOURCE)
		play_window->resume();
}

void MultimediaSource::showPage()
{
	// draw and show itself
	draw();
	showFullScreen();

	if (source_type != NONE_SOURCE && play_window->isPlaying())
		play_window->show();
	else if (radio_node.isNull())
	{
		sourceMenu(FILE_SOURCE);
		selector->show();
	}
	else
	{
		bannNavigazione->setHidden(true);
		source_choice->show();
	}
}

void MultimediaSource::handlePlayerExit()
{
	play_window->hide();
	selector->show();
}

void MultimediaSource::handleSelectorExit()
{
	if (radio_node.isNull())
		handleClose();
	else
	{
		selector->hide();
		bannNavigazione->setHidden(true);
		source_choice->show();
	}
}

void MultimediaSource::handleChoiceSource(int button_id)
{
	// GIANNI: temporanei, fare un enum!
	int BUTTON_RADIO = 0;
	int BUTTON_MEDIA = 1;

	// Create the instances only if change the source type
	if (button_id == BUTTON_RADIO && source_type != RADIO_SOURCE)
	{
		sourceMenu(RADIO_SOURCE);
		play_window->stop();
	}
	else if (button_id == BUTTON_MEDIA && source_type != FILE_SOURCE)
	{
		sourceMenu(FILE_SOURCE);
		play_window->stop();
	}

	bannNavigazione->setHidden(false);
	source_choice->hide();

	if (play_window->isPlaying())
		play_window->show();
	else
		selector->show();
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
	source_choice->setBGColor(c);
}
void MultimediaSource::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	bannNavigazione->setFGColor(c);
	source_choice->setFGColor(c);
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
	qDebug("[AUDIO] Running start play script: %s", start_play_script);

	int rc;
	if ((rc = system(start_play_script)) != 0)
		qDebug("[AUDIO] Error on start play script, exit code %d", WEXITSTATUS(rc));

	if(send_frame)
		emit notifyStartPlay();
}

void MultimediaSource::disableSource(bool send_frame)
{
	qDebug("[AUDIO] Running stop play script: %s", stop_play_script);

	int rc;
	if ((rc = system(stop_play_script)) != 0)
		qDebug("[AUDIO] Error on stop play script, exit code %d", rc);

	if(send_frame)
		emit notifyStopPlay();
}

void MultimediaSource::startPlayer(QValueVector<AudioData> list, unsigned element)
{
	selector->hide();
	play_window->startPlayer(list, element);
	play_window->show();
}

/// ***********************************************************************************************************************
/// Methods for FileSelector
/// ***********************************************************************************************************************

FileSelector::FileSelector(QWidget *parent, unsigned rows_per_page, QString start_path, const char *name, WFlags f) :
	Selector(parent, name, f)
{
	level = 0;
	list_browser = new ListBrowser(this, rows_per_page, name, f);
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE);
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->addWidget(list_browser);

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setMatchAllDirs(true);
	current_dir.setNameFilter("*.[mM][pP]3;*.[wW][[aA][vV];*.[oO][gG][gG];*.[wW][mM][aA]");

	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));

	// Start to Browse Files
	if (!browseFiles(start_path))
	{
		// FIXME display error?
	}
}

void FileSelector::showEvent(QShowEvent *event)
{
	if (!browseFiles())
	{
		// FIXME display error?
		emit notifyExit();
	}
}

void FileSelector::itemIsClicked(int item)
{
	QString filename = files_list[item];
	qDebug("[AUDIO] FileSelector::itemIsClicked %d -> %s", item, filename.ascii());
	QFileInfo clicked_element(current_dir, filename);
	if (!clicked_element.exists())
		qDebug("[AUDIO] Error retrieving file by name: %s", filename.ascii());

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
		QValueVector<AudioData> play_list;
		unsigned element = 0;
		unsigned track_number = 0;

		for (unsigned i = 0; i < files_list.count(); ++i)
		{
			QFileInfo fn(current_dir, files_list[i]);
			if (fn.isDir())
				continue;

			play_list.append(AudioData(fn.absFilePath().latin1()));
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

/// ***********************************************************************************************************************
/// Methods for RadioSelector
/// ***********************************************************************************************************************

RadioSelector::RadioSelector(QWidget *parent, unsigned rows_per_page, QDomNode config, const char *name, WFlags f) :
	Selector(parent, name, f)
{
	list_browser = new ListBrowser(this, rows_per_page, name, f);
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE);
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->addWidget(list_browser);

	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));

	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(QRegExp("item\\d{1,2}")))
		{
			QString descr, url;
			QDomNode child = n.firstChild();
			while (!child.isNull())
			{
				if (child.nodeName() == "descr")
					descr = child.toElement().text();
				else if (child.nodeName() == "url")
					url = child.toElement().text();

				if (descr.length() > 0 && url.length() > 0)
					break;
				child = child.nextSibling();
			}
			if (descr.length() > 0 && url.length() > 0)
				radio_list.append(AudioData(url, descr));
			else
				qDebug("[AUDIO] Error loading radio item %s", n.nodeName().ascii());
		}
		n = n.nextSibling();
	}

	QValueVector<QString> list;
	for (unsigned i = 0; i < radio_list.count(); ++i)
		list.append(radio_list[i].desc);

	list_browser->setList(list);
	list_browser->showList();
}

void RadioSelector::itemIsClicked(int item)
{
	qDebug("[AUDIO] RadioSelector::itemIsClicked %d -> %s", item, radio_list[item].path.ascii());
	emit startPlayer(radio_list, item);
}

void RadioSelector::browseUp()
{
	emit notifyExit();
}

void RadioSelector::nextItem()
{
	list_browser->nextItem();
}

void RadioSelector::prevItem()
{
	list_browser->prevItem();
}

void RadioSelector::setBGColor(QColor c)
{
	list_browser->setBGColor(c);
}

void RadioSelector::setFGColor(QColor c)
{
	list_browser->setFGColor(c);
}
