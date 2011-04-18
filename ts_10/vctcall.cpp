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
#include "hardware_functions.h" // setVctContrast
#include "displaycontrol.h" // bt_global::display
#include "icondispatcher.h"
#include "videodoorentry_device.h"
#include "xml_functions.h"
#include "bann2_buttons.h"
#include "items.h" // ItemTuning
#include "pagestack.h" // bt_global::page_stack
#include "btmain.h" // isCalibrating, vde_call_active
#include "state_button.h"
#include "audiostatemachine.h"
#include "homewindow.h" // TrayBar
#include "ringtonesmanager.h" // bt_global::ringtones
#include "main.h" // VIDEODOORENTRY
#include "btmain.h" // makeActive
#include "videodoorentry.h" // VideoDoorEntry::ring_exclusion

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QSpacerItem>
#include <QGridLayout>
#include <QApplication>

#define BOTTOM_SPACING 15

const QString video_grabber_path = "/home/bticino/bin/rsize";
const QString video_grabber_normal_args = "20 160 0 0 0";
const QString video_grabber_fullscreen_args = "5 12 1 0 0";


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
		bool call_active;
		bool move_enabled;
		bool video_enabled;
		ItemTuningStatus color_status;
		ItemTuningStatus brightness_status;
		ItemTuningStatus contrast_status;

		VCTCallStatus();

		// This method is used to reset the status of the call every time
		// a new videocall is performed (so, attributes like the volume of the
		// audio are not set in the init method because they must to be
		// preserved betweeen different calls).
		void resetStatus();
	};

	// We build the VCTCallStatus here because some the members hands_free and prof_studio
	// are used in other context, that might be constructed before the VCTCallPage.
	VCTCallStatus *VCTCall::call_status = new VCTCallStatus;
}

using namespace VCTCallPrivate;


StateButton *getButton(const QString &image_path)
{
	StateButton *btn = new StateButton;
	btn->setOffImage(image_path);
	btn->setDisabledImage(getBostikName(image_path, "dis"));
	return btn;
}


VCTCallStatus::VCTCallStatus()
{
	// Hands free and Professional studio are initialized here, see the comment
	// above regarding the resetStatus() method.
	hands_free = false;
	prof_studio = false;
	video_enabled = true;
	resetStatus();
}

void VCTCallStatus::resetStatus()
{
	connected = false;
	stopped = false;
	mute = StateButton::DISABLED;
	call_active = false;
	move_enabled = false;
}


CameraMove::CameraMove(VideoDoorEntryDevice *dev, bool is_fullscreen)
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

	fullscreen = getButton(bt_global::skin->getImage(is_fullscreen ? "normalscreen" : "fullscreen"));
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

void CameraMove::showFullScreenButton(bool show)
{
	fullscreen->setVisible(show);
}

void CameraMove::setMoveEnabled(bool move)
{
	up->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	down->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	left->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
	right->setStatus(move ? StateButton::OFF : StateButton::DISABLED);
}


CameraImageControl::CameraImageControl()
{
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	brightness = new ItemTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	l->addWidget(brightness, 1, Qt::AlignHCenter);

	contrast = new ItemTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	l->addWidget(contrast, 1, Qt::AlignHCenter);

	color = new ItemTuning(tr("Color"), bt_global::skin->getImage("color"));
	l->addWidget(color, 1, Qt::AlignHCenter);

	int default_level = 4;
	brightness->setLevel(default_level);
	contrast->setLevel(default_level);
	color->setLevel(default_level);
}


