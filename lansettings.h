#ifndef LAN_SETTINGS_H
#define LAN_SETTINGS_H

#include "page.h"

class BtButton;
class QDomNode;
class QLabel;


class LanSettings : public Page
{
Q_OBJECT
public:
	LanSettings(const QDomNode &config_node);
private:
	BtButton *back_btn, *activate_btn;
	QLabel *box_text;
};

#endif // LAN_SETTINGS_H
