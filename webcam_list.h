#ifndef WEBCAM_LIST_H
#define WEBCAM_LIST_H

#include "page.h"
#include "itemlist.h"

class QDomNode;


/**
 * This page shows the list of webcam, taking them from the configuration file.
 */
class WebcamListPage : public Page
{
Q_OBJECT
public:
	WebcamListPage(const QDomNode &config_node);

	typedef ItemList ContentType;

private slots:
	void itemIsClicked(int index);

private:
	QList<ItemList::ItemInfo> webcam_list;
	void loadWebcams(const QDomNode &config_node);
};

#endif // WEBCAM_LIST_H
