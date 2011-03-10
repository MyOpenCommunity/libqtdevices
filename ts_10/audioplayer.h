/*
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "mediaplayerpage.h"
#include "xmldevice.h"
#include "generic_functions.h" // EntryInfo

#include <QTime>

class QLabel;
class BtButton;
class AudioPlayerPage;
class VirtualSourceDevice;


// Implements the ListManager interface for files retrieved from upnp.
// The items in a directory/album are loaded in using the XmlDevice, and
// only the items to display are requested to the upnp server, in order to
// improve the performance.
class UPnpListManager : public ListManager
{
Q_OBJECT
public:
	UPnpListManager(XmlDevice *dev);
	virtual QString currentFilePath();

	virtual void nextFile();
	virtual void previousFile();

	virtual int currentIndex();
	virtual int totalFiles();

	virtual EntryInfo::Metadata currentMeta();

	// UPnpListManager specific methods
	void setStartingFile(EntryInfo starting_file);
	void setCurrentIndex(int i);
	void setTotalFiles(int n);

signals:
	void serverDown();

private slots:
	void handleResponse(const XmlResponse &response);
	void handleError(int response, int code);

private:
	int index, total_files;
	EntryInfo current_file;
	XmlDevice *dev;
};


/*!
	\ingroup Multimedia
	\brief Page for local audio playback with mplayer.

	Can be used to play both audio files (es. MP3) and ShoutCast
	streams (for IP radio).  it can play through sound diffusion or
	using the local amplifier.
 */
class AudioPlayerPage : public MediaPlayerPage
{
Q_OBJECT
public:
	enum MediaType
	{
		LOCAL_FILE,
		IP_RADIO,
		UPNP_FILE,
		MAX_MEDIA_TYPE
	};

	static AudioPlayerPage *getAudioPlayerPage(MediaType type);
	static QVector<AudioPlayerPage *> audioPlayerPages();

	virtual int sectionId() const;
	void showNextButton(bool show);
	void showPrevButton(bool show);

signals:
	void loopDetected();
	void playerExited();

protected:
	virtual void startPlayback();

public slots:
	void playAudioFile(EntryInfo starting_file, int file_index, int num_files);

	void playAudioFiles(QList<QString> files, unsigned element);
	void playAudioFiles(EntryInfoList entries, unsigned element);
	void playAudioFilesBackground(QList<QString> files, unsigned element);

private:
	void startMPlayer(QString filename, int time);
	void clearLabels();

private slots:
	void refreshPlayInfo(const QMap<QString, QString> &attrs);
	void refreshPlayInfo();
	void changeVolume(int volume);
	void gotoSoundDiffusion();
	void playerStarted();
	void playerStopped();
	void quit();
	void resetLoopCheck();
	void mplayerDone();
	void currentFileChanged();
	void handleServerDown();

private:
	AudioPlayerPage(MediaType type);
	MultimediaPlayerButtons *player_buttons;

	MediaType type;
	bool stream_url_exist;
	bool stream_title_exist;
	bool filename_exist;

	QLabel *description_top, *description_bottom, *track, *elapsed;
	// icon in tray bar
	static BtButton *tray_icon;
	static QVector<AudioPlayerPage *> audioplayer_pages;

	int loop_starting_file; // the index of the song used to detect loop
	int loop_total_time; // the total time used to detect a loop
	QTime loop_time_counter; // used to count the time elapsed
};

#endif
