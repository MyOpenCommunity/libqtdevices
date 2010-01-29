#ifndef MULTIMEDIA_BUTTONS_H
#define MULTIMEDIA_BUTTONS_H

#include <QWidget>

class BtButton;

// contains previous/next, play/pause/stop and fullscreen buttons
class MultimediaPlayerButtons : public QWidget
{
Q_OBJECT
public:
	enum Type
	{
		IMAGE_PAGE = 1,
		IMAGE_WINDOW = 2,
		VIDEO_PAGE = 3,
		VIDEO_WINDOW = 4
	};

	MultimediaPlayerButtons(Type type);

public slots:
	// update the icon of the play button
	void started();
	void stopped();

signals:
	void play();
	void pause();
	void stop();
	void previous();
	void next();
	void skipForward();
	void skipBack();
	void fullScreen();
	void noFullScreen();

private slots:
	void playToggled(bool playing);

private:
	BtButton *getButton(const QString &icon, const char *destination);

private:
	BtButton *play_button;
	QString play_icon, pause_icon;
};

#endif // MULTIMEDIA_BUTTONS_H
