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
		IN_PAGE = 1,
		IN_WINDOW = 2
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
	void fullScreen();
	void noFullScreen();

private slots:
	void playToggled(bool playing);

private:
	BtButton *play_button;
	QString play_icon, pause_icon;
};

#endif // MULTIMEDIA_BUTTONS_H