VCTCall::VCTCall(VideoDoorEntryDevice *d, FormatVideo f)
{
	format = f;
	dev = d;

	SkinContext ctx(666);

	image_control = new CameraImageControl;
	connect(image_control->brightness, SIGNAL(valueChanged(int)), SLOT(changeBrightness(int)));
	connect(image_control->color, SIGNAL(valueChanged(int)), SLOT(changeColor(int)));
	connect(image_control->contrast, SIGNAL(valueChanged(int)), SLOT(changeContrast(int)));

	camera = new CameraMove(dev, format == FULLSCREEN_VIDEO);
	camera->setMoveEnabled(false);
	camera->showFullScreenButton(dev->vctMode() == VideoDoorEntryDevice::SCS_MODE);

	setup_vct_icon = bt_global::skin->getImage("setup_vct");
	setup_vct = new BtButton();
	camera_settings_shown = false;
	toggleCameraSettings();
	connect(setup_vct, SIGNAL(clicked()), SLOT(toggleCameraSettings()));

	video_box = new QLabel;
	video_box->setStyleSheet("background-color: black");

	QString call_icon = bt_global::skin->getImage("call");
	call_accept = new StateButton;
	call_accept->setOffImage(getBostikName(call_icon, "off"));
	call_accept->setOnImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));

	volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	volume->disable();
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

	cycle = getButton(bt_global::skin->getImage("cycle"));
	connect(cycle, SIGNAL(clicked()), SLOT(cycleClicked()));
	connect(&video_grabber, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int,QProcess::ExitStatus)));
	disable();
}

void VCTCall::setVideoDefaults()
{
	int default_level = 4;
	changeBrightness(default_level);
	changeContrast(default_level);
	changeColor(default_level);
}

void VCTCall::cycleClicked()
{
	dev->cycleExternalUnits();
	call_status->stopped = true;
	stopVideo();
}

void VCTCall::enable()
{
	if (signalsBlocked())
	{
		blockSignals(false);
		connect(dev, SIGNAL(valueReceived(DeviceValues)), this, SLOT(valueReceived(DeviceValues)));
	}
}

void VCTCall::disable()
{
	if (!signalsBlocked())
	{
		blockSignals(true);
		disconnect(dev, SIGNAL(valueReceived(DeviceValues)), this, SLOT(valueReceived(DeviceValues)));
	}
}

void VCTCall::finished(int exitcode, QProcess::ExitStatus exitstatus)
{
	qDebug() << "rsize terminated";
	// Called even when we terminate the video process
	Q_UNUSED(exitcode)
	Q_UNUSED(exitstatus)
	bt_global::display->setDirectScreenAccess(false);
}

void VCTCall::changeVolume(int value)
{
	call_status->volume_status = volume->getStatus();
	bt_global::audio_states->setVolume(value);
}

void VCTCall::changeContrast(int value)
{
	call_status->contrast_status = image_control->contrast->getStatus();
	static const QString contrast_command("1");
	setVctVideoValue(contrast_command, QString::number(value));
}

void VCTCall::changeColor(int value)
{
	call_status->color_status = image_control->color->getStatus();
	static const QString color_command("2");
	setVctVideoValue(color_command, QString::number(value));
}

void VCTCall::changeBrightness(int value)
{
	call_status->brightness_status = image_control->brightness->getStatus();
	static const QString brightness_command("3");
	setVctVideoValue(brightness_command, QString::number(value));
}

void VCTCall::refreshStatus()
{
	call_accept->setStatus(call_status->connected);
	volume->setStatus(call_status->volume_status);
	mute_button->setStatus(call_status->mute);
	camera->setMoveEnabled(call_status->move_enabled);

	image_control->brightness->setStatus(call_status->brightness_status);
	image_control->color->setStatus(call_status->color_status);
	image_control->contrast->setStatus(call_status->contrast_status);
}

void VCTCall::toggleMute()
{
	StateButton::Status st = mute_button->getStatus();

	if (st == StateButton::ON)
	{
		bt_global::audio_states->removeState(AudioStates::MUTE);
		mute_button->setStatus(StateButton::OFF);
		call_status->mute = StateButton::OFF;
		volume->enable();
	}
	else
	{
		bt_global::audio_states->toState(AudioStates::MUTE);
		mute_button->setStatus(StateButton::ON);
		call_status->mute = StateButton::ON;
		volume->disable();
	}
}

