#ifndef VCTCALLPAGE_H
#define VCTCALLPAGE_H

#include "page.h"
#include "bann2_buttons.h"
#include "btbutton.h"

#include <QStringList>

class QDomNode;
class EntryphoneDevice;


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


class BannTuning : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannTuning(const QString &banner_text, const QString &icon_name, QWidget *parent = 0);

private slots:
	void decreaseLevel();
	void increaseLevel();

private:
	void changeIcons();
	int current_level;
	QString center_icon;

signals:
	void valueChanged(int);
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



class VCTCallPage : public Page
{
Q_OBJECT
public:
	VCTCallPage(const QDomNode &config_node);
	void addExternalPlace(const QString &where);

private slots:
	void toggleCameraSettings();
	void setBrightness(int value);
	void setColor(int value);
	void setContrast(int value);

private:
	BtButton *cycle, *setup_vct;
	EnablingButton *call_accept, *mute_button, *stairlight, *unlock_door;
	BannTuning *contrast, *brightness, *color;
	CameraMove *camera;
	QString mute_icon, call_icon, setup_vct_icon;
	QStringList places;
	bool camera_settings_shown;
	EntryphoneDevice *dev;
};
#endif //VCTCALLPAGE_H
