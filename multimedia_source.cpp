/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** MultimediaSource.cpp
 **
 ** finestra di dati sulla sorgente MultimediaSource
 **
 ****************************************************************/

#include "multimedia_source.h"
#include "playwindow.h"
#include "bannfrecce.h"
#include "listbrowser.h"
#include "fontmanager.h" // bt_global::font
#include "mediaplayer.h"
#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "titlelabel.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QApplication> // processEvents
#include <QButtonGroup>
#include <QStringList>
#include <QLayout>
#include <QRegExp>
#include <QDebug>
#include <QTime>

#define BROWSER_ROWS_PER_PAGE 4

/*
 * Scripts launched before and after a track is played.
 */
static const char *start_play_script = "/bin/audio_on.tcl";
static const char *stop_play_script = "/bin/audio_off.tcl";


// Interface icon paths.
static const char *IMG_SELECT = IMG_PATH "arrrg.png";


enum ChoiceButtons
{
	BUTTON_RADIO = 0,
	BUTTON_MEDIA
};


SourceChoice::SourceChoice()
{
	setFont(bt_global::font->get(FontManager::TEXT));
	buttons_group = new QButtonGroup(this);

	TitleLabel *l = new TitleLabel(0, width() - 60, 50, 9, 5);
	l->setText(tr("IP Radio"));
	addHorizontalBox(main_layout, l, BUTTON_RADIO);

	l = new TitleLabel(0, width() - 60, 50, 9, 15);
	l->setText(tr("Servers"));
	addHorizontalBox(main_layout, l, BUTTON_MEDIA);

	addBackButton();
	connect(buttons_group, SIGNAL(buttonClicked(int)), SIGNAL(clicked(int)));
}

void SourceChoice::addHorizontalBox(QBoxLayout *layout, QLabel *label, int id_btn)
{
	QHBoxLayout *box = new QHBoxLayout();
	box->addWidget(label, 0, Qt::AlignLeft);
	BtButton *btn = new BtButton();
	btn->setImage(IMG_SELECT);
	box->addWidget(btn, 0, Qt::AlignRight);
	box->setContentsMargins(5, 0, 0, 0);
	buttons_group->addButton(btn, id_btn);
	layout->addLayout(box);
}


MultimediaSource::MultimediaSource(const QDomNode &config_node, int _where_address)
{
	audio_initialized = true;
	where_address = _where_address;
	qDebug("[AUDIO] MultimediaSource ctor: where_address is %d", _where_address);

	source_choice = new SourceChoice;
	source_type = NONE_SOURCE;
	media_player = new MediaPlayer(this);
	play_window = 0;
	selector = 0;

	connect(source_choice, SIGNAL(Closed()), SIGNAL(Closed()));
	connect(source_choice, SIGNAL(clicked(int)), SLOT(handleChoiceSource(int)));

	loadSources(config_node);
}

MultimediaSource::~MultimediaSource()
{
	delete source_choice;
}

void MultimediaSource::loadSources(const QDomNode &config_node)
{
	radio_enabled = getTextChild(config_node, "radiooip").toInt() == 1;
	mediaserver_enabled = getTextChild(config_node, "mediaserver").toInt() == 1;

	radio_node = getChildWithName(config_node, "web_radio");

	// Check for correctness
	if (radio_enabled && radio_node.isNull())
		radio_enabled = false;

	if (!radio_enabled && !mediaserver_enabled)
		mediaserver_enabled = true;
}

void MultimediaSource::sourceMenu(AudioSourceType t)
{
	source_type = t;

	if (play_window)
		play_window->deleteLater();

	if (source_type == RADIO_SOURCE)
		play_window = new RadioPlayWindow(media_player);
	else
		play_window = new MediaPlayWindow(media_player);

	if (selector)
		selector->deleteLater();

	if (source_type == RADIO_SOURCE)
		selector = new RadioSelector(BROWSER_ROWS_PER_PAGE, radio_node);
	else
		selector = new AudioFileSelector(BROWSER_ROWS_PER_PAGE, MEDIASERVER_PATH);

	// Connection to be notified about Start and Stop Play
	connect(this, SIGNAL(notifyStartPlay()), SLOT(handleStartPlay()));
	connect(this, SIGNAL(notifyStopPlay()), SLOT(handleStopPlay()));
	connect(play_window, SIGNAL(notifyStopPlay()), SIGNAL(notifyStopPlay()));
	connect(play_window, SIGNAL(settingsBtn()), SIGNAL(Closed()));
	connect(play_window, SIGNAL(Closed()), SLOT(handlePlayerExit()));

	connect(selector, SIGNAL(notifyExit()), SLOT(handleSelectorExit()));
	connect(selector, SIGNAL(Closed()), SIGNAL(Closed()));

	connect(selector, SIGNAL(startPlayer(QVector<AudioData>, unsigned)),
			SLOT(startPlayer(QVector<AudioData>, unsigned)));
}

