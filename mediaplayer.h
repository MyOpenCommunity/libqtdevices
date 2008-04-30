//
// C++ Interface: mediaplayer
//
// Description: 
//
//
// Author: Kemosh,,, <kemosh@Kemosh-VirtualU>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <qmap.h>
#include <qobject.h>

class MediaPlayer : public QObject
{
Q_OBJECT
public:
	MediaPlayer( QObject *parent = 0, const char *name = 0 );
	~MediaPlayer();

	/// Starts MPlayer playing a single track
	bool play(QString track);

	/// Pause playing of current song
	void pause();

	/// Resume playing of current song
	void resume();

	/// Stop and quit mplayer
	void quit();

	/// Wheather the player is on pause
	bool isPaused()  { return paused; }

	/// Checks if MPlayer is playing checking mplayer_pid
	bool isPlaying();
	
	/// Read ASYNCHRONOUSLY the output from the PIPE and parse it to get INFO
	QMap<QString, QString> getPlayingInfo();

	/// Kill mplayer process avoiding to emit the mplayerTerminated() signal
	void killMPlayer();

	/// Need to be public because called by signal handler
	void sigChildReceived(int dead_pid);

private:
	/// mplayer PID
	int mplayer_pid;

	/// File Descriptors
	int control_fd;
	int output_fd;
	
	/// File Handlers
	FILE *ctrlf;
	FILE *outf;

	/// Send a string to the mplayer process to execute.
	void execCmd(QString command);
	/// Read output from mplayer process.
	QString readOutput();

	bool _isPlaying;
	bool paused;

signals:
	void mplayerTerminated();
};

#endif // MEDIAPLAYER_H

