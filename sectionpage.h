#ifndef SECTION_PAGE_H
#define SECTION_PAGE_H

#include "iconpage.h"

class QDomNode;


/**
 * \class SectionPage
 *
 * A subclass of IconPage that can be used to collect sections
 */
class SectionPage : public IconPage
{
Q_OBJECT
public:
	SectionPage(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};

#endif // SECTION_PAGE_H
