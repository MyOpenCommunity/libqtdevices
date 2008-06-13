#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"
#include "bannonoff.h"
#include "bannonoff2scr.h"

class sottoMenu;

/*****************************************************************
 **Power amplifier
 ****************************************************************/
/*!
 * \class PowerAmplifier
 * \brief This class is made to manage a power audio amplifier.
 *
 */
class PowerAmplifier : public bannRegolaz
{
Q_OBJECT
public:
	PowerAmplifier(QWidget *parent, const char *name, char* indirizzo, char* onIcon, char* offIcon, char *onAmpl, char *offAmpl, char* settingIcon);
	void setBGColor(QColor c);
	void setFGColor(QColor c);

private slots:
	void showSettings();
	void toggleStatus();
	void turnUp();
	void turnDown();
private:
	QString off_icon, on_icon;
	bool status;
	sottoMenu *settings_page;
};


class PowerAmplifierPreset : public bannOnOff
{
Q_OBJECT
public:
	PowerAmplifierPreset(QWidget *parent=0, const char *name=NULL);

private slots:
	void nextPreset();
	void prevPreset();
};

class PowerAmplifierTreble : public bannOnOff2scr
{
Q_OBJECT
public:
	PowerAmplifierTreble(QWidget *parent=0, const char *name=NULL);

private slots:
	void up();
	void down();

private:
	int level;
	void showLevel();
};


#endif
