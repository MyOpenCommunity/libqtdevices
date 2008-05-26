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
#include <qptrvector.h>
#include <qvaluevector.h>
#include <qlabel.h>
#include <qtimer.h>

class FileBrowser;
class PlayWindow;
class QSlider;
class QPainter;
class QFileInfo;
class bannFrecce;
class ButtonsBar;
class BtButton;

/**
 * \class MultimediaSource
 *
 * This class implemets the Page for MULTIMEDIA SOURCE
 */
class  MultimediaSource : public QWidget
{
Q_OBJECT
public:
	MultimediaSource(QWidget *parent=0, const char *name=0, const char *amb="", int where_address=0);

	/*!
	 * \brief Sets the background color for the banner.
	 * The arguments are RGB components for the color.
	 */
	void setBGColor(int, int , int );
	/*!
	 * \brief Sets the foreground color for the banner.
	 * The arguments are RGB components for the color.
	 */
	void setFGColor(int , int , int );
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
	int setBGPixmap(char* );
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

	/// Browser
	FileBrowser *filesWindow;

	char amb[80];
	char nome[15];
	bannFrecce *bannNavigazione;
	QLabel *label;
	bool audio_initialized;
	int where_address;
};


/**
 * \class TitleLabel
 *
 * this class is derived from QLabel
 * and reimplements drawContent to have scrolling text
 */
class TitleLabel : public QLabel
{
Q_OBJECT
public:
	TitleLabel( QWidget *parent = 0, int w = 0, int h = 0, int w_offset = 0, int h_offset = 0, bool scrolling = FALSE, WFlags f = 0 );

	void setText( const QString & text_to_set );
	void drawContents ( QPainter *p );
	void resetTextPosition();

	void setMaxVisibleChars(int n);

private:
	// internal data
	QString text;

	// Total chars
	unsigned text_length;

	int     w_offset;
	int     h_offset;
	bool    scrolling;

	// Text scrolling Timer
	QTimer scrolling_timer;

	/*
	 * Text scrolling parameters.
	 */
	int time_per_step;
	// current chars shifted
	unsigned current_shift;
	// how many chars can be shown at the same time_shift
	unsigned visible_chars;

	QString separator;

	/// refresh text
	void refreshText();

public slots:
	void handleScrollingTimer();
};


/**
 * \class FileBrowser
 *
 * implements a File Browsing Windows with special methods
 * to navigate and to play files.
 */
class  FileBrowser : public QWidget
{
Q_OBJECT
public:
	FileBrowser(QWidget *parent, unsigned rows_per_page, const char *name=0, WFlags f=0);

	/**
	 * Browse given path, return false in case of error.
	 */
	bool browseFiles(QString new_path);

	/**
	 * Browse current path, return false in case of error.
	 */
	bool browseFiles();

	/// Store current path
	QString current_path;

	/// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	/// before to show itself some init is done.
	void showEvent(QShowEvent *event);

public slots:
	void nextItem();
	void prevItem();
	void itemIsClicked(int);
	void browseUp();

private:
	void showFiles();
	QString getTextRepresentation(QFileInfo *file_info);
	unsigned                 rows_per_page;
	QDir                     files_handler;

	// How many subdirs we are descending from root.
	unsigned                 level;

	// The followinw Vector stores pointers to QfileInfo objects.
	// (the Qt class: QPtrVector contains pointers so it is not needed to define it as QPtrVector<QFileInfo*>)
	QPtrVector<QFileInfo>    files_list;
	QMap<QString, unsigned>  pages_indexes;

	/// Widgets
	// Pointer to labels used to visualize files
	//QPtrVector<QLabel>       labels_list;
	QPtrVector<TitleLabel>   labels_list;
	ButtonsBar               *buttons_bar;
	TitleLabel               *no_files_label;

signals:
	void notifyStartPlay();
	void notifyStopPlay();
	void notifyExit();
	void startPlayer(QValueVector<QString> play_list, unsigned element);
};

#endif // MULTIMEDIA_SOURCE_H
