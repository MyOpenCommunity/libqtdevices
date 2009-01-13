#ifndef DISPLAYPAGE_H
#define DISPLAYPAGE_H

#include "sottomenu.h"


class DisplayPage : public sottoMenu
{
Q_OBJECT
public:
	DisplayPage(QDomNode config_node);

private:
	void loadItems(QDomNode config_node);
};


#endif // DISPLAYPAGE_H
