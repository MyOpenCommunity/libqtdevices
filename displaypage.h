#ifndef DISPLAYPAGE_H
#define DISPLAYPAGE_H

#include "page.h"

class QDomNode;


class DisplayPage : public Page
{
Q_OBJECT
public:
	DisplayPage(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};


#endif // DISPLAYPAGE_H
