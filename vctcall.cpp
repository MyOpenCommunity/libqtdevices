#include "vctcall.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName
#include "hardware_functions.h" // setVctContrast, setVolume
#include "displaycontrol.h" // bt_global::display
#include "icondispatcher.h"
#include "entryphone_device.h"
#include "xml_functions.h"
#include "bann2_buttons.h"
#include "items.h" // ItemTuning
#include "pagestack.h"

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QGridLayout>

#ifdef LAYOUT_TOUCHX

#define BOTTOM_SPACING 15

const QString video_grabber_path = "/home/bticino/bin/rsize";
const QString video_grabber_normal = "0";
const QString video_grabber_fullscreen = "1";


namespace VCTCallPrivate
{
	// The struct used to save and restore the status of a video call
	struct VCTCallStatus
	{
		bool connected;
		EnablingButton::Status mute;
		ItemTuningStatus volume_status;

		VCTCallStatus();

		// This method is used to initialize the status of the call every time
		// a new videocall is performed (so, attributes like the volume of the
		// audio are not set in the init method because they must to be
		// preserved betweeen different calls).
		void init();
	};
}

using namespace VCTCallPrivate;


EnablingButton *getButton(const QString &image_path)
{
	EnablingButton *btn = new EnablingButton;
	btn->setImage(image_path);
	btn->setDisabledImage(getBostikName(image_path, "dis"));
	return btn;
}


CameraMove::CameraMove(EntryphoneDevice *dev)
{
	up = getButton(bt_global::skin->getImage("arrow_up"));
	connect(up, SIGNAL(pressed()), dev, SLOT(moveUpPress()));
	connect(up, SIGNAL(released()), dev, SLOT(moveUpRelease()));

	down = getButton(bt_global::skin->getImage("arrow_down"));
	connect(down, SIGNAL(pressed()), dev, SLOT(moveDownPress()));
	connect(down, SIGNAL(released()), dev, SLOT(moveDownRelease()));

	left = getButton(bt_global::skin->getImage("arrow_left"));
	connect(left, SIGNAL(pressed()), dev, SLOT(moveLeftPress()));
	connect(left, SIGNAL(released()), dev, SLOT(moveLeftRelease()));

	right = getButton(bt_global::skin->getImage("arrow_right"));
	connect(right, SIGNAL(pressed()), dev, SLOT(moveRightPress()));
	connect(right, SIGNAL(released()), dev, SLOT(moveRightRelease()));

	fullscreen = getButton(bt_global::skin->getImage("fullscreen"));
	connect(fullscreen, SIGNAL(clicked()), this, SIGNAL(toggleFullScreen()));

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
	fullscreen->setStatus(fs ? EnablingButton::OFF : EnablingButton::DISABLED);
}

void CameraMove::setMoveEnabled(bool move)
{
	up->setStatus(move ? EnablingButton::OFF : EnablingButton::DISABLED);
	down->setStatus(move ? EnablingButton::OFF : EnablingButton::DISABLED);
	left->setStatus(move ? EnablingButton::OFF : EnablingButton::DISABLED);
	right->setStatus(move ? EnablingButton::OFF : EnablingButton::DISABLED);
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


VCTCallStatus::VCTCallStatus()
{
	init();
}

void VCTCallStatus::init()
{
	connected = false;
	mute = EnablingButton::DISABLED;
}



VCTCall::VCTCall(EntryphoneDevice *d, VCTCallStatus *st, FormatVideo f)
{
	call_status = st;
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

	QString call_icon = bt_global::skin->getImage("call");
	call_accept = new BtButton;
	call_accept->setOnOff();
	call_accept->setImage(getBostikName(call_icon, "off"));
	call_accept->setPressedImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));

	volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));

	mute_icon = bt_global::skin->getImage("mute");
	mute_button = getButton(getBostikName(mute_icon, "off"));
	mute_button->setPressedImage(getBostikName(mute_icon, "on"));
	mute_button->setStatus(EnablingButton::DISABLED);
	connect(mute_button, SIGNAL(clicked()), SLOT(toggleMute()));

	stairlight = getButton(bt_global::skin->getImage("stairlight"));
	connect(stairlight, SIGNAL(pressed()), dev, SLOT(stairLightActivate()));
	connect(stairlight, SIGNAL(released()), dev, SLOT(stairLightRelease()));

	unlock_door = getButton(bt_global::skin->getImage("unlock_door"));
	connect(unlock_door, SIGNAL(pressed()), dev, SLOT(openLock()));
	connect(unlock_door, SIGNAL(released()), dev, SLOT(releaseLock()));

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));
	connect(cycle, SIGNAL(clicked()), dev, SLOT(cycleExternalUnits()));
}

