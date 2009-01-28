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


class PageContainer : public Page
{
Q_OBJECT
public:
	PageContainer(const QDomNode &config_node);
	void addPage(Page *page, int id, QString iconName, int x, int y);
	virtual void addBackButton();
private:
	QButtonGroup buttons_group;
	QHash<int, Page*> page_list;
	void loadItems(const QDomNode &config_node);
private slots:
	void clicked(int id);
};

#endif // PAGECONTAINER_H
