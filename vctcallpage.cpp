#include "vctcallpage.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName
#include "hardware_functions.h" // setVctContrast
#include "displaycontrol.h" //forceOperativeMode
#include "icondispatcher.h"
#include "entryphone_device.h"
#include "xml_functions.h"
#include "bann2_buttons.h"
#include "items.h"

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QGridLayout>


#define BOTTOM_SPACING 15

const QString video_grabber_path = "/home/bticino/bin/rsize";
const QString video_grabber_normal = "0";
const QString video_grabber_fullscreen = "1";


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

	QGridLayout *main_layout = new QGridLayout(this);
	main_layout->setContentsMargins(5, 5, 5, 5);
	main_layout->setSpacing(5);
	main_layout->addWidget(up, 0, 1);
	main_layout->addWidget(left, 1, 0);
	main_layout->addWidget(fullscreen, 1, 1);
	main_layout->addWidget(right, 1, 2);
	main_layout->addWidget(down, 2, 1);
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


CameraImageControl::CameraImageControl(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	brightness = new ItemTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	connect(brightness, SIGNAL(valueChanged(int)), SLOT(setBrightness(int)));
	l->addWidget(brightness, 1, Qt::AlignHCenter);

	contrast = new ItemTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	connect(contrast, SIGNAL(valueChanged(int)), SLOT(setContrast(int)));
	l->addWidget(contrast, 1, Qt::AlignHCenter);

	color = new ItemTuning(tr("Color"), bt_global::skin->getImage("color"));
	connect(color, SIGNAL(valueChanged(int)), SLOT(setColor(int)));
	l->addWidget(color, 1, Qt::AlignHCenter);
}

void CameraImageControl::setContrast(int value)
{
	// TODO: original code set this value into a global struct which at some point is written to
	// /dev/nvram...what should we do?
	static const QString contrast_command("1");
	setVctVideoValue(contrast_command, QString::number(value));
}

void CameraImageControl::setColor(int value)
{
	static const QString color_command("2");
	setVctVideoValue(color_command, QString::number(value));
}

void CameraImageControl::setBrightness(int value)
{
	static const QString brightness_command("3");
	setVctVideoValue(brightness_command, QString::number(value));
}


CallControl::CallControl(EntryphoneDevice *d)
{
	dev = d;

	connected = false;
	call_icon = bt_global::skin->getImage("call");
	call_accept = new BtButton;
	call_accept->setOnOff();
	call_accept->setImage(getBostikName(call_icon, "off"));
	call_accept->setPressedImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));

	// TODO: verificare in che situazione posso avere lo stato disabilitato per il
	// pulsante per accettare le videochiamate!
//	call_accept->setDisabledPixmap(getBostikName(call_icon, "dis"));

	ItemTuning *volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	// TODO: connect to volume settings

	mute_icon = bt_global::skin->getImage("mute");
	mute_button = getButton(getBostikName(mute_icon, "off"));
	mute_button->disable();
	// TODO: connect to mute settings

	stairlight = getButton(bt_global::skin->getImage("stairlight"));
	connect(stairlight, SIGNAL(pressed()), dev, SLOT(stairLightActivate()));
	connect(stairlight, SIGNAL(released()), dev, SLOT(stairLightRelease()));

	unlock_door = getButton(bt_global::skin->getImage("unlock_door"));
	connect(unlock_door, SIGNAL(pressed()), dev, SLOT(openLock()));
	connect(unlock_door, SIGNAL(released()), dev, SLOT(releaseLock()));

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));
	connect(cycle, SIGNAL(clicked()), dev, SLOT(cycleExternalUnits()));

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

void CallControl::showEvent(QShowEvent *)
{
	// Every time that the control is shown, the status of the connection must be
	// reset in order to be sync with the effective call status.
	connected = false;
}

void CallControl::toggleCall()
{
	connected = !connected;

	call_accept->setStatus(connected);
	if (connected)
		dev->answerCall();
	else
		emit endCall();
}


