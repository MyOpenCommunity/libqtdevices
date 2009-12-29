#ifndef VCTCALLPAGE_H
#define VCTCALLPAGE_H

#include "page.h"
#include "btbutton.h"

#include <QStringList>
#include <QProcess>

class QDomNode;
class EntryphoneDevice;
class QHBoxLayout;
class BannTuning;
class QLabel;


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


// TODO: should be a StyledWidget??
class CameraMove : public QWidget
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
	BannTuning *contrast, *brightness, *color;
};


class CallControl : public QWidget
{
Q_OBJECT
public:
	CallControl(EntryphoneDevice *d);

private:
	EntryphoneDevice *dev;
	EnablingButton *call_accept, *mute_button, *stairlight, *unlock_door;
	BtButton *cycle;
	QString mute_icon, call_icon;
};



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
