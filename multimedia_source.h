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

class ListBrowser;
class FileSelector;
class PlayWindow;
class QLabel;
class bannFrecce;

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
	 * The arguments are RGB components for the color.
	 */
	void setBGColor(int, int, int);
	/*!
	 * \brief Sets the foreground color for the banner.
	 * The arguments are RGB components for the color.
	 */
	void setFGColor(int, int, int);
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
	 * \brief Sets the background pixmap for the banner.
	 */
	int setBGPixmap(char*);
	/*!
	 * \brief Sets amb. description
	 */
	void setAmbDescr(char *) {}
	/*!
	 * \brief Draws the page
	 */
	void draw() {};

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
	/*!
	 * \brief Shows the aux details page
	 */
	void showAux();

	/// handles to receive play and stop notifications
	void handleStartPlay();
	void handleStopPlay();

	void startPlayer(QValueVector<QString> play_list, unsigned element);
private:
	/// Player screen
	PlayWindow *play_window;

	FileSelector *selector;

	char amb[80];
	char nome[15];
	bannFrecce *bannNavigazione;
	QLabel *label;
	bool audio_initialized;
	int where_address;
};


/**
 * \class FileSelector
 *
 * implements a File Selector Windows with special methods
 * to navigate and to play files.
 */
class  FileSelector : public QWidget
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

	void itemIsClicked(QString item);
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

signals:
	void notifyStartPlay();
	void notifyStopPlay();
	void notifyExit();
	void startPlayer(QValueVector<QString> play_list, unsigned element);
};

#endif