VCTCall::VCTCall(EntryphoneDevice *d, FormatVideo f)
{
	format = f;
	dev = d;
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));

	SkinContext ctx(666);

	image_control = new CameraImageControl;

	camera = new CameraMove(dev);
	camera->setMoveEnabled(false);

	setup_vct = new BtButton;
	setup_vct_icon = bt_global::skin->getImage("setup_vct");
	camera_settings_shown = false;
	toggleCameraSettings();
	connect(setup_vct, SIGNAL(clicked()), SLOT(toggleCameraSettings()));

	video_box = new QLabel;
	video_box->setStyleSheet("background-color: black");

	call_control = new CallControl(dev);
	connect(call_control, SIGNAL(endCall()), SLOT(handleClose()));
}


void VCTCall::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::INCOMING_CALL:
		{
			emit incomingCall();
			QStringList args;
			QPoint top_left = video_box->mapToGlobal(QPoint(0, 0));
			args << QString::number(top_left.x()) << QString::number(top_left.y()) << QString::number(format);
			video_grabber.start(video_grabber_path, args);
		}
			break;
		case EntryphoneDevice::END_OF_CALL:
			video_grabber.terminate();
			emit callClosed();
			break;
		}
		++it;
	}
}

void VCTCall::toggleCameraSettings()
{
	camera_settings_shown = !camera_settings_shown;
	if (camera_settings_shown)
	{
		image_control->hide();
		camera->show();
		setup_vct->setImage(getBostikName(setup_vct_icon, "off"));
	}
	else
	{
		image_control->show();
		camera->hide();
		setup_vct->setImage(getBostikName(setup_vct_icon, "on"));
	}
}

void VCTCall::endCall()
{
	dev->endCall();
	video_grabber.terminate();
}

void VCTCall::handleClose()
{
	endCall();
	emit callClosed();
}


VCTCallPage::VCTCallPage(EntryphoneDevice *d)
{
	VCTCall *vct_call = new VCTCall(d, VCTCall::NORMAL_VIDEO);
	connect(vct_call, SIGNAL(callClosed()), this, SIGNAL(Closed()));
	connect(vct_call, SIGNAL(incomingCall()), SLOT(showPage()));

	// sidebar
	QVBoxLayout *sidebar = new QVBoxLayout;
	sidebar->setContentsMargins(0, 0, 0, 5);
	sidebar->setSpacing(5);
	sidebar->addStretch(1);
	sidebar->addWidget(vct_call->camera, 0, Qt::AlignCenter);
	sidebar->addWidget(vct_call->image_control, 0, Qt::AlignCenter);
	sidebar->addWidget(vct_call->setup_vct);

	vct_call->video_box->setFixedSize(352, 240);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), vct_call, SLOT(endCall()));
	connect(back, SIGNAL(clicked()), this, SIGNAL(Closed()));

	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 0, 0);
	bottom->setSpacing(BOTTOM_SPACING);
	bottom->addWidget(back);
	bottom->addWidget(vct_call->call_control);

	QGridLayout *layout = new QGridLayout(this);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	layout->addLayout(sidebar, 0, 1, 2, 1);
	layout->addWidget(vct_call->video_box, 1, 0);
	layout->addLayout(bottom, 2, 0, 1, 2, Qt::AlignLeft);
	layout->setContentsMargins(10, 0, 0, 10);
	layout->setSpacing(10);

	prev_page = 0;
}

void VCTCallPage::showPage()
{
	prev_page = currentPage();
	Page::showPage();
}

void VCTCallPage::showPreviousPage()
{
	// TODO: la previous page non tiene conto della gestione dello screensaver.
	// Gestire in modo migliore con un oggetto globale!
	Q_ASSERT_X(prev_page, "VCTCallPage::showPreviousPage", "Previous page not set!");
	prev_page->showPage();
}

void VCTCallPage::showEvent(QShowEvent *)
{
	bt_global::display.forceOperativeMode(true);
}

void VCTCallPage::hideEvent(QHideEvent *)
{
	bt_global::display.forceOperativeMode(false);
}

