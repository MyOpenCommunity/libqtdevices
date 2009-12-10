#include "vctcallpage.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName
#include "icondispatcher.h"

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>

EnablingButton::EnablingButton(QWidget *parent) :
	BtButton(parent)
{
}

void EnablingButton::setDisabledPixmap(const QString &path)
{
	disabled_pixmap = *bt_global::icons_cache.getIcon(path);
}

void EnablingButton::enable()
{
	BtButton::enable();
	setIcon(pixmap);
}

void EnablingButton::disable()
{
	BtButton::disable();
	setIcon(disabled_pixmap);
}



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

EnablingButton *getButton(const QString &image_path)
{
	EnablingButton *btn = new EnablingButton;
	btn->setImage(image_path);
	btn->setDisabledPixmap(getBostikName(image_path, "dis"));
	return btn;
}

CameraMove::CameraMove(device *dev)
{
	up = getButton(bt_global::skin->getImage("arrow_up"));
	down = getButton(bt_global::skin->getImage("arrow_down"));
	right = getButton(bt_global::skin->getImage("arrow_right"));
	left = getButton(bt_global::skin->getImage("arrow_left"));
	fullscreen = getButton(bt_global::skin->getImage("fullscreen"));

	QHBoxLayout *center = new QHBoxLayout;
	center->setContentsMargins(0, 0, 0, 0);
	center->setSpacing(0);
	center->addWidget(left);
	center->addWidget(fullscreen);
	center->addWidget(right);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(up);
	l->addLayout(center);
	l->addWidget(down);
}

void CameraMove::setFullscreenEnabled(bool fs)
{
	fs ? fullscreen->enable() : fullscreen->disable();
}

void CameraMove::setMoveEnabled(bool move)
{
	if (move)
	{
		up->enable();
		down->enable();
		left->enable();
		right->enable();
	}
	else
	{
		up->disable();
		down->disable();
		left->disable();
		right->disable();
	}
}



VCTCallPage::VCTCallPage()
{
	SkinContext ctx(666);

	// sidebar
	QVBoxLayout *sidebar = new QVBoxLayout;
	sidebar->setContentsMargins(0, 0, 0, 15);
	sidebar->setSpacing(0);

	brightness = new BannTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	sidebar->addWidget(brightness);
	contrast = new BannTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	sidebar->addWidget(contrast);
	color = new BannTuning(tr("Color"), bt_global::skin->getImage("color"));
	sidebar->addWidget(color);
	camera = new CameraMove(0);
	camera->setMoveEnabled(false);
	sidebar->addWidget(camera);

	setup_vct = new BtButton;
	setup_vct_icon = bt_global::skin->getImage("setup_vct");
	camera_settings_shown = false;
	toggleCameraSettings();
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

	call_icon = bt_global::skin->getImage("call");
	call_accept = new EnablingButton;
	call_accept->setImage(getBostikName(call_icon, "off"));
	call_accept->setDisabledPixmap(getBostikName(call_icon, "dis"));

	BannTuning *volume = new BannTuning("", bt_global::skin->getImage("volume"));

	mute_icon = bt_global::skin->getImage("mute");
	mute_button = getButton(getBostikName(mute_icon, "off"));
	mute_button->disable();

	stairlight = getButton(bt_global::skin->getImage("stairlight"));
	unlock_door = getButton(bt_global::skin->getImage("unlock_door"));

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));

	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 30, 5);
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
	camera_settings_shown = !camera_settings_shown;
	if (camera_settings_shown)
	{
		brightness->hide();
		color->hide();
		contrast->hide();
		camera->show();
		setup_vct->setImage(getBostikName(setup_vct_icon, "off"));
	}
	else
	{
		brightness->show();
		color->show();
		contrast->show();
		camera->hide();
		setup_vct->setImage(getBostikName(setup_vct_icon, "on"));
	}
}

void VCTCallPage::addExternalPlace(const QString &where)
{
	places << where;
}
