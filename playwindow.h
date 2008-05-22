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

#include <qwidget.h>
#include <qptrvector.h>
#include <qvaluevector.h>

class MediaPlayer;
class QFileInfo;
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
	PlayWindow(QWidget *parent = 0, const char * name = 0);
	~PlayWindow() {};

	// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	// Play control
	void nextTrack();
	void prevTrack();
	void pause();
	void resume();
	void stop();

	/// Stores current playing info
	QMap<QString, QString> playing_info;

	/// Start PLAY, begins to play a given track and sets the play_list
	void startNewPlaylist(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element);

	// Run script and send frame to turn on and off Audio System
	void turnOnAudioSystem(bool send_frame);
	void turnOffAudioSystem(bool send_frame);

	/// Return true if a song is currently active, even if in pause.
	bool isPlaying();

public slots:
	void handle_buttons(int);
	void handleBackBtn();
	void handleSettingsBtn();
	void handle_data_refresh_timer();
	void handlePlayingDone();
	void handlePlayingKilled();
	void handlePlayingAborted();

protected:
	// FIX: temporaneamente qui, da spostare
	void addTextLabels(QBoxLayout *layout, QFont& aFont);
	void addNameLabels(QBoxLayout *layout, QFont& aFont);
	
	/// Clean play INFO from MPlayer
	void cleanPlayInfo();

	/// Method to Get and Visualize playing INFO from MPlayer
	void refreshPlayInfo();

	// Change status of play/pause button in control bar
	void generatePlaylist(QPtrVector<QFileInfo> files_list, QFileInfo *clicked_element);
	void startMediaPlayer(unsigned int);
	virtual void startPlayer(QString track);
	void stopMediaPlayer();
	void playNextTrack();
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
	 * current_track is the track played by mplayer.
	 * next_track is the track to be played by next mplayer instance.
	 * CURRENT_TRACK_NONE means no track has to be played.
	 */
	unsigned int current_track;
	unsigned int next_track;
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


class MediaPlayWindow : public PlayWindow
{
public:
	MediaPlayWindow(QWidget *parent = 0, const char * name = 0) : PlayWindow(parent, name) {}
	~MediaPlayWindow() {};

};


class RadioPlayWindow : public PlayWindow
{
public:
	RadioPlayWindow(QWidget *parent = 0, const char * name = 0) : PlayWindow(parent, name) {}
	~RadioPlayWindow() {};
	void startPlayer(QString track);
private:
	/// Start a radio stream
	void startRadioStream(QString name, QString url);
};


#endif