void MultimediaSource::initAudio()
{
	// Now init is done by BtMain.cpp calling dm->inizializza()
	qDebug("[AUDIO] MultimediaSource::initAudio()");
	// perform Audio Init
	if (!audio_initialized)
	{
		sendFrame(QString("*#22*7*#15*%1***4**0**1***0##").arg(where_address));
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
	if (source_type != NONE_SOURCE && play_window->isPlaying())
		play_window->showPage();
	else if (mediaserver_enabled && !radio_enabled)
	{
		sourceMenu(FILE_SOURCE);
		selector->showPage();
	}
	else if (radio_enabled && !mediaserver_enabled)
	{
		sourceMenu(RADIO_SOURCE);
		selector->showPage();
	}
	else
		source_choice->showPage();
}

void MultimediaSource::handlePlayerExit()
{
	selector->showPage();
}

void MultimediaSource::handleSelectorExit()
{
	if (!radio_enabled || !mediaserver_enabled)
		emit Closed();
	else
		source_choice->showPage();
}

void MultimediaSource::handleChoiceSource(int button_id)
{
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
	Q_ASSERT_X(play_window, "MultimediaSource::handleChoiceSource", "PlayWindow not set!");

	if (play_window->isPlaying())
		play_window->showPage();
	else
		selector->showPage();
}

void MultimediaSource::handleStartPlay()
{
	sendFrame(QString("*22*1#4#1*2#%1##").arg(where_address));
}

void MultimediaSource::handleStopPlay()
{
	sendFrame(QString("*22*0#4#1*2#%1##").arg(where_address));
}

void MultimediaSource::enableSource(bool send_frame)
{
	qDebug("[AUDIO] Running start play script: %s", start_play_script);

	int rc;
	if ((rc = system(start_play_script)) != 0)
		qWarning("[AUDIO] Error on start play script, exit code %d", WEXITSTATUS(rc));

	if (send_frame)
		emit notifyStartPlay();
}

void MultimediaSource::disableSource(bool send_frame)
{
	qDebug("[AUDIO] Running stop play script: %s", stop_play_script);

	int rc;
	if ((rc = system(stop_play_script)) != 0)
		qWarning("[AUDIO] Error on stop play script, exit code %d", rc);

	if (send_frame)
		emit notifyStopPlay();
}

void MultimediaSource::startPlayer(QVector<AudioData> list, unsigned element)
{
	play_window->startPlayer(list, element);
	play_window->showPage();
}


AudioFileSelector::AudioFileSelector(unsigned rows_per_page, QString start_path)
	: FileSelector(rows_per_page, start_path)
{
	ListBrowser *list_browser = new ListBrowser(this, rows_per_page);
	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	bannFrecce *nav_bar = new bannFrecce(this, 4, ICON_DIFFSON);
	connect(nav_bar, SIGNAL(downClick()), SLOT(prevItem()));
	connect(nav_bar, SIGNAL(upClick()), SLOT(nextItem()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	buildPage(list_browser, nav_bar);

	browseDirectory(start_path);
}

void AudioFileSelector::startPlayback(int item)
{
	QVector<AudioData> play_list;
	unsigned element = 0;
	unsigned track_number = 0;

	const QList<QFileInfo> &files_list = getFiles();
	QFileInfo clicked_element = files_list[item];

	for (int i = 0; i < files_list.size(); ++i)
	{
		const QFileInfo& fn = files_list[i];
		if (fn.isDir())
			continue;

		play_list.append(AudioData(fn.absoluteFilePath(), fn.completeBaseName()));
		if (clicked_element.absoluteFilePath() == fn.absoluteFilePath())
			element = track_number;

		++track_number;
	}
	emit startPlayer(play_list, element);
}

bool AudioFileSelector::browseFiles(const QDir &directory, QList<QFileInfo> &files)
{
	QStringList filters;
	filters << "*.[mM]3[uU]" << "*.[mM][pP]3" << "*.[wW][[aA][vV]" << "*.[oO][gG][gG]" << "*.[wW][mM][aA]";

	// Create fileslist from files
	QList<QFileInfo> temp_files_list = directory.entryInfoList(filters);

	if (temp_files_list.empty())
	{
		qDebug() << "[AUDIO] empty directory: " << directory.absolutePath();
		return false;
	}

	files.clear();

	QVector<QString> names_list;

	for (int i = 0; i < temp_files_list.size(); ++i)
	{
		const QFileInfo& f = temp_files_list.at(i);
		if (f.fileName() != "." && f.fileName() != "..")
		{
			names_list.append(f.fileName());
			files.append(f);
		}
	}

	page_content->setList(names_list, displayedPage(directory));
	page_content->showList();

	return true;
}

int AudioFileSelector::currentPage()
{
	return page_content->getCurrentPage();
}

void AudioFileSelector::nextItem()
{
	page_content->nextItem();
}

void AudioFileSelector::prevItem()
{
	page_content->prevItem();
}


RadioSelector::RadioSelector(unsigned rows_per_page, QDomNode config)
{
	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 0);
	main_layout->setSpacing(0);

	main_layout->setContentsMargins(0, 0, 0, 0);
	list_browser = new ListBrowser(this, rows_per_page);
	connect(list_browser, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	main_layout->addWidget(list_browser, 1);

	bannFrecce *nav_bar = new bannFrecce(this, 4, ICON_DIFFSON);
	connect(nav_bar, SIGNAL(downClick()), SLOT(prevItem()));
	connect(nav_bar, SIGNAL(upClick()), SLOT(nextItem()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));
	main_layout->addWidget(nav_bar);

	foreach (const QDomNode &item, getChildren(config, "item"))
	{
		QString descr = getTextChild(item, "descr");
		QString url = getTextChild(item, "url");
		radio_list.append(AudioData(url, descr));
	}

	QVector<QString> list;
	for (int i = 0; i < radio_list.size(); ++i)
		list.append(radio_list[i].desc);

	list_browser->setList(list);
	list_browser->showList();
}

void RadioSelector::itemIsClicked(int item)
{
	qDebug() << "[AUDIO] RadioSelector::itemIsClicked " << item << " -> " << radio_list[item].path;
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
