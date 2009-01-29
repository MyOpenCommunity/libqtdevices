#include "pagecontainer.h"
#include "btbutton.h"
#include "btmain.h" // bt_global::btmain
#include "xml_functions.h" // getTextChild, getChildren
#include "generic_functions.h" // rearmWDT

#include <QDomNode>
#include <QDebug>
#include <QTime>

#define DIM_BUT 80
#define BACK_BUTTON_X    0
#define BACK_BUTTON_Y  250
#define BACK_BUTTON_DIM 60
#define IMG_BACK_BUTTON     IMG_PATH "arrlf.png"


PageContainer::PageContainer(const QDomNode &config_node) : buttons_group(this)
{
	connect(&buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
	loadItems(config_node);
}

void PageContainer::loadItems(const QDomNode &config_node)
{
	QTime wdtime;
	wdtime.start(); // Start counting for wd refresh

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		// Within the pagemenu element, it can exists items that are not a page.
		if (Page *p = bt_global::btmain->getPage(id))
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

void PageContainer::addBackButton()
{
	BtButton *b = new BtButton(this);
	b->setGeometry(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_DIM, BACK_BUTTON_DIM);
	b->setImage(IMG_BACK_BUTTON);
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));
}

void PageContainer::clicked(int id)
{
	page_list[id]->showPage();
	//hide();
}

