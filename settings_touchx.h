#ifndef SETTINGS_TOUCHX_H
#define SETTINGS_TOUCHX_H

#include "pagecontainer.h"

#include <QWidget>

class QDomNode;


class SettingsTouchX : public PageContainer
{
Q_OBJECT
public:
	SettingsTouchX(const QDomNode &config_node);

	virtual int sectionId();

private:
	void loadItems(const QDomNode &config_node);
};

#endif // SETTINGS_TOUCHX_H
