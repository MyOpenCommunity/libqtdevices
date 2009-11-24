#ifndef ICON_SETTINGS_H
#define ICON_SETTINGS_H

#include "iconpage.h"

#include <QWidget>

class QDomNode;


class IconSettings : public IconPage
{
Q_OBJECT
public:
	IconSettings(const QDomNode &config_node);

	virtual int sectionId();

private:
	void loadItems(const QDomNode &config_node);
};

#endif // ICON_SETTINGS_H
