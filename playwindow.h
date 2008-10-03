/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contains all classes related to PlayWindow class.
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2007
 */

#ifndef PLAY_WINDOW_H
#define PLAY_WINDOW_H

#include "multimedia_source.h"

#include <QWidget>
#include <QVector>
#include <QString>

#include <limits.h> // UINT_MAX

class MediaPlayer;
class ButtonsBar;
class BtButton;
class TitleLabel;
class QBoxLayout;

/**
 * \class PlayWindow
 *
 * This (abstract) class realize the Play Window, called by MultimediaSource
 * when an item is clicked.
 */
class  PlayWindow : public QWidget
{
Q_OBJECT
public:
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

	void startPlayer(QVector<AudioData> _play_list, unsigned element);

protected:
	// To make PlayWindow an abstract class.
	PlayWindow(MediaPlayer *player, QWidget *parent = 0, const char * name = 0);
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
	void notifyStopPlay();

private:
	QVector<AudioData> play_list;

	/*
	 * current_track is the track played by mplayer.
	 * CURRENT_TRACK_NONE means no track has to be played.
	 */
	unsigned int current_track;
	static const unsigned CURRENT_TRACK_NONE = UINT_MAX;

	// TODO: rimuovere questi metodi qt3!
	void setPaletteForegroundColor(const QColor &c);
	void setPaletteBackgroundColor(const QColor &c);
};


/**
 * \class MediaPlayWindow
 *
 * This class inherits from PlayWindow class to implements the playing of files.
 */
class MediaPlayWindow : public PlayWindow
{
Q_OBJECT
public:
	MediaPlayWindow(MediaPlayer *player, QWidget *parent = 0, const char * name = 0);

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
	TitleLabel *time_pos_label, *meta_title_label, *meta_artist_label, *meta_album_label;

	/// Timer to refresh data from MediaPlayer
	QTimer *data_refresh_timer;
};


/**
 * \class RadioPlayWindow
 *
 * This class inherits from PlayWindow class to implements the playing of radio.
 */
class RadioPlayWindow : public PlayWindow
{
Q_OBJECT
public:
	RadioPlayWindow(MediaPlayer *player, QWidget *parent = 0, const char * name = 0);

	// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

protected:
	void startPlayer(unsigned int);

private slots:
	void handleButtons(int);

private:
	/// Play buttons
	ButtonsBar  *play_controls;
	TitleLabel *meta_title_label;
};

#endif