void VCTCall::changeVolume(int value)
{
	call_status->volume_status = volume->getStatus();
	setVolume(VOLUME_VIDEOCONTROL, value);
}

void VCTCall::refreshStatus()
{
	call_accept->setStatus(call_status->connected);
	volume->setStatus(call_status->volume_status);
	mute_button->setStatus(call_status->mute);
}

void VCTCall::toggleMute()
{
	EnablingButton::Status st = mute_button->getStatus();
	setVolume(VOLUME_MIC, st == EnablingButton::ON ? 0 : 1);

	if (st == EnablingButton::ON)
	{
		mute_button->setStatus(EnablingButton::OFF);
		call_status->mute = EnablingButton::OFF;
	}
	else
	{
		mute_button->setStatus(EnablingButton::ON);
		call_status->mute = EnablingButton::ON;
	}
}

void VCTCall::toggleCall()
{
	call_status->connected = !call_status->connected;
	if (call_status->connected)
		mute_button->setStatus(EnablingButton::OFF);
	else
		mute_button->setStatus(EnablingButton::DISABLED);
	call_status->mute = mute_button->getStatus();

	refreshStatus();
	if (call_status->connected)
		dev->answerCall();
	else
		handleClose();
}

bool VCTCall::startVideo()
{
	if (video_grabber.state() == QProcess::NotRunning)
	{
		QStringList args;
		QPoint top_left = video_box->mapToGlobal(QPoint(0, 0));
		args << QString::number(top_left.x()) << QString::number(top_left.y()) << QString::number(format);
		video_grabber.start(video_grabber_path, args);
		return true;
	}
	return false;
}

bool VCTCall::stopVideo()
{
	if (video_grabber.state() == QProcess::Running)
	{
		video_grabber.terminate();
		return true;
	}
	return false;
}

void VCTCall::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::VCT_CALL:
			if (startVideo())
				emit incomingCall();
			break;
		case EntryphoneDevice::END_OF_CALL:
			if (stopVideo())
				emit callClosed();
			break;
		case EntryphoneDevice::MOVING_CAMERA:
			camera->setMoveEnabled(it.value().toBool());
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
	call_status = new VCTCallStatus;

	vct_call = new VCTCall(d, call_status, VCTCall::NORMAL_VIDEO);
	call_status->volume_status = vct_call->volume->getStatus();

	connect(vct_call, SIGNAL(callClosed()), SLOT(handleClose()));
	connect(vct_call, SIGNAL(incomingCall()), SLOT(showPage()));

	window = new VCTCallWindow(d, call_status);
	connect(window, SIGNAL(Closed()), SLOT(handleClose()));
	connect(vct_call->camera, SIGNAL(toggleFullScreen()), SLOT(enterFullScreen()));
	connect(window, SIGNAL(exitFullScreen()), SLOT(exitFullScreen()));

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
	connect(back, SIGNAL(clicked()), SLOT(handleClose()));

	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 0, 0);
	bottom->setSpacing(BOTTOM_SPACING);
	bottom->addWidget(back);

	bottom->addWidget(vct_call->call_accept);
	bottom->addWidget(vct_call->volume);
	bottom->addWidget(vct_call->mute_button);
	bottom->addWidget(vct_call->stairlight);
	bottom->addWidget(vct_call->unlock_door);
	bottom->addWidget(vct_call->cycle);

	QGridLayout *layout = new QGridLayout(this);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	layout->addLayout(sidebar, 0, 1, 2, 1);
	layout->addWidget(vct_call->video_box, 1, 0);
	layout->addLayout(bottom, 2, 0, 1, 2, Qt::AlignLeft);
	layout->setContentsMargins(10, 0, 0, 10);
	layout->setSpacing(10);
}

