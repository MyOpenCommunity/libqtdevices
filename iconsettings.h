#ifndef ICON_SETTINGS_H
#define ICON_SETTINGS_H

#include "iconpage.h"

#include <QWidget>

class QDomNode;
class BtButton;
class banner;


class IconSettings : public IconPage
{
Q_OBJECT
public:
	IconSettings(const QDomNode &config_node);

	virtual int sectionId();
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};

// this can be a generic class
class ListPage : public BannerPage
{
public:
	ListPage(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};


class RingtonesPage : public ListPage
{
Q_OBJECT
public:
	RingtonesPage(const QDomNode &config_node);

private slots:
	void stopRingtones();
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
