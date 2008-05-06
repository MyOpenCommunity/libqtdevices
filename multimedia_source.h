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
#include <qmap.h>
#include <qptrvector.h>
#include <qvaluevector.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qpainter.h>
#include <limits>

#include "main.h"
#include "buttons_bar.h"


class FileBrowser;
class AudioPlayingWindow;
class MediaPlayer;


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
private:
	/// Player screen
	AudioPlayingWindow *playing_window;

	/// Filer browser
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
	FileBrowser(QWidget *parent, AudioPlayingWindow *, unsigned rows_per_page, const char *name=0, WFlags f=0);

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

	/// Show and hide playing windows
	void showPlayingWindow();
	void hidePlayingWindow();

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
	AudioPlayingWindow       *playing_window;
	TitleLabel               *no_files_label;

signals:
	void notifyStartPlay();
	void notifyStopPlay();
	void notifyExit();
};


/**
 * \class AudioPlayingWindow
 *
 * This class implemets the Playing Window,
 * it is called from fileBrowser (that is part of MultimediaSource Page)
 * when file (song) is clicked
 */
class  AudioPlayingWindow : public QWidget
{
Q_OBJECT
public:
	AudioPlayingWindow(QWidget *parent = 0, const char * name = 0);
	~AudioPlayingWindow() {};

	// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	// Play control
	void nextTrack();
	void prevTrack();
	void pauseOrResume();
	void stop();

	/// Stores current playing info
	QMap<QString, QString> playing_info;

	/// Start PLAY, begins to play a given track and sets the play_list
	void startPlay(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element);

	// Run script and send frame to turn on and off Audio System
	void turnOnAudioSystem(bool send_frame);
	void turnOffAudioSystem(bool send_frame);

	/// Return true if a song is currently active, even if in pause.
	bool isPlaying();

	/// Return true if a song is in pause.
	bool isPaused();

public slots:
	void handle_buttons(int);
	void handleBackBtn();
	void handleSettingsBtn();
	void handle_data_refresh_timer();
	void handlePlayingDone();
	void handlePlayingAborted();

private:
	/// Method to Get and Visualize playing INFO from MPlayer
	void refreshPlayingInfo();

	/// Clean playing INFO from MPlayer
	void cleanPlayingInfo();

	// Change status of play/pause button in control bar
	void generatePlaylist(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element);
	void startMediaPlayer();
	void stopMediaPlayer();
	void showPlayBtn();
	void showPauseBtn();

	/// refreshing info time interval
	int refresh_time;

	/**
	 * The differences between files_list and play_list are:
	 *  - play_list contains QString with the full path
	 *  - files_list does not contain dirs
	 */
	QValueVector<QString> play_list;

	/*
	 * Track to be played by next mplayer instance.
	 * CURRENT_TRACK_NONE means no track has to be played.
	 */
	unsigned int current_track;
	static const unsigned CURRENT_TRACK_NONE = UINT_MAX;

	/// Widgets
	ButtonsBar  *play_controls;
	MediaPlayer *media_player;
	BtButton    *back_btn, *settings_btn;

	/// Labels
	TitleLabel *file_name_label,   *percent_pos_label,
	           *time_length_label, *time_pos_label,
	           *meta_title_label,  *meta_artist_label,
	           *meta_album_label,  *meta_track_label;

	/// Timer to refresh data from MediaPlayer
	QTimer *data_refresh_timer;

signals:
	void notifyStartPlay();
	void notifyStopPlay();

	// User released settings button
	void settingsBtn();
};

#endif // MULTIMEDIA_SOURCE_H
