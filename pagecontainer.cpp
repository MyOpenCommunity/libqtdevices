#include "pagecontainer.h"
#include "btbutton.h"
#include "global.h" // BTouch
#include "xml_functions.h" // getTextChild, getChildren
#include "generic_functions.h" // rearmWDT

#include <QDomNode>
#include <QDebug>
#include <QTime>

#define DIM_BUT 80


PageContainer::PageContainer(QDomNode config_node) : buttons_group(this)
{
	connect(&buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
	loadItems(config_node);
}

void PageContainer::loadItems(QDomNode config_node)
{
	QTime wdtime;
	wdtime.start(); // Start counting for wd refresh

	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		// Within the pagemenu element, it can exists items that are not a page.
		if (Page *p = BTouch->getPage(id))
			addPage(p, id, img1, x, y);

		if (wdtime.elapsed() > 1000)
		{
			wdtime.restart();
			rearmWDT();
		}
	}
}

void PageContainer::addPage(Page *page, int id, QString iconName, int x, int y)
{
	BtButton *b = new BtButton(this);
	b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	b->setImage(iconName);

	buttons_group.addButton(b, id);
	page_list[id] = page;
	// TODO: attualmente chiudere la pagina contenitore e riaprirla sul Closed
	// crea un aggiornamento grafico piuttosto brutto.. capire quale e' il mdo
	// giusto di risolverlo!
	//connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
	connect(page, SIGNAL(Closed()), page, SLOT(hide()));
}

void PageContainer::clicked(int id)
{
	page_list[id]->showPage();
	//hide();
}

