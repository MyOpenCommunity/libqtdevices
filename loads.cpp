/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** loads.cpp
 **
 **
 **
 ****************************************************************/

#include "loads.h"
#include "generic_functions.h" // createMsgOpen
#include "xml_functions.h" // getChildren, getTextChild
#include "content_widget.h" // content_widget


bannLoads::bannLoads(Page *parent, QString indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannLoads::Attiva()
{
	sendFrame(createMsgOpen("3", "2", getAddress()));
}


Loads::Loads(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void Loads::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		Q_ASSERT_X(id == CARICO, "Loads::loadItems", "Type of item not handled on loads page!");
		QString where = getTextChild(item, "where");
		QString img = IMG_PATH + getTextChild(item, "cimg1");

		banner *b = new bannLoads(this, where, img);
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		b->Draw();
		content_widget->appendBanner(b);
	}
}
