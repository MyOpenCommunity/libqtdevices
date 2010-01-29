#ifndef MULTIMEDIA_H
#define MULTIMEDIA_H

#include "iconpage.h"

class QDomNode;


class MultimediaSectionPage : public IconPage
{
Q_OBJECT
public:
	MultimediaSectionPage(const QDomNode &config_node);

	virtual int sectionId();

private:
	void loadItems(const QDomNode &config_node);
};

#endif // MULTIMEDIA_H

