#ifndef LAN_SETTINGS
#define LAN_SETTINGS

#include "sottomenu.h"

class LanSettings : public sottoMenu
{
Q_OBJECT
public:
	LanSettings(QWidget *parent = 0);
signals:
	void Closed();
};

#endif
