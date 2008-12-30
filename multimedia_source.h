/**
 * \file
 * <!--
 * Copyright 2007, 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the management of the multimedia part
 *
 * This class is the manager of the multimedia part, that is used to choose and
 * play music (internet radio or song through upnp server). The classes involved
 * are:
 * - SourceChoice, that shows a menu to choose between radio or media server
 *   sources.
 *
 * - the interface Selector, implemented by RadioSelector and FileSelector, to
 *   choose the radio/file to play.
 *
 * - the abstract class PlayWindow, implemented by RadioPlayWindow and
 *   MediaPlayWindow, to play music using the MediaPlayer class.
 *
 * - the MediaPlayer, a wrapper around mplayer.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef MULTIMEDIA_SOURCE_H
#define MULTIMEDIA_SOURCE_H

#include "page.h"

#include <QDir>
#include <QMap>
#include <QVector>
#include <QWidget>
#include <QDomNode>

class ListBrowser;
class Selector;
class PlayWindow;
class MediaPlayer;
class bannFrecce;
class BtButton;

class QLabel;
class QButtonGroup;
class QBoxLayout;

enum AudioSourceType
{
	RADIO_SOURCE,
	FILE_SOURCE,
	NONE_SOURCE // Only for inizialization
};


struct AudioData
{
	QString path;
	QString desc;
	AudioData(QString p = "", QString d = "") { path = p; desc = d; }
};

/**
 * \class SourceChoice
 *
 * This class show the menu to choice from Radio Over Ip and Media Server
 */
class SourceChoice : public Page
{
Q_OBJECT
public:
	SourceChoice(QWidget *parent);

signals:
	void clicked(int);
	void Closed();

private:
	void addHorizontalBox(QBoxLayout *layout, QLabel *label, int id_btn);
	BtButton *back_btn;
	QButtonGroup *buttons_group;
};


/**
 * \class MultimediaSource
 *
 * This class implements the logic for MULTIMEDIA SOURCE
 */
class MultimediaSource : public QWidget
{
Q_OBJECT
public:
	MultimediaSource(QWidget *parent, int where_address);
	/*!
	 * \brief Sets amb. description
	 */
	void setAmbDescr(const QString &) {}

	/*
	 * \brief Enable/Disable audio source
	 */
	void enableSource(bool send_frame);
	void disableSource(bool send_frame);

	/// Init Audio System
	void initAudio();

	/*
	 * Play control
	 */
	void nextTrack();
	void prevTrack();
	void pause();
	void resume();
	void stop();

signals:
	/*!
	 * \brief Emitted when the page is going to be closed
	 */
	void Closed();
	/*!
	 * \brief Emitted when fwd button is pressed
	 */
	void Btnfwd();

	void notifyStartPlay();
	void notifyStopPlay();

public slots:
	void showPage();

private:
	void sourceMenu(AudioSourceType t);
	/*!
	 * \brief Draws the page
	 */
	void draw() {};

	void loadSources();

	PlayWindow *play_window;
	Selector *selector;
	MediaPlayer *media_player;

	char amb[80];
	char nome[15];
	bannFrecce *bannNavigazione;
	QLabel *label;
	bool audio_initialized;
	int where_address;

	AudioSourceType source_type;
	SourceChoice *source_choice;
	QDomNode radio_node;
	bool radio_enabled, mediaserver_enabled;

private slots:
	/// handles to receive play and stop notifications
	void handleStartPlay();
	void handleStopPlay();
	void startPlayer(QVector<AudioData> play_list, unsigned element);

	void handleChoiceSource(int button_id);
	void handleSelectorExit();
	void handlePlayerExit();
	void handleClose();
};

/**
 * \class Selector
 *
 * Realize a common interface for all selector classes.
 *
 */
class Selector : public QWidget
{
Q_OBJECT
public:
	Selector(QWidget *parent) : QWidget(parent) {}

public slots:
	virtual void nextItem() = 0;
	virtual void prevItem() = 0;

	virtual void itemIsClicked(int item) = 0;
	virtual void browseUp() = 0;

signals:
	virtual void notifyExit();
	virtual void startPlayer(QVector<AudioData> play_list, unsigned element);
};


/**
 * \class FileSelector
 *
 * implements a File Selector Windows with methods to navigate and play files.
 */
class  FileSelector : public Selector
{
Q_OBJECT
public:
	FileSelector(QWidget *parent, unsigned rows_per_page, QString start_path);

public slots:
	void nextItem();
	void prevItem();

	void itemIsClicked(int item);
	void browseUp();

private:
	/// before to show itself some init is done.
	void showEvent(QShowEvent *event);

	void showFiles();

	/// The handler of current directory
	QDir current_dir;

	// How many subdirs we are descending from root.
	unsigned level;

	QVector<QFileInfo> files_list;

	QMap<QString, unsigned>  pages_indexes;

	/// The listBrowser instance, used to display files.
	ListBrowser *list_browser;

	/// Browse current path, return false in case of error.
	bool browseFiles();

	/// Browse given path, return false in case of error.
	bool browseFiles(QString new_path);

	/// Change the current dir, return false in case of error.
	bool changePath(QString new_path);

	QLabel *createWaitDialog();
	void destroyWaitDialog(QLabel *l);
};


/**
 * \class RadioSelector
 *
 * implements a Radio Selector Windows with methods to navigate and play radio.
 */
class RadioSelector : public Selector
{
Q_OBJECT
public:
	RadioSelector(QWidget *parent, unsigned rows_per_page, QDomNode config);

public slots:
	void nextItem();
	void prevItem();

	void itemIsClicked(int item);
	void browseUp();

private:
	QVector<AudioData> radio_list;

	/// The listBrowser instance, used to display files.
	ListBrowser *list_browser;
};

#endif