void VCTCall::activateAudio()
{
	if (dev->ipCall())
		bt_global::audio_states->toState(AudioStates::IP_VIDEO_CALL);
	else
		bt_global::audio_states->toState(AudioStates::SCS_VIDEO_CALL);
	volume->enable();

	call_status->connected = true;
	call_status->mute = StateButton::OFF;
	refreshStatus();
}

void VCTCall::toggleCall()
{
	if (!call_status->connected)
	{
		if (call_status->stopped)
			resumeVideo();
		dev->answerCall();
	}
	else
	{
		// Se the comment about the IP calls on VCTCall::endCall
		if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
			handleClose();
		else
			endCall();
	}
}

void VCTCall::resumeVideo()
{
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(resumeVideo()));
	if (bt_global::display->isDirectScreenAccess())
	{
		connect(bt_global::display, SIGNAL(directScreenAccessStopped()), SLOT(resumeVideo()));
		return;
	}

	call_status->stopped = false;
	startVideo();
}

void VCTCall::startVideo()
{
	if (call_status->video_enabled && video_grabber.state() == QProcess::NotRunning)
	{
		QString args;
		if (format == NORMAL_VIDEO)
			args = video_grabber_normal_args;
		else // fullscreeen
			args = video_grabber_fullscreen_args;

		qDebug() << "start rsize";
		video_grabber.start(video_grabber_path + " " + args);
		bt_global::display->setDirectScreenAccess(true);
	}
}

void VCTCall::stopVideo()
{
	if (call_status->video_enabled && video_grabber.state() != QProcess::NotRunning)
	{
		qDebug() << "terminate rsize";
		video_grabber.terminate();
	}
}