void VCTCallPage::enterFullScreen()
{
	// Signals from vct_call must be managed only when the window is not visible.
	vct_call->stopVideo();
	vct_call->blockSignals(true);
	window->showWindow();
}

void VCTCallPage::exitFullScreen()
{
	vct_call->startVideo();
	vct_call->refreshStatus();
	vct_call->blockSignals(false);
	Page::showPage();
}

void VCTCallPage::showPage()
{
	bt_global::page_stack.showVCTCall(this);
	call_status->init();
	vct_call->refreshStatus();
	if (bt_global::display.currentState() != DISPLAY_FREEZED)
		bt_global::display.forceOperativeMode(true);
	Page::showPage();
}


void VCTCallPage::handleClose()
{
	bt_global::display.forceOperativeMode(false);
	emit Closed();
}


VCTCallWindow::VCTCallWindow(EntryphoneDevice *d, VCTCallStatus *call_status)
{
	vct_call = new VCTCall(d, call_status, VCTCall::FULLSCREEN_VIDEO);
	vct_call->blockSignals(true);

	// Signals from vct_call must be managed only when the window is visible.
	connect(vct_call, SIGNAL(callClosed()), SLOT(handleClose()));
	connect(vct_call->camera, SIGNAL(toggleFullScreen()), SLOT(fullScreenExit()));

	QGridLayout *buttons_layout = new QGridLayout;
	buttons_layout->setContentsMargins(23, 0, 23, 0);
	buttons_layout->setSpacing(5);
	buttons_layout->addWidget(vct_call->call_accept, 0, 0);
	buttons_layout->addWidget(vct_call->setup_vct, 0, 1);
	buttons_layout->addWidget(vct_call->volume, 1, 0, 1, 2, Qt::AlignCenter);
	buttons_layout->addWidget(vct_call->mute_button, 2, 0);
	buttons_layout->addWidget(vct_call->unlock_door, 2, 1);
	buttons_layout->addWidget(vct_call->stairlight, 3, 0);
	buttons_layout->addWidget(vct_call->cycle, 3, 1);

	QVBoxLayout *sidebar = new QVBoxLayout;
	sidebar->setContentsMargins(0, 0, 0, 5);
	sidebar->setSpacing(5);
	sidebar->addStretch(1);
	sidebar->addWidget(vct_call->camera, 0, Qt::AlignCenter);
	sidebar->addWidget(vct_call->image_control, 0, Qt::AlignCenter);
	sidebar->addSpacing(20);
	sidebar->addLayout(buttons_layout);
	vct_call->video_box->setFixedSize(610, 460);

	QGridLayout *layout = new QGridLayout(this);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	layout->addLayout(sidebar, 0, 1, 2, 1);
	layout->addWidget(vct_call->video_box, 1, 0);
	layout->setContentsMargins(10, 0, 0, 10);
	layout->setSpacing(10);
}

void VCTCallWindow::showWindow()
{
	vct_call->blockSignals(false);
	vct_call->startVideo();
	vct_call->refreshStatus();
	Window::showWindow();
}

void VCTCallWindow::fullScreenExit()
{
	vct_call->stopVideo();
	vct_call->blockSignals(true);
	emit exitFullScreen();
}

void VCTCallWindow::handleClose()
{
	vct_call->stopVideo();
	vct_call->blockSignals(true);
	emit Closed();
}

#endif
