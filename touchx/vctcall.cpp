/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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
#include "pagestack.h" // bt_global::page_stack
#include "btmain.h" // isCalibrating
#include "state_button.h"

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QGridLayout>

#define BOTTOM_SPACING 15

const QString video_grabber_path = "/home/bticino/bin/rsize";
const QString video_grabber_normal_args = "20 160 0";
const QString video_grabber_fullscreen_args = "5 12 1";


namespace VCTCallPrivate
{
	// The struct used to save and restore the status of a video call
	struct VCTCallStatus
	{
		bool connected;
		bool stopped;
		StateButton::Status mute;
		ItemTuningStatus volume_status;
		bool hands_free;
		bool prof_studio;

		VCTCallStatus();

		// This method is used to initialize the status of the call every time
		// a new videocall is performed (so, attributes like the volume of the
		// audio are not set in the init method because they must to be
		// preserved betweeen different calls).
		void init();
	};

	VCTCallStatus *VCTCall::call_status = 0;
}

using namespace VCTCallPrivate;



StateButton *getButton(const QString &image_path)
{
	StateButton *btn = new StateButton;
	btn->setOffImage(image_path);
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
	fullscreen->setStatus(fs ? StateButton::OFF : StateButton::DISABLED);
}

void CameraMove::setMoveEnabled(bool move)
{
	up->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	down->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	left->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	right->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
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
	stopped = false;
	mute = StateButton::DISABLED;
}



VCTCall::VCTCall(EntryphoneDevice *d, FormatVideo f)
{
	format = f;
	dev = d;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

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
	call_accept = new StateButton;
	call_accept->setOnOff();
	call_accept->setOffImage(getBostikName(call_icon, "off"));
	call_accept->setOnImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));

	volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));

	mute_icon = bt_global::skin->getImage("mute");
	mute_button = getButton(getBostikName(mute_icon, "off"));
	mute_button->setOnImage(getBostikName(mute_icon, "on"));
	mute_button->setStatus(StateButton::DISABLED);
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
	connect(&video_grabber, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int,QProcess::ExitStatus)));
}

void VCTCall::finished(int exitcode, QProcess::ExitStatus exitstatus)
{
	Q_UNUSED(exitcode)
	Q_UNUSED(exitstatus)
	emit videoFinished();
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
	StateButton::Status st = mute_button->getStatus();
	setVolume(VOLUME_MIC, st == StateButton::ON ? 0 : 1);

	if (st == StateButton::ON)
	{
		mute_button->setStatus(StateButton::OFF);
		call_status->mute = StateButton::OFF;
	}
	else
	{
		mute_button->setStatus(StateButton::ON);
		call_status->mute = StateButton::ON;
	}
}

void VCTCall::toggleCall()
{
	call_status->connected = !call_status->connected;
	call_status->mute = call_status->connected ? StateButton::OFF : StateButton::DISABLED;

	refreshStatus();
	if (call_status->connected)
	{
		if (call_status->stopped)
			resumeVideo();
		dev->answerCall();
	}
	else
		handleClose();
}

void VCTCall::resumeVideo()
{
	// We have to wait the ending of the process to restart the process.
	if (video_grabber.state() == QProcess::NotRunning)
	{
		call_status->stopped = false;
		disconnect(this, SIGNAL(videoFinished()), this, SLOT(resumeVideo()));
		startVideo();
	}
	else // we re-try when the video is terminated.
		connect(this, SIGNAL(videoFinished()), this, SLOT(resumeVideo()));
}

void VCTCall::startVideo()
{
	if (video_grabber.state() == QProcess::NotRunning)
	{
		QString args;
		if (format == NORMAL_VIDEO)
			args = video_grabber_normal_args;
		else // fullscreeen
			args = video_grabber_fullscreen_args;

		video_grabber.start(video_grabber_path + " " + args);
	}
}

void VCTCall::stopVideo()
{
	if (video_grabber.state() == QProcess::Running)
		video_grabber.terminate();
}

void VCTCall::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::VCT_CALL:
			if (call_status->stopped)
				resumeVideo();
			else
				emit incomingCall();
			break;
		case EntryphoneDevice::AUTO_VCT_CALL:
			emit autoIncomingCall();
			break;
		case EntryphoneDevice::CALLER_ADDRESS:
			emit callerAddress();
			break;
		case EntryphoneDevice::END_OF_CALL:
			stopVideo();
			emit callClosed();
			break;
		case EntryphoneDevice::STOP_VIDEO:
			call_status->stopped = true;
			stopVideo();
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
	stopVideo();
}