void VCTCall::valueReceived(const DeviceValues &values_list)
{
	if (!call_status->call_active)
		if (!values_list.contains(VideoDoorEntryDevice::VCT_CALL) &&
			!values_list.contains(VideoDoorEntryDevice::AUTO_VCT_CALL))
		{
			return;
		}

	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case VideoDoorEntryDevice::VCT_CALL:
		case VideoDoorEntryDevice::AUTO_VCT_CALL:
			if (dev->ipCall())
				call_status->video_enabled = false;
			else
				call_status->video_enabled = (it.value().toInt() == VideoDoorEntryDevice::AUDIO_VIDEO);
			if (call_status->stopped && it.key() == VideoDoorEntryDevice::VCT_CALL)
				resumeVideo();
			else
				emit incomingCall();
			call_status->call_active = true;
			if (values_list.contains(VideoDoorEntryDevice::CALLER_ADDRESS))
				emit callerAddress(values_list[VideoDoorEntryDevice::CALLER_ADDRESS].toString());
			break;
		case VideoDoorEntryDevice::CALLER_ADDRESS:
			// We must manage the callerAddress after the incoming call.
			if (!values_list.contains(VideoDoorEntryDevice::VCT_CALL) &&
				!values_list.contains(VideoDoorEntryDevice::AUTO_VCT_CALL))
				emit callerAddress(it.value().toString());
			break;
		case VideoDoorEntryDevice::END_OF_CALL:
			stopVideo();
			cleanAudioStates();
			call_status->resetStatus();
			// Reset the timers for the freeze/screensaver.
			bt_global::btmain->makeActive();
			emit callClosed();
			break;
		case VideoDoorEntryDevice::STOP_VIDEO:
			call_status->stopped = true;
			stopVideo();
			break;
		case VideoDoorEntryDevice::VCT_TYPE:
		{
			bool new_status = (it.value().toInt() == VideoDoorEntryDevice::AUDIO_VIDEO);
			if (dev->ipCall())
			{
				new_status = false; // Ip calls has always the video disabled.
				emit updateScreen();

				// If we have already answer, we need to send again the answer.
				if (bt_global::audio_states->contains(AudioStates::IP_VIDEO_CALL))
					dev->answerCall();
			}
			bool old_status = call_status->video_enabled;

			// Switch from a camera with video to a camera without video
			if (old_status && !new_status)
				stopVideo();

			call_status->video_enabled = new_status;

			// Switch from a camera without video to a camera with video
			if ((!old_status || call_status->stopped) && new_status)
			{
				call_status->stopped = false;
				resumeVideo();
			}
			break;
		}
		case VideoDoorEntryDevice::MOVING_CAMERA:
			call_status->move_enabled = it.value().toBool();
			camera->setMoveEnabled(call_status->move_enabled);
			break;
		case VideoDoorEntryDevice::ANSWER_CALL:
			if (!call_status->connected)
				activateAudio();
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

void VCTCall::cleanAudioStates()
{
	// if mute, exit from the correspondent state
	if (call_status->mute == StateButton::ON)
		bt_global::audio_states->removeState(AudioStates::MUTE);

	// if connected, exit from the videocall state
	if (call_status->connected)
	{
		// We cannot use "dev->ipCall()" because the status of the device is reset
		// after the end of the call.
		if (bt_global::audio_states->contains(AudioStates::IP_VIDEO_CALL))
			bt_global::audio_states->removeState(AudioStates::IP_VIDEO_CALL);
		else
			bt_global::audio_states->removeState(AudioStates::SCS_VIDEO_CALL);
		volume->disable();
	}
}

void VCTCall::endCall()
{
	dev->endCall();

	// We close the page and change the audio state only for the SCS call. For
	// the IP call, after sent the END_OF_CALL frame, we have to wait an incoming
	// END_OF_CALL frame to avoid problems for the underlying audio system.
	if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
	{
		stopVideo();
		cleanAudioStates();
		call_status->resetStatus();
	}
}

void VCTCall::handleClose()
{
	endCall();
	emit callClosed();
}


VCTCallPage::VCTCallPage(VideoDoorEntryDevice *d)
{
	dev = d;
	already_closed = false;
	// Manage only the ringtones, the other values are managed by the VCTCall
	// object, to avoid duplicated code between the VCTCallPage and the VCTCallWindow.
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	// We assume that the VCTCall::call_status is previously built.
	vct_call = new VCTCall(d, VCTCall::NORMAL_VIDEO);

	vct_call->setVideoDefaults();
	vct_call->enable();
	VCTCall::call_status->volume_status = vct_call->volume->getStatus();

	connect(vct_call, SIGNAL(callClosed()), SLOT(handleClose()));
	connect(vct_call, SIGNAL(incomingCall()), SLOT(incomingCall()));
	connect(vct_call, SIGNAL(callerAddress(QString)), SLOT(callerAddress(QString)));
	connect(vct_call, SIGNAL(updateScreen()), SLOT(updateScreen()));

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


	if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
	{
		vct_call->video_box->setFixedSize(344, 240);
		sidebar->addWidget(vct_call->setup_vct, 0, Qt::AlignCenter);
	}
	else
	{
		// In ip mode we cannot modify the image, so we hide the button and
		// replace it with a spacer.
		vct_call->setup_vct->hide();
		sidebar->addSpacing(vct_call->setup_vct->height());
		vct_call->video_box->setFixedSize(320, 240);
	}

	BtButton *back = new BtButton(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SLOT(backClicked()));

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

	if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
		layout->addWidget(vct_call->video_box, 1, 0);
	else
	{
		QHBoxLayout *video_layout = new QHBoxLayout;
		video_layout->setContentsMargins(16, 0, 16, 0);
		video_layout->setSpacing(0);
		video_layout->addWidget(vct_call->video_box);
		layout->addLayout(video_layout, 1, 0);
	}
	layout->addLayout(bottom, 2, 0, 1, 2, Qt::AlignLeft);
	layout->setContentsMargins(20, 0, 0, 20);
	layout->setSpacing(10);
}

void VCTCallPage::updateScreen()
{
	update();
}

VCTCallPage::~VCTCallPage()
{
	delete VCTCall::call_status;
}

void VCTCallPage::closeCall()
{
	cleanUp();
	if (!isVisible()) // If the page is not visible, we are in fullscreen mode.
	{
		// Beacause the page and the window doesn't share the same vctcall (and
		// thus the same QProcess instance) we have to call the vctcall->stopVideo()
		// of the window (called inside the cleanUp) in order to terminate the
		// rsize process.
		window->cleanUp();
		bt_global::page_stack.closeWindow(window);
	}

	bt_global::page_stack.closePage(this);
}

void VCTCallPage::backClicked()
{
	vct_call->endCall();
	if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
		handleClose();
}

void VCTCallPage::valueReceived(const DeviceValues &values_list)
{
	if (values_list.contains(VideoDoorEntryDevice::RINGTONE))
	{
		bool ring_exclusion = VideoDoorEntry::ring_exclusion;

		ringtone = values_list[VideoDoorEntryDevice::RINGTONE].toInt();
		if (ringtone == Ringtones::PE1 || ringtone == Ringtones::PE2 ||
			ringtone == Ringtones::PE3 || ringtone == Ringtones::PE4)
		{
			if (!ring_exclusion)
				connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(playRingtone()));

			// We always enter in the VDE_RINGTONE state, because if we are running an audio files
			// we want to stop the reproduction even if the ring exclusion is on.
			if (bt_global::audio_states->currentState() != AudioStates::PLAY_VDE_RINGTONE)
				bt_global::audio_states->toState(AudioStates::PLAY_VDE_RINGTONE);
			else if (!ring_exclusion)
				playRingtone();

			// We are supposing that the VCT ringtones arrive (only) together the VCT call.
			if (ring_exclusion) // We have not to wait the end of the ringtone
				manageHandsFree();
		}
	}
}

