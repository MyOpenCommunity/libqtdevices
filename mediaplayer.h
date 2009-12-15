#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMap>
#include <QObject>

#include <stdio.h>

class QRect;


class MediaPlayer : public QObject
{
Q_OBJECT
public:
	MediaPlayer(QObject *parent = 0);
	~MediaPlayer();

	/// Starts MPlayer playing a single track
	bool play(QString track, bool write_output = true);

	/// Starts MPlayer playing a video
	bool playVideo(QString track, QRect geometry, bool write_output = true);

	/// Pause playing of current song
	void pause();

	/// Resume playing of current song
	void resume();

	/// Stop and quit mplayer
	void quit();

	/// Wheather the player is on pause
	bool isPaused() { return paused; }

	/// Retrun true if music is playing
	bool isInstanceRunning();

	/// Read ASYNCHRONOUSLY the output from the PIPE and parse it to get INFO
	QMap<QString, QString> getPlayingInfo();

	/// Need to be public because called by signal handler
	void sigChildReceived(int dead_pid, int status);

	/// moves the playback backward/forward by the given offset
	void seek(int seconds);

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

	bool runMPlayer(const char *mplayer_args[], bool write_output);

	bool _isPlaying;
	bool paused;

signals:
	/// mplayer child process quit gracefully and done it's work.
	void mplayerDone();

	/// mplayer child process was killed by a signal
	void mplayerKilled();

	/// mplayer exited with error or aborted
	void mplayerAborted();
};

#endif

