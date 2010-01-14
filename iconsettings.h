#ifndef ICON_SETTINGS_H
#define ICON_SETTINGS_H

#include "iconpage.h"

#include <QWidget>

class QDomNode;
class BtButton;


class IconSettings : public IconPage
{
Q_OBJECT
public:
	IconSettings(const QDomNode &config_node);

	virtual int sectionId();

private:
	void loadItems(const QDomNode &config_node);
};


// button to toggle on/off the beep sound
class ToggleBeep : public QWidget
{
Q_OBJECT
public:
	ToggleBeep(bool status, QString label, QString icon_on, QString icon_off);

private slots:
	void toggleBeep();

private:
	BtButton *button;
};

class PasswordPage : public BannerPage
{
Q_OBJECT
public:
	PasswordPage(const QDomNode &config_node);
};

#endif // ICON_SETTINGS_H