void VCTCallPage::cleanUp()
{
	// the cleanUp is performed when we exit from the page using an external
	// button. In this case, we have to send the end of call (even if is an
	// autoswitch call) and terminate the video.
	vct_call->endCall();

	already_closed = true;
	// For IP calls we have to send the end of call frame and wait that the same
	// frame came back on the monitor channel to avoid problems with the underlying
	// audio system.
	// But the cleanUp method is designed to be syncronous, so we have to use this
	// trick to obtain the same result (the page is closed and the script audio were
	// called only after the incoming END_OF_CALL frame).
	// TODO: we can do better refactor the pagestack!
	if (dev->vctMode() == VideoDoorEntryDevice::IP_MODE)
	{
		while (vct_call->call_status->call_active) // We wait for the callClosed()
			QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	else
		handleClose();
}

void VCTCallPage::handleClose()
{
	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(playRingtone()));
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showPage()));

	if (bt_global::audio_states->contains(AudioStates::PLAY_VDE_RINGTONE))
	{
		bt_global::audio_states->removeState(AudioStates::PLAY_VDE_RINGTONE);
		bt_global::ringtones->stopRingtone();
	}

	vct_call->stopVideo();
	bt_global::btmain->vde_call_active = false;
	vct_call->enable();
	if (!already_closed)
		emit Closed();

	already_closed = false;
}

int VCTCallPage::sectionId() const
{
	return VIDEODOORENTRY;
}

void VCTCallPage::playRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(playRingtone()));
	connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(manageHandsFree()));
	bt_global::ringtones->playRingtone(static_cast<Ringtones::Type>(ringtone));
}

void VCTCallPage::manageHandsFree()
{
	disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(manageHandsFree()));
	if (VCTCall::call_status->hands_free)
		vct_call->toggleCall();
}

void VCTCallPage::enterFullScreen()
{
	// We need this two-pass signal-slot because we have to wait until the
	// terminating process exit. We don't care about calling twice the stopVideo
	// method (see the doc of that method)
	vct_call->stopVideo();
	connect(bt_global::display, SIGNAL(directScreenAccessStopped()), SLOT(showVCTWindow()));
}

void VCTCallPage::showVCTWindow()
{
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showVCTWindow()));
	// Signals from vct_call must be managed only when the window is not visible.
	vct_call->disable();
	window->showWindow();
}

