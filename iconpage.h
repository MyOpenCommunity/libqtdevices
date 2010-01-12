/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef ICONPAGE_H
#define ICONPAGE_H

#include "page.h"
#include "gridcontent.h"

#include <QButtonGroup>
#include <QHash>
#include <QList>

class IconContent;
class BtButton;

/**
 * \class IconPage
 *
 * Show a list of buttons, each of them is automatically connected (in addPage method)
 * with the correspondent page through the slot 'showPage' and signal 'Closed'.
 */
class IconPage : public Page
{
Q_OBJECT
public:
	// the type returned by page_content
	typedef IconContent ContentType;

	IconPage();
	virtual void addBackButton();
	virtual void activateLayout();

protected:
	void buildPage(IconContent *content, NavigationBar *nav_bar, const QString &label = QString());
	void addPage(Page *page, int id, const QString &label, const QString &iconName, int x = 0, int y = 0);
	BtButton *addButton(const QString &label, const QString &iconName, int x = 0, int y = 0);

private:
	QButtonGroup buttons_group;
	QHash<int, Page*> page_list;

protected slots:
	void clicked(int id);
};


/**
 * The IconContent class manages a grid of buttons.
 */
class IconContent : public GridContent
{
friend void IconPage::activateLayout();
Q_OBJECT
public:
	IconContent(QWidget *parent=0);
	void addButton(QWidget *button, const QString &label = QString());
	void addWidget(QWidget *widget);

protected:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent();

private:
	QList<QWidget*> items;
};

#endif // ICONPAGE_H
