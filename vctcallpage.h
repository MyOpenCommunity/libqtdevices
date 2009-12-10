#ifndef VCTCALLPAGE_H
#define VCTCALLPAGE_H

#include "page.h"
#include "bann2_buttons.h"

#include <QStringList>

class QDomNode;

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



class VCTCallPage : public Page
{
Q_OBJECT
public:
	VCTCallPage();
	void addExternalPlace(const QString &where);

private slots:
	void toggleCameraSettings();

private:
	BtButton *mute_button, *call_accept, *stairlight, *unlock_door, *cycle, *setup_vct;
	QString mute_icon, call_icon, unlock_icon, stairlight_icon, setup_vct_icon;
	QStringList places;
};
#endif //VCTCALLPAGE_H
