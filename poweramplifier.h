#ifndef POWER_AMPLIFIER_H
#define POWER_AMPLIFIER_H

#include "bannregolaz.h"

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

#endif
