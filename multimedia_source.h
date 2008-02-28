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

#include <qframe.h>
#include <qprocess.h>
#include <qlcdnumber.h>
#include <qcursor.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qmap.h>
#include <qptrvector.h>
#include <qvaluevector.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtextedit.h>

//#include "btbutton.h"
#include "main.h"
#include "btlabel.h"
#include "bannfrecce.h"
#include "bann3but.h"



class FileBrowser;
class AudioPlayingWindow;
class ButtonsBar;
class MediaPlayer;
class AudioPlayingWindow;


/** \class MultimediaSource
 *  This class implemets the Page for MULTIMEDIA SOURCE
 *
 */
class  MultimediaSource : public QWidget
{
Q_OBJECT
public:
	MultimediaSource( QWidget *parent=0, const char *name=0, const char *amb="", int where_address=0);
	//void browseFiles(QString);
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
	void setAmbDescr(char *);
	/*!
	 * \brief Draws the page
	 */
	void draw();
	/*!
	 * File Browsing Windows
	 */
	FileBrowser *filesWindow;

	BtLabel *auxName, *ambDescr;
	//BtButton *fwdBut, *prova_button;
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
	/*!
	 * \brief Disables/enables everything
	 */
	void freezed(bool);
	/*!
	 * \brief Handle long pression of back key.
	 */
	void handleLongBackPress();
	/// handles to receive play and stop notifications
	void handleStartPlay();
	void handleStopPlay();
private:
	char amb[80];
	char nome[15];
	bannFrecce *bannNavigazione;
	QLabel *label;
	bool audio_initialized;
	int where_address;
};


/** \class TitleLabel
 *  this class is derived from QLabel
 *  and reimplements drawContent to have scrolling text
 */
class TitleLabel : public QLabel
{
Q_OBJECT
public:
	TitleLabel( QWidget *parent = 0, int w = 0, int h = 0, int w_offset = 0, int h_offset = 0, bool scrolling = FALSE );
	
	void setText( const QString & text_to_set );
	void drawContents ( QPainter *p );
	void resetTextPosition();

	void setMaxVisibleChars(int n);

private:
	/// internal data
	QString _text;
	int     _text_length;
	int     _w_offset;
	int     _h_offset;
	bool    _scrolling;

	/// text scrolling Timer
	QTimer scrolling_timer;

	/// text scrolling parameters
	int time_per_step;
	int current_shift;
	int step_shift;
	int visible_chars;
	QString separator;
	
	/// refresh text
	void refreshText();

public slots:
	void handleScrollingTimer();
};


/** \class FileBrowser
 *  This class implemets a File Browsing Windows (derived from QListBox) with special methods
 *  to navigate and to play files
 */
class  FileBrowser : public QWidget
{
Q_OBJECT
public:
	FileBrowser(QWidget *parent=0, unsigned rows_per_page=3, const char *name=0, WFlags f=0);
	
	/// Browse given path
	void browseFiles(QString new_path);
	void browseFiles();

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
	QMap<QString, int>       pages_indexes;

	/// Widgets
	// Pointer to labels used to visualize files
	//QPtrVector<QLabel>       labels_list;
	QPtrVector<TitleLabel>       labels_list;
	ButtonsBar               *buttons_bar;
	AudioPlayingWindow       *playing_window;

signals:
	void notifyStartPlay();
	void notifyStopPlay();
	void notifyExit();
};


/** \class AudioPlayingWindow
 *  This class implemets the Playing Window,
 *  it is called from fileBrowser (that is part of MultimediaSource Page)
 *  when file (song) is clicked
 *
 */
class  AudioPlayingWindow : public QWidget
{
Q_OBJECT
public:
	AudioPlayingWindow(QWidget *parent = 0, const char * name = 0);
	~AudioPlayingWindow() {};
	
	/// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	/// Change Track
	void nextTrack();
	void prevTrack();

	/// Stores current playing info
	QMap<QString, QString> playing_info;
	
	/// Start PLAY, begins to play a given track and sets the play_list
	void startPlay(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element);

	/// Run script and send frame to turn on and off Audio System
	void turnOnAudioSystem();
	void turnOffAudioSystem();
	
public slots:
	void handle_buttons(int);
	void handle_data_refresh_timer();
	void handle_playing_terminated();

private:
	/// refreshing info time interval
	int refresh_time;

	/** The differences between files_list and play_list are:
	 *     - play_list contains QString with the full path
	 *     - play_list does not contain dirs
	 */
	QValueVector<QString>    play_list;
	int                      current_track;

	/// Widgets
	QSlider                  *time_slider;
	ButtonsBar               *buttons_bar;
	MediaPlayer              *media_player;

	/// Labels
	TitleLabel               *file_name_label,   *percent_pos_label, *time_length_label, *time_pos_label;
	TitleLabel               *meta_title_label,  *meta_artist_label, *meta_album_label,  *meta_track_label;

	/// Timer to refresh data from MediaPlayer
	QTimer                   *data_refresh_timer;

	/// Method to Get and Visualize playing INFO from MPlayer
	void refreshPlayingInfo();

	/// Clean playing INFO from MPlayer
	void cleanPlayingInfo();

signals:
	void notifyStartPlay();
	void notifyStopPlay();
};


/** \class ButtonsBar
 *  this class implements a buttons bar
 */
class ButtonsBar : public QWidget
{
Q_OBJECT
public:
	ButtonsBar(QWidget *parent = 0, int number_of_buttons = 1, Orientation orientation = Horizontal);
	bool setButtonIcons(int button_number, const QPixmap &icon, const QPixmap &pressed_icon);
	void showButton(int idx);
	void hideButton(int idx);
	void setBGColor(QColor c);
	void setFGColor(QColor c);
private:
	QPtrVector<BtButton>  buttons_list;
	QButtonGroup         *buttons_group;
signals:
	/// The signal pressed(int ) of QButton group is connected to the following by the constructor
	void clicked(int button_number);
};

#endif // MULTIMEDIA_SOURCE_H
