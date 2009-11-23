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
class QGridLayout;
class IconContent;

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
	// the type returned by page_content
	typedef IconContent ContentType;

	PageContainer();
	virtual void addBackButton();

protected:
	void buildPage(IconContent *content, NavigationBar *nav_bar, const QString &label = QString());
	void addPage(Page *page, int id, QString iconName, int x = 0, int y = 0);

private:
	QButtonGroup buttons_group;
	QHash<int, Page*> page_list;

protected slots:
	void clicked(int id);
};


/**
 * The IconContent class manages a grid of buttons.
 */
class IconContent : public QWidget
{
Q_OBJECT
public:
	IconContent(QWidget *parent=0);
	void addButton(QWidget *button);

public slots:
	void pgUp();
	void pgDown();
	void resetIndex();

signals:
	void displayScrollButtons(bool display);

protected:
	void showEvent(QShowEvent *e);

private:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	void drawContent();

	int pageCount() const;

private:
	bool need_update; // a flag to avoid useless call to drawContent

	int current_page;
	QList<int> pages;
	QList<QWidget*> items;
};


/**
 * \class SectionPageContainer
 *
 * A subclass of PageContainer that loads pages at construction time using getPage
 */
class SectionPageContainer : public PageContainer
{
Q_OBJECT
public:
	SectionPageContainer(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};

#endif // PAGECONTAINER_H
