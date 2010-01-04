#ifndef VCTCALLPAGE_H
#define VCTCALLPAGE_H

#include "page.h"
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



/**
 * The widget that contains all the button to control the videocall (usually
 * placed at the bottom of the page).
 */
class CallControl : public QWidget
{
Q_OBJECT
public:
	CallControl(EntryphoneDevice *d);

signals:
	void endCall();

protected:
	virtual void showEvent(QShowEvent *);

private slots:
	void toggleCall();

private:
	EntryphoneDevice *dev;
	EnablingButton *mute_button, *stairlight, *unlock_door;
	BtButton *cycle, *call_accept;
	QString mute_icon, call_icon;
	bool connected;
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
	void toggleCameraSettings();

	void closeCall();
	void status_changed(const StatusList &sl);

private:
	QHBoxLayout *buildBottomLayout();
	void closePage();

	BtButton *setup_vct;
	CameraMove *camera;
	CameraImageControl *image_control;
	QLabel *video_box;
	CallControl *call_control;
	QString setup_vct_icon;
	bool camera_settings_shown;
	EntryphoneDevice *dev;
	QProcess video_grabber;
	Page *prev_page;
};

#endif //VCTCALLPAGE_H
