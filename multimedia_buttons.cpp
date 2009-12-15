#include "multimedia_buttons.h"
#include "btbutton.h"
#include "skinmanager.h"

#include <QHBoxLayout>


static inline BtButton *getButton(const QString &icon)
{
	BtButton *button = new BtButton;
	button->setImage(icon);

	return button;
}

MultimediaPlayerButtons::MultimediaPlayerButtons(Type type)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(type == IN_WINDOW ? 0 : 5);

	play_icon = bt_global::skin->getImage("start");
	pause_icon = bt_global::skin->getImage("pause");

	BtButton *prev = getButton(bt_global::skin->getImage("previous"));
	BtButton *next = getButton(bt_global::skin->getImage("next"));
	BtButton *stop = getButton(bt_global::skin->getImage("stop"));
	BtButton *screen = getButton(bt_global::skin->getImage(type == IN_WINDOW ? "nofullscreen" : "fullscreen"));

	play_button = getButton(play_icon);
	play_button->setPressedImage(pause_icon);
	play_button->setCheckable(true);
	play_button->setOnOff();

	l->addWidget(prev);
	l->addWidget(play_button);
	l->addWidget(stop);
	l->addWidget(next);
	l->addWidget(screen);

	connect(play_button, SIGNAL(toggled(bool)), SLOT(playToggled(bool)));
	connect(prev, SIGNAL(clicked()), SIGNAL(previous()));
	connect(next, SIGNAL(clicked()), SIGNAL(next()));
	connect(stop, SIGNAL(clicked()), SIGNAL(stop()));
	connect(screen, SIGNAL(clicked()), type == IN_PAGE ? SIGNAL(fullScreen()) : SIGNAL(noFullScreen()));
}

void MultimediaPlayerButtons::playToggled(bool playing)
{
	if (playing)
		emit play();
	else
		emit pause();
}

void MultimediaPlayerButtons::started()
{
	play_button->setStatus(true);
	play_button->setChecked(true);
}

void MultimediaPlayerButtons::stopped()
{
	play_button->setStatus(false);
	play_button->setChecked(false);
}

