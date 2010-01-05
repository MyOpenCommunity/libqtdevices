#ifndef VCTCALLPAGE_H
#define VCTCALLPAGE_H

#include "page.h"
#include "window.h"
#include "btbutton.h"

#include <QString>
#include <QStringList>
#include <QProcess>


class BannTuning;
class EntryphoneDevice;
class QDomNode;
class QHBoxLayout;
class QLabel;
class QShowEvent;
class ItemTuning;


class EnablingButton : public BtButton
{
Q_OBJECT
public:
	EnablingButton(QWidget *parent = 0);
	void setDisabledPixmap(const QString &path);
	virtual void enable();
	virtual void disable();

private:
	QPixmap disabled_pixmap;
};


/**
 * The pad to control the movements of the camera (if the camera support them)
 * and to show the video call page as a window.
 */
class CameraMove : public QWidget // TODO: should be a StyledWidget??
{
Q_OBJECT
public:
	// TODO: we need a EntryphoneDevice to control the camera
	CameraMove(EntryphoneDevice *dev);
	void setFullscreenEnabled(bool fs);
	void setMoveEnabled(bool move);

signals:
	void toggleFullScreen();

private:
	EnablingButton *up, *left, *fullscreen, *right, *down;
};


/**
 * The widget that contains the controls for the image of the video call (usually
 * placed on the right of the video area).
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


struct VCTCallStatus
{
	bool connected;

	VCTCallStatus() { init(); }

	void init()
	{
		connected = false;
	}
};


class VCTCall : public QObject
{
Q_OBJECT
public:
	enum FormatVideo
	{
		NORMAL_VIDEO = 0,
		FULLSCREEN_VIDEO = 1,
	};

	VCTCall(EntryphoneDevice *d, VCTCallStatus *st, FormatVideo f);
	void refreshStatus();
	void startVideo();
	void stopVideo();

	BtButton *setup_vct;
	CameraMove *camera;
	CameraImageControl *image_control;
	QLabel *video_box;
	QString setup_vct_icon;

	EnablingButton *mute_button, *stairlight, *unlock_door;
	BtButton *cycle, *call_accept;
	QString mute_icon, call_icon;
	ItemTuning *volume;

public slots:
	void endCall();

signals:
	void callClosed();
	void incomingCall();

private slots:
	void status_changed(const StatusList &sl);
	void toggleCameraSettings();
	void handleClose();
	void toggleCall();

private:
	FormatVideo format;
	bool camera_settings_shown;
	EntryphoneDevice *dev;
	QProcess video_grabber;
	VCTCallStatus *call_status;
};


class VCTCallWindow : public Window
{
Q_OBJECT
public:
	VCTCallWindow(EntryphoneDevice *d, VCTCallStatus *call_status);

public slots:
	virtual void showWindow();

signals:
	void exitFullScreen();

private slots:
	void handleClose();

private:
	VCTCall *vct_call;
};


/**
 * The page of the video call. It is showed indirecly when a call frame came
 * from the Openserver.
 */
class VCTCallPage : public Page
{
Q_OBJECT
public:
	VCTCallPage(EntryphoneDevice *d);

public slots:
	void showPreviousPage();


private slots:
	virtual void showPage();
	void handleClose();
	void enterFullScreen();
	void exitFullScreen();

private:
	Page *prev_page;
	Window *window;
	VCTCallStatus *call_status;
	VCTCall *vct_call;
};


#endif //VCTCALLPAGE_H
