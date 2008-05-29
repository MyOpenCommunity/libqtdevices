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
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2007
 */

#ifndef PLAY_WINDOW_H
#define PLAY_WINDOW_H

#include "multimedia_source.h"

#include <qwidget.h>
#include <qvaluevector.h>

class MediaPlayer;
class ButtonsBar;
class BtButton;
class TitleLabel;
class QBoxLayout;

/**
 * \class PlayWindow
 *
 * This class implemets the Play Window,
 * it is called from fileBrowser (that is part of MultimediaSource Page)
 * when file (song) is clicked
 */
class  PlayWindow : public QWidget
{
Q_OBJECT
public:
	virtual ~PlayWindow();
	// Apply Style
	virtual void setBGColor(QColor c);
	virtual void setFGColor(QColor c);
	
	// Play control
	void nextTrack();
	void prevTrack();
	virtual void pause();
	virtual void resume();
	void stop();

	/// Return true if a song is currently active, even if in pause.
	bool isPlaying();

	// Run script and send frame to turn on and off Audio System
	void turnOnAudioSystem(bool send_frame);
	void turnOffAudioSystem(bool send_frame);

	/// Start PLAY, begins to play and sets the play_list
	void startPlayer(QValueVector<AudioData> _play_list, unsigned element);

protected:
	// To make PlayWindow an abstract class.
	PlayWindow(QWidget *parent = 0, const char * name = 0);
	virtual void startPlayer(unsigned int);
	virtual void stopPlayer();
	void playNextTrack();

	void addMainControls(QBoxLayout* layout);

	/// Return the description of current song/radio played.
	QString getCurrentDescription();

	/// Media player
	MediaPlayer *media_player;

	/// Main control
	BtButton    *back_btn, *settings_btn;

	/// Main layout
	QBoxLayout *main_layout;

	/// To read media player output
	bool read_player_output;

protected slots:
	// MediaPlayer Handlers
	virtual void handlePlayingDone();
	virtual void handlePlayingKilled() {}
	void handlePlayingAborted();

signals:
	void settingsBtn();
	void backBtn();
	void notifyStartPlay();
	void notifyStopPlay();

private:
	QValueVector<AudioData> play_list;

	/*
	 * current_track is the track played by mplayer.
	 * next_track is the track to be played by next mplayer instance.
	 * CURRENT_TRACK_NONE means no track has to be played.
	 */
	unsigned int current_track;
	unsigned int next_track;
	static const unsigned CURRENT_TRACK_NONE = UINT_MAX;
};


class MediaPlayWindow : public PlayWindow
{
Q_OBJECT
public:
	MediaPlayWindow(QWidget *parent = 0, const char * name = 0);

	// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	// Play control
	void pause();
	void resume();

protected slots:
	// MediaPlayer Handlers
	void handlePlayingDone();
	void handlePlayingKilled();

protected:
	void stopPlayer();
	void startPlayer(unsigned int);

private slots:
	/// Method to Get and Visualize playing INFO from MPlayer
	void refreshPlayInfo();
	void handleButtons(int);

private:
	void addTextLabels(QBoxLayout *layout, QFont& aFont);
	void addNameLabels(QBoxLayout *layout, QFont& aFont);

	/// Play buttons
	ButtonsBar  *play_controls;

	void showPlayBtn();
	void showPauseBtn();

	/// Clean play INFO from MPlayer
	void cleanPlayInfo();

	/// Stores current playing info
	QMap<QString, QString> playing_info;

	/// refreshing info time interval
	int refresh_time;

	/// Labels
	TitleLabel *file_name_label,   *percent_pos_label,
	           *time_length_label, *time_pos_label,
	           *meta_title_label,  *meta_artist_label,
	           *meta_album_label,  *meta_track_label;

	/// Timer to refresh data from MediaPlayer
	QTimer *data_refresh_timer;
};


class RadioPlayWindow : public PlayWindow
{
Q_OBJECT
public:
	RadioPlayWindow(QWidget *parent = 0, const char * name = 0);

	// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

protected:
	void startPlayer(unsigned int);

private slots:
	void handleButtons(int);

private:
	void addTextLabels(QBoxLayout *layout, QFont& aFont);
	void addNameLabels(QBoxLayout *layout, QFont& aFont);

	/// Play buttons
	ButtonsBar  *play_controls;
	TitleLabel *meta_title_label;
};

#endif
