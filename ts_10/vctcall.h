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


#ifndef VCTCALL_H
#define VCTCALL_H

#include "page.h"
#include "window.h"

#include <QString>
#include <QStringList>
#include <QProcess>


class VideoDoorEntryDevice;
class ItemTuning;
class QDomNode;
class QHBoxLayout;
class QLabel;
class QShowEvent;
class StateButton;
class BtButton;


namespace VCTCallPrivate
{
	class VCTCallStatus;

	/*!
		\brief The pad to control the movements of the camera and toggle the fullscreen mode.
	*/
	class CameraMove : public QWidget
	{
	Q_OBJECT
	public:
		CameraMove(VideoDoorEntryDevice *dev);
		void showFullScreenButton(bool show);
		void setMoveEnabled(bool move);

	signals:
		void toggleFullScreen();

	private:
		StateButton *up, *left, *fullscreen, *right, *down;
	};


	/*!
		\brief The widget that contains the controls for the image of the video call.
	*/
	class CameraImageControl : public QWidget
	{
	Q_OBJECT
	public:
		CameraImageControl(QWidget *parent = 0);

	private slots:
		void setBrightness(int value);
		void setColor(int value);
		void setContrast(int value);

	private:
		ItemTuning *contrast, *brightness, *color;
	};


	/*!
		\brief This object encapsulates the common stuff between the VCTCallPage
		and VCTCallWindow.

		Graphical objects are created here but placed by the page or the window.
	*/
	class VCTCall : public QObject
	{
	Q_OBJECT
	public:
		enum FormatVideo
		{
			NORMAL_VIDEO = 0,
			FULLSCREEN_VIDEO = 1,
		};

		VCTCall(VideoDoorEntryDevice *d, FormatVideo f);
		void refreshStatus();

		// Start the video process, if it is not already running
		void startVideo();

		// Stop the video process, if it is running
		void stopVideo();

		// Only 1 instance can be active at the same time, so we have to use the
		// enable/disable methods to ensure that.
		void enable();
		void disable();

		// Common graphical objects
		BtButton *setup_vct;
		CameraMove *camera;
		CameraImageControl *image_control;
		QLabel *video_box;
		QString setup_vct_icon;

		StateButton *mute_button, *stairlight, *unlock_door, *call_accept, *cycle;
		QString mute_icon;
		ItemTuning *volume;
		static VCTCallStatus *call_status;

	public slots:
		void endCall();
		void toggleCall();

	signals:
		void callClosed();
		void incomingCall();
		void callerAddress(QString address);

	private slots:
		void valueReceived(const DeviceValues &values_list);
		void toggleCameraSettings();
		void handleClose();
		void toggleMute();
		void changeVolume(int value);
		void finished(int exitcode, QProcess::ExitStatus exitstatus);
		void resumeVideo();
		void cleanAudioStates();

	private:
		FormatVideo format;
		bool camera_settings_shown;
		VideoDoorEntryDevice *dev;
		QProcess video_grabber;
	};

	/*!
		\brief The window of the video call. It is showed only by the page.
	*/
	class VCTCallWindow : public Window
	{
	Q_OBJECT
	public:
		VCTCallWindow(VideoDoorEntryDevice *d);

	public slots:
		virtual void showWindow();

	signals:
		void exitFullScreen();

	protected:
		void showEvent(QShowEvent *);
		void hideEvent(QHideEvent *);

	private slots:
		void handleClose();
		void fullScreenExit();
		void showVCTPage();

	private:
		VCTCall *vct_call;
	};

}


/*!
	\ingroup VideoDoorEntry
	\brief The page for video calls.

	This is the main class of the \ref VideoDoorEntry system, that manages the
	audio-video parts of a videocall, handle the fullscreen mode, the
	ProfessionalStudio and HandsFree facilities.

	A %VctCallPage is always shown indirectly: even if we want to switch on a
	camera we actually request the switch and wait for the call from the camera.

	The page is also behaves like a popup-page: it breaks the navigation, the
	screensaver or everything else (with the exception of the calibration, that
	has the priority).
*/
class VCTCallPage : public Page
{
Q_OBJECT
public:
	VCTCallPage(VideoDoorEntryDevice *d);
	~VCTCallPage();

	static void setHandsFree(bool on);
	static void setProfStudio(bool on);

	virtual int sectionId() const;
	virtual void showPage();

public slots:
	virtual void cleanUp();

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private slots:
	void incomingCall();
	void handleClose();
	void enterFullScreen();
	void exitFullScreen();
	void callerAddress(QString address);
	void showVCTWindow();
	void valueReceived(const DeviceValues &values_list);
	void playRingtone();
	void backClicked();

	void manageHandsFree();

private:
	int ringtone;
	VCTCallPrivate::VCTCallWindow *window;
	VCTCallPrivate::VCTCall *vct_call;
	VideoDoorEntryDevice *dev;
	bool already_closed;
};

#endif //VCTCALL_H
