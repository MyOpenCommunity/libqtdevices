/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This is a container for other pages.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef PAGECONTAINER_H
#define PAGECONTAINER_H

#include "page.h"

#include <QButtonGroup>
#include <QHash>

class QDomNode;

/**
 * \class PageContainer
 *
 * A container for other pages. Show a list of buttons, each of them is automatically
 * connected (in addPage method) with the correspondent page through the slot
 * 'showPage' and signal 'Closed'.
 */
class PageContainer : public Page
{
Q_OBJECT
public:
	PageContainer(const QDomNode &config_node);
	virtual void addBackButton();

protected:
	void addPage(Page *page, int id, QString iconName, int x, int y);

private:
	QButtonGroup buttons_group;
	QHash<int, Page*> page_list;
	void loadItems(const QDomNode &config_node);

private slots:
	void clicked(int id);
};

#endif // PAGECONTAINER_H
