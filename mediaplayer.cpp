//
// C++ Implementation: mediaplayer
//
// Description:
//
//
// Author: Kemosh,,, <kemosh@Kemosh-VirtualU>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mediaplayer.h"

#include <qregexp.h>

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";

// Dirty kludge to allow mplayerExited access to class instance.
static MediaPlayer *_globalMediaPlayer;

static void mplayerExited(int signo, siginfo_t *info, void *)
{
	int status;
	wait(&status);

	if (_globalMediaPlayer)
		_globalMediaPlayer->sigChildReceived(info->si_pid, status);
}

MediaPlayer::MediaPlayer( QObject *parent, const char *name ) : QObject(parent, name)
{
	struct sigaction sa;

	memset( &sa, 0, sizeof ( sa ) );
	sigemptyset( &sa.sa_mask );
	sigaddset( &sa.sa_mask, SIGCHLD );
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = ::mplayerExited;
	sigaction( SIGCHLD, &sa, NULL );

	mplayer_pid = 0;
	paused = false;
	_isPlaying = false;
	_globalMediaPlayer = this;
}

MediaPlayer::~MediaPlayer()
{
	_globalMediaPlayer = NULL;
}

bool MediaPlayer::play(QString track)
{
	int control_pipe[2];
	int output_pipe[2];

	if (pipe(control_pipe) < 0)
		return false;
	if (pipe(output_pipe) < 0)
		return false;

	mplayer_pid = fork();
	if (mplayer_pid == -1)
		return false;

	// CHILD
	if (mplayer_pid == 0)
	{
		// Close unused write end
		close(control_pipe[1]);
		close(output_pipe[0]);

		dup2(control_pipe[0], STDIN_FILENO);
		dup2(output_pipe[1], STDOUT_FILENO);

		//char * const mplayer_args[] = { "mplayer", "-slave", "-idle", NULL };
		const char *mplayer_args[] = { "mplayer", NULL, NULL };

		mplayer_args[1] = track.latin1();

		execve(MPLAYER_FILENAME, const_cast<char * const *>(mplayer_args), environ);
	}
	// PARENT
	else
	{
		_isPlaying = true;
		paused = false;

		close(control_pipe[0]);
		close(output_pipe[1]);

		control_fd = control_pipe[1];
		output_fd  = output_pipe[0];

		// Make file descriptors NON BLOCKING
		fcntl(control_fd, F_SETFL, O_NONBLOCK);
		fcntl(output_fd, F_SETFL, O_NONBLOCK);

		ctrlf = fdopen(control_fd, "w");
		outf  = fdopen(output_fd, "r");

		qDebug("[AUDIO] playing track '%s'", track.ascii());
	}
}

void MediaPlayer::pause()
{
	execCmd(" ");
	paused = true;
}

void MediaPlayer::resume()
{
	execCmd(" ");
	paused = false;
}

void MediaPlayer::quit()
{
	if (mplayer_pid)
	{
		mplayer_pid = 0;
		execCmd("q");
	}
}

void MediaPlayer::execCmd(QString command)
{
	fprintf(ctrlf, command.latin1());
	fflush(ctrlf);
}


QString MediaPlayer::readOutput()
{
	char line[100];
	QString result;

	while (fgets(line, 100, outf) != NULL)
		result += QString(line);

	return result;
}


bool MediaPlayer::isPlaying()
{
	return _isPlaying;
}


QMap<QString, QString> MediaPlayer::getPlayingInfo()
{
	/// Define Search Data Map
	QMap<QString, QString> data_search;
	data_search["file_name"]    = "Playing [^\\n]*([^/\\n]+)\\.\\n";
	data_search["meta_title"]   = "Title: ([^\\n]*)\\n";
	data_search["meta_artist"]  = "Artist: ([^\\n]*)\\n";
	data_search["meta_album"]   = "Album: ([^\\n]*)\\n";
	data_search["meta_year"]    = "Year: ([^\\n]*)\\n";
	data_search["meta_comment"] = "Comment: ([^\\n]*)\\n";
	data_search["meta_genre"]   = "Genre: ([^\\n]*)\\n";
	data_search["total_time"]   = "[(](\\d+:\\d+\\.\\d+)[)] \\d+\\.\\d+";
	data_search["current_time"] = "A:\\s+\\d+\\.\\d+\\s+[(](\\d*:*\\d+\\.\\d+)[)]";

	/// READ ROW output from MPlayer
	QString row_data = readOutput();

	/// Create output Map
	QMap<QString, QString> info_data;

	// row data visualization
	// qDebug("\n____________________________________________________________________________");
	// qDebug(row_data);
	// qDebug("____________________________________________________________________________\n");

	/// Parse ROW data to get info
	QMap<QString, QString>::Iterator it;
	for ( it = data_search.begin(); it != data_search.end(); ++it )
	{
		QRegExp rx( it.data() );

		if ( rx.search( row_data ) > -1 )
			info_data[it.key()] = rx.cap(1); //matches[matches.count()-1];
	}

	return info_data;
}


void MediaPlayer::quitMPlayer()
{
	/// kill the MPlayer process
	kill(mplayer_pid, SIGINT);
}

void MediaPlayer::sigChildReceived(int dead_pid, int status)
{
	qDebug("[AUDIO] Signal SIGCHLD received, pid %d", dead_pid);

	if (WIFEXITED(status))
		qDebug("[AUDIO] child exited, with code %d", WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		qDebug("[AUDIO] child terminated by signal %d", WTERMSIG(status));
	else
		qDebug("[AUDIO] child quit for unknown reason");

	/// Check if the dead child is mplayer or not
	if (dead_pid == mplayer_pid)
	{
		qDebug("[AUDIO] Child was my mplayer");

		mplayer_pid = 0;

		// Notify to audio playing window that mplayer is terminated
		emit mplayerTerminated();
	}
	else
		qDebug("[AUDIO] Child was *not* my mplayer, probably start/stop script");
}