void VCTCall::handleClose()
{
	endCall();
	emit callClosed();
}


VCTCallPage::VCTCallPage(EntryphoneDevice *d)
{
	dev = d;
	// There is only 1 VCTCallPage instance, so I can build the VCTCallStatus here.
	VCTCall::call_status = new VCTCallStatus;
	vct_call = new VCTCall(d, VCTCall::NORMAL_VIDEO);
	VCTCall::call_status->volume_status = vct_call->volume->getStatus();

	connect(vct_call, SIGNAL(callClosed()), SLOT(handleClose()));
	connect(vct_call, SIGNAL(incomingCall()), SLOT(incomingCall()));
	connect(vct_call, SIGNAL(autoIncomingCall()), SLOT(autoIncomingCall()));
	connect(vct_call, SIGNAL(callerAddress()), SLOT(callerAddress()));

	window = new VCTCallWindow(d);
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
	sidebar->addWidget(vct_call->setup_vct, 0, Qt::AlignCenter);

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
	layout->setContentsMargins(20, 0, 0, 20);
	layout->setSpacing(10);
}

VCTCallPage::~VCTCallPage()
{
	delete VCTCall::call_status;
}

void VCTCallPage::cleanUp()
{
	// the cleanUp is performed when we exit from the page using an external
	// button. In this case, we have to send the end of call (even if is an
	// autoswitch call) and terminate the video.
	vct_call->endCall();

	bt_global::display->forceOperativeMode(false);
	vct_call->blockSignals(false);
}

void VCTCallPage::handleClose()
{
	bt_global::display->forceOperativeMode(false);
	vct_call->blockSignals(false);
	emit Closed();
}

int VCTCallPage::sectionId()
{
	return VIDEOCITOFONIA;
}

void VCTCallPage::enterFullScreen()
{
	// We need this two-pass signal-slot because we have to wait until the
	// terminating process exit.
	vct_call->stopVideo();
	connect(vct_call, SIGNAL(videoFinished()), this, SLOT(showVCTWindow()));
}

void VCTCallPage::showVCTWindow()
{
	disconnect(vct_call, SIGNAL(videoFinished()), this, SLOT(showVCTWindow()));
	// Signals from vct_call must be managed only when the window is not visible.
	vct_call->blockSignals(true);
	window->showWindow();
}

void VCTCallPage::exitFullScreen()
{
	vct_call->startVideo();
	vct_call->refreshStatus();
	vct_call->blockSignals(false);
	showPage();
}

void VCTCallPage::setHandsFree(bool on)
{
	VCTCall::call_status->hands_free = on;
}

void VCTCallPage::setProfStudio(bool on)
{
	VCTCall::call_status->prof_studio = on;
}

void VCTCallPage::incomingCall()
{
	autoIncomingCall();
	// We want to answer automatically if hands free is on.
	if (VCTCall::call_status->hands_free)
		vct_call->toggleCall();
}

void VCTCallPage::callerAddress()
{
	if (VCTCall::call_status->prof_studio) // we want to open the door
	{
		dev->openLock();
		dev->releaseLock();
	}
}

void VCTCallPage::autoIncomingCall()
{
	bt_global::page_stack.showVCTPage(this);
	VCTCall::call_status->init();
	vct_call->refreshStatus();

	showPage();
	repaint();

	if (!BtMain::isCalibrating())
	{
		vct_call->startVideo();
		if (bt_global::display->currentState() != DISPLAY_FREEZED)
			bt_global::display->forceOperativeMode(true);
	}

}



VCTCallWindow::VCTCallWindow(EntryphoneDevice *d)
{
	vct_call = new VCTCall(d, VCTCall::FULLSCREEN_VIDEO);
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
	vct_call->video_box->setFixedSize(624, 455);

	QGridLayout *layout = new QGridLayout(this);
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	layout->addLayout(sidebar, 0, 1, 2, 1);
	layout->addWidget(vct_call->video_box, 1, 0);
	layout->setContentsMargins(5, 0, 0, 12);
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
	connect(vct_call, SIGNAL(videoFinished()), this, SLOT(showVCTPage()));
}

void VCTCallWindow::showVCTPage()
{
	disconnect(vct_call, SIGNAL(videoFinished()), this, SLOT(showVCTPage()));
	vct_call->blockSignals(true);
	emit exitFullScreen();
}

void VCTCallWindow::handleClose()
{
	vct_call->stopVideo();
	vct_call->blockSignals(true);
	emit Closed();
}