void VCTCallPage::exitFullScreen()
{
	vct_call->refreshStatus();
	vct_call->enable();
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
	vct_call->refreshStatus();
	// Restore the 'normal' status of these buttons, that can be disabled when
	// calling the guard unit.
	vct_call->stairlight->setStatus(StateButton::OFF);
	vct_call->unlock_door->setStatus(StateButton::OFF);
	vct_call->cycle->setStatus(StateButton::OFF);

	showPage();
	bt_global::btmain->vde_call_active = true;
}

void VCTCallPage::callerAddress(QString address)
{
	QString addr = address;
	bool is_autoswitch = false;

	if (address.at(0) == '@')
	{
		addr = addr.mid(1);
		is_autoswitch = true;
	}

	// we want to open the door (only if the call does not come from an autoswitch)
	if (VCTCall::call_status->prof_studio && !is_autoswitch)
	{
		dev->openLock();
		dev->releaseLock();
	}

	if (addr == (*bt_global::config)[GUARD_UNIT_ADDRESS])
	{
		vct_call->call_status->move_enabled = false;
		vct_call->camera->setMoveEnabled(false);

		vct_call->stairlight->setStatus(StateButton::DISABLED);
		vct_call->unlock_door->setStatus(StateButton::DISABLED);
		vct_call->cycle->setStatus(StateButton::DISABLED);
	}
}

void VCTCallPage::showPage()
{
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showPage()));
	if (bt_global::display->isDirectScreenAccess())
	{
		connect(bt_global::display, SIGNAL(directScreenAccessStopped()), SLOT(showPage()));
		return;
	}

	bt_global::page_stack.showUserPage(this);
	// When we exit from the screensaver to enter in the screenoff state the code
	// calls the showPage on the page where the screensaver starts.
	// And if the showPage contains a makeActive calls the UI doesn't exit from the
	// screen off state.
	// Fortunately in this case the problem is only theorical, because the screensaver
	// can't start if the vct page is still on the top of the stack.
	bt_global::btmain->makeActive();
	Page::showPage();

	// We must start the video after draw the page
	repaint();
	// TODO the check should never be necessary (the window is never shown
	//      during calibration)
	if (!BtMain::isCalibrating())
		vct_call->startVideo();
}


VCTCallWindow::VCTCallWindow(VideoDoorEntryDevice *d)
{
	vct_call = new VCTCall(d, VCTCall::FULLSCREEN_VIDEO);
	vct_call->disable();

	// Signals from vct_call must be managed only when the window is visible.
	connect(vct_call, SIGNAL(callClosed()), SLOT(handleClose()));
	connect(vct_call->camera, SIGNAL(toggleFullScreen()), SLOT(fullScreenExit()));
	connect(vct_call, SIGNAL(updateScreen()), SLOT(updateScreen()));

	QGridLayout *buttons_layout = new QGridLayout;
	buttons_layout->setContentsMargins(30, 0, 30, 0);
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

void VCTCallWindow::updateScreen()
{
	update();
}

void VCTCallWindow::showWindow()
{
	vct_call->enable();
	vct_call->startVideo();
	vct_call->refreshStatus();
	bt_global::page_stack.showUserWindow(this);
	Window::showWindow();

	// We must start the video after draw the window
	repaint();
	// TODO the check should never be necessary (the window is never shown
	//      during calibration)
	if (!BtMain::isCalibrating())
		vct_call->startVideo();
}

void VCTCallWindow::fullScreenExit()
{
	vct_call->stopVideo();
	connect(bt_global::display, SIGNAL(directScreenAccessStopped()), SLOT(showVCTPage()));
}

void VCTCallWindow::showVCTPage()
{
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showVCTPage()));
	vct_call->disable();
	emit exitFullScreen();
	bt_global::page_stack.closeWindow(this);
}

void VCTCallWindow::handleClose()
{
	vct_call->stopVideo();
	vct_call->disable();
	emit Closed();
}

void VCTCallWindow::cleanUp()
{
	vct_call->stopVideo();
	vct_call->disable();
}
