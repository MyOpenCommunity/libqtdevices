
#include "mediaplayer.h"

#include <QRegExp>
#include <QDebug>

#include <unistd.h>
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
	waitpid(-1, &status, WNOHANG);

	if (_globalMediaPlayer)
		_globalMediaPlayer->sigChildReceived(info->si_pid, status);
}

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
	struct sigaction sa;

	memset( &sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGCHLD);
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = ::mplayerExited;
	sigaction(SIGCHLD, &sa, NULL);

	mplayer_pid = 0;
	paused = false;
	_isPlaying = false;
	_globalMediaPlayer = this;

	ctrlf = NULL;
	outf = NULL;
}

MediaPlayer::~MediaPlayer()
{
	_globalMediaPlayer = NULL;
}

bool MediaPlayer::play(QString track, bool write_output)
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

	if (mplayer_pid == 0)
	{
		// CHILD
		// Close unused write end
		close(control_pipe[1]);
		close(output_pipe[0]);

		dup2(control_pipe[0], STDIN_FILENO);

		if (write_output)
			dup2(output_pipe[1], STDOUT_FILENO);
		else
		{
			int nullfd = open("/dev/null", O_WRONLY);
			if (nullfd != -1)
				dup2(nullfd, STDOUT_FILENO);
			else
				qDebug("[AUDIO] unable to open /dev/null");
		}

		//char * const mplayer_args[] = { "mplayer", "-slave", "-idle", NULL };
		//const char *mplayer_args[] = {MPLAYER_FILENAME, "-af", "pan=2:1:1", NULL, NULL, NULL};
		const char *mplayer_args[] = {MPLAYER_FILENAME, NULL, NULL, NULL, NULL, NULL};

		QByteArray t = track.toLatin1();
		if (track.endsWith(".m3u", Qt::CaseInsensitive))
		{
			mplayer_args[1] = "-playlist";
			mplayer_args[2] = t.constData();
		}
		else
			mplayer_args[1] = t.constData();

		execve(MPLAYER_FILENAME, const_cast<char * const *>(mplayer_args), environ);
	}
	else
	{
		// PARENT
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

		qDebug() << "[AUDIO] playing track: "<< track;
	}

	return true;
}

void MediaPlayer::pause()
{
	if (!paused)
	{
		execCmd(" ");
		paused = true;
	}
}

void MediaPlayer::resume()
{
	if (paused)
	{
		execCmd(" ");
		paused = false;
	}
}

void MediaPlayer::execCmd(QString command)
{
	if (ctrlf)
	{
		QByteArray c = command.toLatin1();
		fprintf(ctrlf, c.constData());
		fflush(ctrlf);
	}
	else
		qDebug("[AUDIO] MediaPlayer::execCmd(): mplayer not running");
}

QString MediaPlayer::readOutput()
{
	char line[1024];
	QString result;

	if (outf)
	{
		while (fgets(line, sizeof(line), outf))
			result += line;
	}
	else
		qDebug("[AUDIO] MediaPlayer::readOutput(): mplayer not running");

	return result;
}


bool MediaPlayer::isInstanceRunning()
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

	/// Parse ROW data to get info
	QMap<QString, QString>::Iterator it;
	for (it = data_search.begin(); it != data_search.end(); ++it)
	{
		QRegExp rx(it.value());

		if (rx.indexIn(row_data) > -1)
			info_data[it.key()] = rx.cap(1);
	}

	return info_data;
}


void MediaPlayer::quit()
{
	if (mplayer_pid)
		kill(mplayer_pid, SIGINT);
}

void MediaPlayer::sigChildReceived(int dead_pid, int status)
{
	/// Check if the dead child is mplayer or not
	if (dead_pid == mplayer_pid)
	{
		mplayer_pid = 0;
		_isPlaying = false;
		ctrlf = NULL;
		outf = NULL;

		if (WIFEXITED(status))
		{
			int rc = WEXITSTATUS(status);
			qDebug("[AUDIO] mplayer exited, with code %d", rc);
			if (rc == 0) //end of song
			{
				emit mplayerDone();
				return;
				
			}
			else if(rc == 1) //signal received
			{
				emit mplayerKilled();
				return;
			}
		}
		else if (WIFSIGNALED(status))
		{
			qDebug("[AUDIO] mplayer terminated by signal %d", WTERMSIG(status));
			if (WTERMSIG(status) == SIGINT)
			{
				emit mplayerKilled();
				return;
			}
		}
		else
			qDebug("[AUDIO] mplayer aborted for unknown reason");

		emit mplayerAborted();
	}
}
