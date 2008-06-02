/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the management of the multimedia source page (interface)
 *
 * This class defines the visualization properties for MULTIMEDIA Audio Source page
 * it has been done starting from the aux audio source class, done by Cimiagi
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \date December 2007
 */

#ifndef MULTIMEDIA_SOURCE_H
#define MULTIMEDIA_SOURCE_H

#include <qdir.h>
#include <qvaluevector.h>
#include <qwidget.h>
#include <qdom.h>

class ListBrowser;
class Selector;
class PlayWindow;
class MediaPlayer;
class QLabel;
class bannFrecce;
class ButtonsBar;
class BtButton;

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


class SourceChoice : public QWidget
{
Q_OBJECT
public:
	SourceChoice(QWidget *parent, const char *name);
	void setBGColor(QColor c);
	void setFGColor(QColor c);

signals:
	void clicked(int);
	void Closed();

private:
	ButtonsBar *buttons_bar;
	BtButton   *back_btn;
};


/**
 * \class MultimediaSource
 *
 * This class implemets the Page for MULTIMEDIA SOURCE
 */
class MultimediaSource : public QWidget
{
Q_OBJECT
public:
	MultimediaSource(QWidget *parent=0, const char *name=0, const char *amb="", int where_address=0);

	/*!
	 * \brief Sets the background color for the banner.
	 * The argument is the QColor description of the color.
	 */
	void setBGColor(QColor);
	/*!
	 * \brief Sets the foreground color for the banner.
	 * The argument is the QColor description of the color.
	 */
	void setFGColor(QColor);
	/*!
	 * \brief Sets amb. description
	 */
	void setAmbDescr(char *) {}

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
	/*!
	 * \brief Send open frame.
	 */
	void sendFrame(char *);

public slots:
	void showPage();

private:
	void sourceMenu(AudioSourceType t);
	/*!
	 * \brief Sets the background color for the banner.
	 * The arguments are RGB components for the color.
	 */
	void setBGColor(int, int, int);
	/*!
	 * \brief Sets the foreground color for the banner.
	 * The arguments are RGB components for the color.
	 */
	void setFGColor(int, int, int);
	/*!
	 * \brief Sets the background pixmap for the banner.
	 */
	int setBGPixmap(char*);
	/*!
	 * \brief Draws the page
	 */
	void draw() {};

	void loadRadioNode();

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

private slots:
	/// handles to receive play and stop notifications
	void handleStartPlay();
	void handleStopPlay();
	void startPlayer(QValueVector<AudioData> play_list, unsigned element);

	void handleChoiceSource(int button_id);
	void handleSelectorExit();
	void handlePlayerExit();
	void handleClose();
};


class Selector : public QWidget
{
Q_OBJECT
public:
	Selector(QWidget *parent, const char *name=0, WFlags f=0) : QWidget(parent, name, f) {}
	virtual void setBGColor(QColor c) = 0;
	virtual void setFGColor(QColor c) = 0;

public slots:
	virtual void nextItem() = 0;
	virtual void prevItem() = 0;

	virtual void itemIsClicked(int item) = 0;
	virtual void browseUp() = 0;

signals:
	virtual void notifyExit();
	virtual void startPlayer(QValueVector<AudioData> play_list, unsigned element);
};


/**
 * \class FileSelector
 *
 * implements a File Selector Windows with special methods
 * to navigate and to play files.
 */
class  FileSelector : public Selector
{
Q_OBJECT
public:
	FileSelector(QWidget *parent, unsigned rows_per_page, QString start_path, const char *name=0, WFlags f=0);

	/// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

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

	QValueVector<QString>    files_list;

	QMap<QString, unsigned>  pages_indexes;

	/// The listBrowser instance, used to display files.
	ListBrowser *list_browser;

	/// Browse current path, return false in case of error.
	bool browseFiles();

	/// Browse given path, return false in case of error.
	bool browseFiles(QString new_path);
};


class RadioSelector : public Selector
{
Q_OBJECT
public:
	RadioSelector(QWidget *parent, unsigned rows_per_page, QDomNode config, const char *name=0, WFlags f=0);

	/// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

public slots:
	void nextItem();
	void prevItem();

	void itemIsClicked(int item);
	void browseUp();

private:
	QValueVector<AudioData> radio_list;

	/// The listBrowser instance, used to display files.
	ListBrowser *list_browser;
};

#endif
