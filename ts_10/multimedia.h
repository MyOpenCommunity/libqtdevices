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


#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include "iconpage.h"
#include "mount_watcher.h"

#include <QFutureWatcher>

class QDomNode;
class StateButton;
class MultimediaFileListPage;
class FileSelector;
class SongSearch;
class IPRadioPage;


/*!
	\defgroup Multimedia Multimedia

	Allows access to multimedia functionality of the touchscreen.  Users can
	read RSS feeds (FeedManager), display images from webcams (WebcamPage),
	listen to web radios (IPRadioPage),
	browse files contained in an SD card or USB key (MultimediaFileListPage),
	display images (SlideshowPage), videos (VideoPlayerPage) and play
	MP3 files (AudioPlayerPage).

	Some multimedia classes are also used by the sound diffusion section
	(SongSearch, MediaPlayer, AudioPlayerPage, MultimediaSectionPage,
	MultimediaFileListPage, IPRadioPage) to implement local source playback.

	Note that audio playback can be either local or through the sound diffusion
	depending on touchscreen model and configuration.
*/


/*!
	\ingroup Multimedia
	\brief Button that monitors the state of a mount point

	When the device is mounted, clcking the button browses the corresponding mount point
	using the specified file selector.
*/
class FileSystemBrowseButton : public IconPageButton
{
Q_OBJECT
public:
	FileSystemBrowseButton(MountWatcher &watch, FileSelector *browser,
			       MountType type, const QString &label,
			       const QString &icon_mounted, const QString &icon_unmounted);

	QString currentPath() const { return directory; }

private slots:
	void mounted(const QString &path, MountType type);
	void unmounted(const QString &path, MountType type);
	void browse();

private:
	FileSelector *browser;
	QString directory;
	MountType type;
};


/*!
	\ingroup Multimedia
	\brief Main page of the multimedia section

	It's also used by \ref SoundDiffusion (for the local source) and by \ref Settings
	(for screensaver with slide-show).
*/
class MultimediaSectionPage : public IconPage
{
Q_OBJECT
public:
	enum Item
	{
		ITEMS_FILESYSTEM = 0x0001,
		ITEMS_WEBCAM = 0x0010,
		ITEMS_WEBRADIO = 0x0100,
		ITEMS_RSS = 0x1000,
		ITEMS_UPNP = 0x10000,
		ITEMS_AUDIO = ITEMS_FILESYSTEM | ITEMS_WEBRADIO | ITEMS_UPNP,
		ITEMS_ALL = ITEMS_FILESYSTEM | ITEMS_AUDIO | ITEMS_WEBCAM | ITEMS_RSS | ITEMS_UPNP
	};
	Q_DECLARE_FLAGS(Items, Item);

	// The FileSelector passed here, should be deleted by this class
	// if the configuration doesn't support filesystem (usb and sd).
	MultimediaSectionPage(const QDomNode &config_node,
						  MultimediaSectionPage::Items items = MultimediaSectionPage::ITEMS_ALL,
						  FileSelector *browser = 0,
						  const QString &title = QString());

	virtual int sectionId() const;

	static void playSomethingRandomly();
	static Page *current_player;

protected:
	virtual void showEvent(QShowEvent *);

private slots:
	void gotoPlayerPage();

private:
	void loadItems(const QDomNode &config_node);

	MultimediaSectionPage::Items showed_items;
	FileSelector *browser;
	bool delete_browser;
	BtButton *play_button;


	// used by playSomethingRandomly()
	static SongSearch *song_search;
	static bool usb_initialized;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MultimediaSectionPage::Items);


/*!
	\ingroup Multimedia
	\ingroup SoundDiffusion
	\brief Start playing one of the available multimedia audio sources.

	Search asynchronously for a web radio or mp3 to play; used when the touch is
	turned on as a source and nothing is playing.
*/
class SongSearch : public QObject
{
Q_OBJECT
public:
	SongSearch(QList<int> sources, FileSystemBrowseButton *usb, FileSystemBrowseButton *sd, IPRadioPage *radio);

	void startSearch();

private:
	typedef QPair<QStringList, bool * volatile> AsyncRes;

	void terminateSearch();
	void nextSource();
	void playAudioFiles(QStringList things, int type);

	// called in a separate thread
	static AsyncRes scanPath(const QString &path, bool *terminate);

private slots:
	void pathScanComplete();

private:
	// the page IDs of the available sources
	QList<int> sources;
	// -1 when the search is not active; index in the sources array during search
	int current_source;

	// available sources
	FileSystemBrowseButton *usb_button, *sd_button;
	IPRadioPage *ip_radio;

	// terminate the current search; only set once to true in the main thread
	// and only read in the slave thread
	bool * volatile terminate;
};

#endif // MULTIMEDIA_H

