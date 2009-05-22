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

#include <QApplication>
#include <QButtonGroup>
#include <QStringList>
#include <QDomNode>
#include <QLayout>
#include <QRegExp>
#include <QDebug>
#include <QTime>

#include <assert.h>

#define BROWSER_ROWS_PER_PAGE 4

#define MEDIASERVER_MSEC_WAIT_TIME 2000

/*
 * Scripts launched before and after a track is played.
 */
static const char *start_play_script = "/bin/audio_on.tcl";
static const char *stop_play_script = "/bin/audio_off.tcl";


// Interface icon paths.
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_WAIT = IMG_PATH "loading.png";


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


enum ChoiceButtons
{
	BUTTON_RADIO = 0,
	BUTTON_MEDIA
};


SourceChoice::SourceChoice()
{
	setFont(bt_global::font->get(FontManager::TEXT));
	buttons_group = new QButtonGroup(this);

	TitleLabel *l = new TitleLabel(0, MAX_WIDTH - 60, 50, 9, 5);
	l->setText(tr("IP Radio"));
	addHorizontalBox(main_layout, l, BUTTON_RADIO);

	l = new TitleLabel(0, MAX_WIDTH - 60, 50, 9, 15);
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
		selector = new FileSelector(BROWSER_ROWS_PER_PAGE, MEDIASERVER_PATH);

	// Connection to be notified about Start and Stop Play
	connect(this, SIGNAL(notifyStartPlay()), SLOT(handleStartPlay()));
	connect(this, SIGNAL(notifyStopPlay()), SLOT(handleStopPlay()));
	connect(play_window, SIGNAL(notifyStopPlay()), SIGNAL(notifyStopPlay()));
	connect(play_window, SIGNAL(settingsBtn()), SIGNAL(Closed()));
	connect(play_window, SIGNAL(backBtn()), SLOT(handlePlayerExit()));

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
	assert(play_window && "PlayWindow not set!");

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


FileSelector::FileSelector(unsigned rows_per_page, QString start_path)
{
	level = 0;
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

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

	QStringList filters;
	filters << "*.[mM]3[uU]" << "*.[mM][pP]3" << "*.[wW][[aA][vV]" << "*.[oO][gG][gG]" << "*.[wW][mM][aA]";
	current_dir.setNameFilters(filters);
	changePath(start_path);
}

void FileSelector::showPage()
{
	// refresh QDir information
	current_dir.refresh();

	QLabel *l = createWaitDialog();
	QTime time_counter = startTimeCounter();

	if (!browseFiles())
	{
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
		emit notifyExit();
		return;
	}

	waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
	destroyWaitDialog(l);

	Selector::showPage();
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
		if (!browseFiles(clicked_element.absoluteFilePath()))
		{
			destroyWaitDialog(l);
			emit notifyExit();
			return;
		}
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
	}
	else
	{
		QVector<AudioData> play_list;
		unsigned element = 0;
		unsigned track_number = 0;

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
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
		emit startPlayer(play_list, element);
	}
}

void FileSelector::browseUp()
{
	if (level)
	{
		--level;
		QLabel *l = createWaitDialog();
		QTime time_counter = startTimeCounter();

		if (!browseFiles(QFileInfo(current_dir, "..").absoluteFilePath()))
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

bool FileSelector::browseFiles(QString new_path)
{
	QString old_path = current_dir.absolutePath();
	if (changePath(new_path))
	{
		if (!browseFiles())
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
	return browseFiles();
}

bool FileSelector::changePath(QString new_path)
{
	// if new_path is valid changes the path and run browseFiles()
	if (QFileInfo(new_path).exists())
	{
		// save the info of old directory
		pages_indexes[current_dir.absolutePath()] = list_browser->getCurrentPage();

		QString new_path_string = QFileInfo(new_path).absoluteFilePath();
		// change path
		current_dir.setPath(new_path_string);
		return true;
	}
	return false;
}

void FileSelector::destroyWaitDialog(QLabel *l)
{
	l->hide();
	l->deleteLater();
}

QLabel *FileSelector::createWaitDialog()
{
	QLabel* l = new QLabel(parentWidget());
	QPixmap *icon = bt_global::icons_cache.getIcon(IMG_WAIT);
	l->setPixmap(*icon);

	QRect r = icon->rect();
	r.moveCenter(QPoint(MAX_WIDTH / 2, MAX_HEIGHT / 2));
	l->setGeometry(r);

	l->show();
	qApp->processEvents();
	return l;
}

bool FileSelector::browseFiles()
{
	// Create fileslist from files
	QList<QFileInfo> temp_files_list = current_dir.entryInfoList();

	if (temp_files_list.empty())
	{
		qDebug() << "[AUDIO] empty directory: " << current_dir.absolutePath();
		return false;
	}

	files_list.clear();

	QVector<QString> names_list;

	for (int i = 0; i < temp_files_list.size(); ++i)
	{
		const QFileInfo& f = temp_files_list.at(i);
		if (f.fileName() != "." && f.fileName() != "..")
		{
			names_list.append(f.fileName());
			files_list.append(f);
		}
	}

	unsigned page = 0;
	if (pages_indexes.contains(current_dir.absolutePath()))
		page = pages_indexes[current_dir.absolutePath()];

	list_browser->setList(names_list, page);
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


RadioSelector::RadioSelector(unsigned rows_per_page, QDomNode config)
{
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
