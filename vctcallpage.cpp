#include "vctcallpage.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>

BannTuning::BannTuning(const QString &banner_text, const QString &icon_name, QWidget *parent) :
	Bann2CentralButtons(parent)
{
	// levels go from 0 to 8 inclusive
	current_level = 4;
	center_icon = icon_name;

	initBanner(getBostikName(center_icon, QString("sxl") + QString::number(current_level)),
		getBostikName(center_icon, QString("dxl") + QString::number(current_level)),
		banner_text);
	connect(center_left, SIGNAL(clicked()), SLOT(decreaseLevel()));
	connect(center_right, SIGNAL(clicked()), SLOT(increaseLevel()));
}

void BannTuning::decreaseLevel()
{
	if (current_level > 0)
	{
		--current_level;
		changeIcons();
		emit valueChanged(current_level);
	}
}

void BannTuning::increaseLevel()
{
	if (current_level < 8)
	{
		++current_level;
		changeIcons();
		emit valueChanged(current_level);
	}
}

void BannTuning::changeIcons()
{
	center_left->setImage(getBostikName(center_icon, QString("sxl") + QString::number(current_level)));
	center_right->setImage(getBostikName(center_icon, QString("dxl") + QString::number(current_level)));
}


CameraMove::CameraMove(device *dev)
{
	up = new BtButton;
	up->setImage(bt_global::skin->getImage("arrow_up"));
	down = new BtButton;
	down->setImage(bt_global::skin->getImage("arrow_down"));
	right = new BtButton;
	right->setImage(bt_global::skin->getImage("arrow_right"));
	left = new BtButton;
	left->setImage(bt_global::skin->getImage("arrow_left"));
	fullscreen = new BtButton;
	fullscreen->setImage(bt_global::skin->getImage("fullscreen"));

	QHBoxLayout *center = new QHBoxLayout;
	center->setContentsMargins(0, 0, 0, 0);
	center->setSpacing(0);
	center->addWidget(left);
	center->addWidget(fullscreen);
	center->addWidget(right);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(5);
	l->addWidget(up);
	l->addLayout(center);
	l->addWidget(down);
}



VCTCallPage::VCTCallPage()
{
	SkinContext ctx(666);

	// sidebar
	QVBoxLayout *sidebar = new QVBoxLayout;

	brightness = new BannTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	sidebar->addWidget(brightness);
	contrast = new BannTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	sidebar->addWidget(contrast);
	color = new BannTuning(tr("Color"), bt_global::skin->getImage("color"));
	sidebar->addWidget(color);

	setup_vct = new BtButton;
	setup_vct_icon = bt_global::skin->getImage("setup_vct");
	setup_vct->setImage(getBostikName(setup_vct_icon, "on"));
	connect(setup_vct, SIGNAL(clicked()), SLOT(toggleCameraSettings()));
	sidebar->addWidget(setup_vct);

	QHBoxLayout *hbox = new QHBoxLayout;

	// widget where video will be displayed
	QWidget *video = new QWidget;
	video->setFixedSize(352, 240);
	video->setStyleSheet("background-color: black");

	hbox->addWidget(video);
	hbox->addLayout(sidebar);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SIGNAL(Closed()));

	call_accept = new BtButton;
	call_icon = bt_global::skin->getImage("call");
	call_accept->setImage(getBostikName(call_icon, "off"));

	BannTuning *volume = new BannTuning("", bt_global::skin->getImage("volume"));

	mute_button = new BtButton;
	mute_icon = bt_global::skin->getImage("mute");
	mute_button->setImage(getBostikName(mute_icon, "offdis"));

	stairlight = new BtButton;
	stairlight_icon = bt_global::skin->getImage("stairlight");
	stairlight->setImage(stairlight_icon);

	unlock_door = new BtButton;
	unlock_icon = bt_global::skin->getImage("unlock_door");
	unlock_door->setImage(unlock_icon);

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));

	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 30, 0);
	bottom->setSpacing(15);
	bottom->addWidget(back);
	bottom->addWidget(call_accept);
	bottom->addWidget(volume);
	bottom->addWidget(mute_button);
	bottom->addWidget(stairlight);
	bottom->addWidget(unlock_door);
	bottom->addWidget(cycle);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addLayout(hbox);
	layout->addLayout(bottom);
}

void VCTCallPage::toggleCameraSettings()
{
	qDebug() << "toggleCameraSettings called";
}

void VCTCallPage::addExternalPlace(const QString &where)
{
	places << where;
}
