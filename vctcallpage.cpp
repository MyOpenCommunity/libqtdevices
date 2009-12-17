#include "vctcallpage.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName
#include "hardware_functions.h" // setVctContrast
#include "icondispatcher.h"
#include "entryphone_device.h"
#include "xml_functions.h"
#include "devices_cache.h"

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>

#define BOTTOM_SPACING 15

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

CameraMove::CameraMove(EntryphoneDevice *dev)
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



CallControl::CallControl(EntryphoneDevice *d)
{
	dev = d;

	call_icon = bt_global::skin->getImage("call");
	call_accept = new EnablingButton;
	call_accept->setImage(getBostikName(call_icon, "off"));
	call_accept->setDisabledPixmap(getBostikName(call_icon, "dis"));
	// TODO: connect to accept call

	BannTuning *volume = new BannTuning("", bt_global::skin->getImage("volume"));
	// TODO: connect to volume settings

	mute_icon = bt_global::skin->getImage("mute");
	mute_button = getButton(getBostikName(mute_icon, "off"));
	mute_button->disable();
	// TODO: connect to mute settings

	stairlight = getButton(bt_global::skin->getImage("stairlight"));
	// TODO: connect
	unlock_door = getButton(bt_global::skin->getImage("unlock_door"));
	// TODO: connect

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));
	// TODO: connect

	QHBoxLayout *bottom = new QHBoxLayout(this);
	bottom->setContentsMargins(0, 0, 30, 0);
	bottom->setSpacing(BOTTOM_SPACING);
	bottom->addWidget(call_accept);
	bottom->addWidget(volume);
	bottom->addWidget(mute_button);
	bottom->addWidget(stairlight);
	bottom->addWidget(unlock_door);
	bottom->addWidget(cycle);
}





VCTCallPage::VCTCallPage(const QDomNode &config_node)
{
	// we have a random configuration node and we must get our internal unit address
	// which on the very top of conf file.
	QDomNode n = config_node;
	while (n.nodeName() != "configuratore")
		n = n.parentNode();
	QString where = getElement(n, "setup/scs/coordinate_scs/my_piaddress").text();
	//transform address into internal address
	where.prepend("1");
	// we must have only one entryphone device since we need to remember some state
	dev = bt_global::add_device_to_cache(new EntryphoneDevice(where));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));

	SkinContext ctx(666);

	// sidebar
	QVBoxLayout *sidebar = new QVBoxLayout;
	sidebar->setContentsMargins(0, 0, 0, 15);
	sidebar->setSpacing(0);

	brightness = new BannTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	connect(brightness, SIGNAL(valueChanged(int)), SLOT(setBrightness(int)));
	sidebar->addWidget(brightness);

	contrast = new BannTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	connect(contrast, SIGNAL(valueChanged(int)), SLOT(setContrast(int)));
	sidebar->addWidget(contrast);

	color = new BannTuning(tr("Color"), bt_global::skin->getImage("color"));
	connect(color, SIGNAL(valueChanged(int)), SLOT(setColor(int)));
	sidebar->addWidget(color);

	camera = new CameraMove(dev);
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

	QHBoxLayout *bottom = buildBottomLayout();

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addLayout(hbox, 1);
	layout->addLayout(bottom, 0);
}

void VCTCallPage::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::INCOMING_CALL:
			showPage();
			break;
		case EntryphoneDevice::END_OF_CALL:
			emit Closed();
			break;
		}
		++it;
	}
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

QHBoxLayout *VCTCallPage::buildBottomLayout()
{
	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SLOT(closeCall()));
	call_control = new CallControl(dev);
	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 0, 0);
	bottom->setSpacing(BOTTOM_SPACING);
	bottom->addWidget(back);
	bottom->addWidget(call_control);
	return bottom;
}

void VCTCallPage::closeCall()
{
	dev->endCall();
	emit Closed();
}

void VCTCallPage::setContrast(int value)
{
	// TODO: original code set this value into a global struct which at some point is written to
	// /dev/nvram...what should we do?
	static const QString contrast_command("1");
	setVctVideoValue(contrast_command, QString::number(value));
}

void VCTCallPage::setColor(int value)
{
	static const QString color_command("2");
	setVctVideoValue(color_command, QString::number(value));
}

void VCTCallPage::setBrightness(int value)
{
	static const QString brightness_command("3");
	setVctVideoValue(brightness_command, QString::number(value));
}

void VCTCallPage::addExternalPlace(const QString &where)
{
	places << where;
}
